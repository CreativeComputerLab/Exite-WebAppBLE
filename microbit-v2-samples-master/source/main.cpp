/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "MicroBit.h"
#include "MicrobitBLEExcite.h"
#include "MicroBitSetAllService.h"
//#include <string>
#include <cstring>
// May only be needed in setallservice
//#include "ble_gap.h"
//#include "ble_gatt.h"

const int FIRMWARE_VERSION = 4;
const int FIRMWARE_SIZE = 4;

MicroBit uBit;

// one pin is represented by one int
// Max 20 byte packets over serial means 5 ints can fit per frame
const int PINS_PER_FRAME = 5;
const int DATA_LEN_BYTES = PINS_PER_FRAME * 4;    // 5 pins per serial  transfer * 4 bytes per pin = 20 byte  serial packet transfer size.
const int PIN_DATA_SIZE = DATA_LEN_BYTES * 4;  // 5 pins per frame * 4 serial packets = 20 pins @4 bytes per pin = 80 bytes
const int TOTAL_PINS_NUM = 25; // 20 microbit pins + extra frame
const int TOTAL_WORKING_PINS = 17; // pins 0 - 16 are the GPIO pin set.  Array indices 17-19 are for other command data.

int notificationData[TOTAL_PINS_NUM] = {};
int setAllData[TOTAL_PINS_NUM] = {};
bool notificationsOn = false;
bool distSensorLoop = false;
bool bootedUp = false;
//const int NOTIFICATION_DATA_CYCLE = 30;  //ms
const int NOTIFICATION_DATA_CYCLE = 50;  //ms

// commands to device
const int DISPLAY_COMMAND = 17; //17th setAll array index contains microbit display commands
const int PRINT_COMMAND = 20; 
const int STOP_COMMAND = 24; 
const int STOP_NOTIFICATIONS = 0x10;
const int START_NOTIFICATIONS = 0x20;
const int FIRMWARE_VERSION_QUERY = 0x40;
const int STOP_ALL = 0x30;

// Masks for reading pin data
const	int VALUE_MASK = 0xffff;  // 0-15 LSB bits of int value
const	int MODE_MASK =                    0x10000;  // bit 16  Analog or Digital Mode  analog = 0, digital = 1
const	int INPUT_PIN_MASK =               0x20000;  // bit 17
const	int SERVO_PIN_MASK =               0x40000;  // bit 18
const	int SERVO_PULSE_PIN_MASK =         0x80000;  // bit 19
const	int ANALOG_PULSE_PIN_MASK =       0x100000;  // bit 20
const	int DIGITAL_PULSE_PIN_MASK =      0x200000;  // bit 21
const	int DIGITAL_PULSE_HIGH_LOW_MASK = 0x400000;  // bit 22
const	int IS_TOUCHED_PIN_MASK =         0x800000;  // bit 23  - this means fresh data
const	int FRAME_NUMBER_MASK =         0x70000000;  // bits 30-28 - This is the frame number of the first byte in the 20 byte xfer
                                                     // Frame 0/Pin0 = 0, Frame 1/Pin5 = 1, Frame 2/Pin10 = 2, Frame 3/Pin15 = 3, etc.

// microbit display command masks contained in the setAll[17] int.
// Note the MSB is set to 1 to indicate fresh data
const	int ENABLE_LED_DISPLAY = 0xC0000000;
const	int DISABLE_LED_DISPLAY = 0xA0000000;
const	int PRINT_LED_DISPLAY = 0x90000000;
uint8_t charBuf[12] = {};  // GLobal buffer for chars. Declared globally to save memory

// TODO:  auto detect the user's desired data transport and support it.
bool	usingSerial = false;
bool 	usingBLE = true;
MicroBitSetAllService* mbs;

MicroBitImage STOPPED("\
        1 0 0 0 1\n\
        0 1 0 1 0\n\
        0 0 1 0 0\n\
        0 1 0 1 0\n\
        1 0 0 0 1\n");


MicroBitImage BLE("\
        0 0 1 1 0\n\
        1 1 1 0 1\n\
        0 1 1 1 0\n\
        1 1 1 0 1\n\
        0 0 1 1 0\n");

MicroBitImage smiley("0,0,0,0, 0\n0,255,0,255,0\n0,0,0,0,0\n32,0,0,0,32\n0,32,32,32,0\n");
MicroBitImage smileyWink("0,0,0,0, 0\n0,255,0,0,0\n0,0,0,0,0\n32,0,0,0,32\n0,32,32,32,0\n");
MicroBitImage waiting("32,32,32,32, 32\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n");


MicroBitImage image(5,5);

int analogPulseBits = 0;

int BLEconnected = 0;
MicrobitBLEExcite*  mbe;

// BLE
void onConnected(MicroBitEvent)
{
	BLEconnected = 1;
	uBit.display.clear();
    uBit.display.print(BLE);
    uBit.sleep(2000);
    uBit.display.disable();
    uBit.serial.printf("\nConnected\n");

}

void onDisconnected(MicroBitEvent)
{
    uBit.serial.printf("\nDisconnected\n");
    BLEconnected = 0;
    uBit.display.clear();
    
}



bool isDigitalPin (int pinData) {
    // THis is little endian
    int val = pinData & MODE_MASK;
    return val == MODE_MASK; // bit 16 0x10000 big endian
}

bool isInputPin (int pinData) {
    // THis is little endian
    int val = pinData & INPUT_PIN_MASK;
    return val == INPUT_PIN_MASK; // bit 17  0x20000
}

// TODO  define the masks as above and apply to these functions below
bool isServoPin (int pinData) {
    // THis is little endian
    int val = pinData & SERVO_PIN_MASK;
    return val == SERVO_PIN_MASK;  // bit 18  0x40000
}

bool isServoPulsePin (int pinData) {
    // THis is little endian
    int val = pinData & SERVO_PULSE_PIN_MASK;
    return val == SERVO_PULSE_PIN_MASK;  // bit 19 0x80000
}

bool isAnalogPulsePin (int pinData) {
    // THis is little endian
    int val = pinData & ANALOG_PULSE_PIN_MASK;
    return val == ANALOG_PULSE_PIN_MASK; // bit 20  0x100000
}

bool isDigitalPulsePin (int pinData) {
    // THis is little endian
    int val = pinData & DIGITAL_PULSE_PIN_MASK;
    return val == DIGITAL_PULSE_PIN_MASK; // bit 21  0x200000
}

// not used in favor of isBitSet()
bool isDigitalPulseHigh (int pinData) {
    // THis is little endian
    int val = pinData & DIGITAL_PULSE_HIGH_LOW_MASK;
    return val == DIGITAL_PULSE_HIGH_LOW_MASK; // bit 22  0x400000
}


bool isTouched (int pinData) {
    // THis is little endian
    int val = pinData & IS_TOUCHED_PIN_MASK;
    return val == IS_TOUCHED_PIN_MASK; // bit 23  0x800000
}

int setBit(int number, int n) {
	return number |= 1U << n;
}

int clearBit(int number, int n) {
	return number &= ~(1U << n);
}

bool isBitSet (int number, int n) {
	return (number >> n) & 1U;
}

bool isAnalogPulsePinRead (int pinBits, int pinNum) {
    return isBitSet(pinBits, pinNum);
}

void setFrameNumber(int frameNum) {
	int index = frameNum * 5;
	notificationData[index]|= ((frameNum << 28) & FRAME_NUMBER_MASK);

}

int getFrameNumber(uint8_t *bytes) {
	// TODO - Find out whether its byte 0 or 3 due to endianess
	return (bytes[3] & 0x7);
}


void setPinNotification(MicroBitPin pin, int pinIndex) {

	if (isAnalogPulsePin(notificationData[pinIndex]) && (isInputPin(notificationData[pinIndex]))){
		// Not Needed as these are set in setPins
		notificationData[pinIndex] |= ANALOG_PULSE_PIN_MASK; 
		notificationData[pinIndex] |= INPUT_PIN_MASK; 
		return;
	}

	if (isDigitalPulsePin(notificationData[pinIndex]) && (isInputPin(notificationData[pinIndex]))){
		// Not Needed as these are set in setPins
		notificationData[pinIndex] |= DIGITAL_PULSE_PIN_MASK; 
		notificationData[pinIndex] |= INPUT_PIN_MASK; // Set the 17th input bit to 1
		notificationData[pinIndex] |= MODE_MASK; // Set the 16th mode bit to 1
		return;
	}  

	if (pin.isAnalog()){
		// Analog Pin
		if (pin.isInput()){
			// Analog Input Pin (Read / Get)  get the value
			notificationData[pinIndex] =  pin.getAnalogValue() & VALUE_MASK;
			// Set the appropriate bits
			notificationData[pinIndex] |= INPUT_PIN_MASK; // Set the 17th input bit to 1 for input type			
		} 
		else {
			//Analog Input Pin (Write / Set) No Value expected so set it to 0
			notificationData[pinIndex] = 0 ; // the whole int is zeroed out. No need to set subsequent mask bits to 0 ??
			// Set the appropriate bits
			notificationData[pinIndex] &= ~(INPUT_PIN_MASK); // Set the 17th input bit to 0
		}
		notificationData[pinIndex] &=  ~(MODE_MASK); // set the 16th mode bit to 0
	}
	else if (pin.isDigital()){ 
		// Digital Pin
			if (pin.isInput()){		
				// Digital Pin (read / Get)
				notificationData[pinIndex] =  pin.getDigitalValue() & VALUE_MASK;
				// Set the appropriate bits
				notificationData[pinIndex] |= INPUT_PIN_MASK; // Set the 17th input bit to 1	
			}
			else {
				//Digital Output Pin (Write / Set) No Value expected so set it to 0
				notificationData[pinIndex] = 0;
				// Set the appropriate bits
				notificationData[pinIndex] &= ~(INPUT_PIN_MASK); // Set the 12th input bit to 0
			}
			notificationData[pinIndex] |= MODE_MASK; // Set the 16th mode bit to 1
	} else {
		//uBit.display.print("X");
		//uBit.sleep(1000);		
	}
}


void getNotificationData () {
	
	setPinNotification(uBit.io.P0, 0);
	setPinNotification(uBit.io.P1, 1);
	setPinNotification(uBit.io.P2, 2);
	setPinNotification(uBit.io.P3, 3);
	setPinNotification(uBit.io.P4, 4);
	//notificationData[1] = 0x12345678;
	setFrameNumber(0);
	//uBit.serial.printf("NotificationData Frame 0: %x  %x  %x  %x  %x\n", notificationData[0], notificationData[1], notificationData[2], notificationData[3], notificationData[4]);

		
	setPinNotification(uBit.io.P5, 5);
	setPinNotification(uBit.io.P6, 6);
	setPinNotification(uBit.io.P7, 7);
	setPinNotification(uBit.io.P8, 8);
	setPinNotification(uBit.io.P9, 9);
	//notificationData[6] = 0xABABABAB;
	setFrameNumber(1);
	//uBit.serial.printf("NotificationData Frame 1: %x  %x  %x  %x  %x\n", notificationData[5], notificationData[6], notificationData[7], notificationData[8], notificationData[9]);

	
	setPinNotification(uBit.io.P10, 10);
	setPinNotification(uBit.io.P11, 11);
	//setPinNotification(uBit.io.P12, 12);  reserved by microbit, used by us!
	notificationData[12] = (int)(uBit.accelerometer.getGesture());  // orientation screenUp, et al.
	setPinNotification(uBit.io.P13, 13);
	setPinNotification(uBit.io.P14, 14);	
	//notificationData[11] = 0xCDEFCDEF;
	setFrameNumber(2);
	//uBit.serial.printf("NotificationData Frame 2: %x  %x  %x  %x  %x \n", notificationData[10], notificationData[11], notificationData[12], notificationData[13], notificationData[14]);

	
	setPinNotification(uBit.io.P15, 15);
	setPinNotification(uBit.io.P16, 16);

	// Non IO Pin Array entries used for other data.
	notificationData[17] = uBit.accelerometer.getX();
	notificationData[18] = uBit.accelerometer.getY();
	notificationData[19] = uBit.accelerometer.getZ();
	//notificationData[16] = 0xBEEFFACE;
	setFrameNumber(3);
	//uBit.serial.printf("NotificationData Frame 3: %x  %x  %x  %x  %x \n", notificationData[15], notificationData[16], notificationData[17], notificationData[18], notificationData[19]);

	
	// Frame 4 isn't being used yet
	//notificationData[21] = 0xC0FFEE00;
	setFrameNumber(4);
	//uBit.serial.printf("NotificationData Frame 4: %x  %x  %x  %x  %x \n", notificationData[20], notificationData[21], notificationData[22], notificationData[23], notificationData[24]);
}


// Send the notification data array to the driver over serial or BLE
void sendNotificationData () {
	//uBit.serial.clearTxBuffer();
	uint8_t *bytes = (uint8_t *) notificationData;  // Cast the int array to a byte array
	uint8_t buffer [TOTAL_PINS_NUM];

	// Send 20 bytes of data at a time via serial. 5 pins for each transfer
	std::memcpy(buffer, bytes, 20 );
	//uBit.serial.printf("\nbuffer Frame 0: %x  %x  %x  %x  %x %x  %x  %x  %x  %x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);

	if (usingBLE)
		mbs->sendNotificationData(buffer);
	else
		uBit.serial.send(buffer, DATA_LEN_BYTES);

	//uBit.sleep(10);
	std::memcpy(buffer, bytes + 20, 20 );
	//uBit.serial.printf("\nbuffer Frame 1: %x  %x  %x  %x  %x %x  %x  %x  %x  %x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	if (usingBLE)
		mbs->sendNotificationData(buffer);
	else
		uBit.serial.send(buffer, DATA_LEN_BYTES);

	//uBit.sleep(10);
	std::memcpy(buffer, bytes + 40, 20 );
	//uBit.serial.printf("\nbuffer Frame 2: %x  %x  %x  %x  %x %x  %x  %x  %x  %x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	if (usingBLE)
		mbs->sendNotificationData(buffer);
	else
		uBit.serial.send(buffer, DATA_LEN_BYTES);

	//uBit.sleep(10);
	std::memcpy(buffer, bytes + 60, 20 );
	//uBit.serial.printf("\nbuffer Frame 3: %x  %x  %x  %x  %x %x  %x  %x  %x  %x\n\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8], buffer[9]);
	if (usingBLE)
		mbs->sendNotificationData(buffer);
	else
		uBit.serial.send(buffer, DATA_LEN_BYTES);

	//uBit.sleep(10);
	std::memcpy(buffer, bytes + 80, 20 );
	if (usingBLE)
		mbs->sendNotificationData(buffer);
	else
		uBit.serial.send(buffer, DATA_LEN_BYTES);	
}


void notificationFiber () {
	while (notificationsOn) {
		getNotificationData();
		sendNotificationData();
		uBit.sleep(NOTIFICATION_DATA_CYCLE);
	}
	release_fiber();
}


int getPinIdFromPinNum(int pinNum) {
	int pinId;

	switch (pinNum) {
		case 0:  // pin0
			pinId = MICROBIT_ID_IO_P0;
		break;
		case 1:  // pin1
			pinId = MICROBIT_ID_IO_P1;
		break;
		case 2:  // pin2
			pinId = MICROBIT_ID_IO_P2;
		break;
		case 3:  // pin3
			pinId = MICROBIT_ID_IO_P3;
		break;							
		case 4:  // pin4
			pinId = MICROBIT_ID_IO_P4;
		break;
		case 5:  // pin
			pinId = MICROBIT_ID_IO_P5;
		break;
		case 6:  // pin
			pinId = MICROBIT_ID_IO_P6;
		break;
		case 7:  // pin
			pinId = MICROBIT_ID_IO_P7;
		break;		
		case 8:  // pin
			pinId = MICROBIT_ID_IO_P8;
		break;
		case 9:  // pin
			pinId = MICROBIT_ID_IO_P9;
		break;
		case 10:  // pin10
			pinId = MICROBIT_ID_IO_P10;
		break;		
		case 11:  // pin10
			pinId = MICROBIT_ID_IO_P11;
		break;
		/*  Pin 12 is reserved. Not sure if it will work or not */
		case 13:  // pin13
			pinId = MICROBIT_ID_IO_P13;
		break;
		case 14:  // pin14
			pinId = MICROBIT_ID_IO_P14;
		break;
		case 15:  // pin15
			pinId = MICROBIT_ID_IO_P15;
		break;
		case 16:  // pin16
			pinId = MICROBIT_ID_IO_P16;
		break;		
		default:
			return -1; // 
	}
	return pinId;
}

MicroBitPin* getPinFromPinNum(int pinNum) {
	MicroBitPin *pin = NULL;

	switch (pinNum) {
		case 0:  // pin0
			pin = &uBit.io.P0;
		break;
		case 1:  // pin1
			pin = &uBit.io.P1;
		break;
		case 2:  // pin2
			pin = &uBit.io.P2;
		break;
		case 3:  // pin3
			pin = &uBit.io.P3;
		break;							
		case 4:  // pin4
			pin = &uBit.io.P4;
		break;
		case 5:  // pin
			pin = &uBit.io.P5;
		break;
		case 6:  // pin
			pin = &uBit.io.P6;
		break;
		case 7:  // pin
			pin = &uBit.io.P7;
		break;		
		case 8:  // pin
			pin = &uBit.io.P8;
		break;
		case 9:  // pin
			pin = &uBit.io.P9;
		break;
		case 10:  // pin10
			pin = &uBit.io.P10;
		break;		
		case 11:  // pin10
			pin = &uBit.io.P11;
		break;
		/*  Pin 12 is reserved. Not sure if it will work or not */
		case 13:  // pin13
			pin = &uBit.io.P13;
		break;
		case 14:  // pin14
			pin = &uBit.io.P14;
		break;
		case 15:  // pin15
			pin = &uBit.io.P15;
		break;
		case 16:  // pin16
			pin = &uBit.io.P16;
		break;		
		default:
			return NULL; // 
	}
	return pin;
}



 void onPulse(MicroBitEvent evt) 
 { 
 		int pinNum = -1;

 		switch (evt.source) {
		case MICROBIT_ID_IO_P0:  // pin0
			pinNum = 0;
		break;
		case MICROBIT_ID_IO_P1:  // pin0
			pinNum = 1;
		break;
		case MICROBIT_ID_IO_P2:  // pin0
			pinNum = 2;
		break;
		case MICROBIT_ID_IO_P3:  // pin0
			pinNum = 3;
		break;
		case MICROBIT_ID_IO_P4:  // pin0
			pinNum = 4;
		break;
		case MICROBIT_ID_IO_P5:  // pin0
			pinNum = 5;
		break;
		case MICROBIT_ID_IO_P6:  // pin0
			pinNum = 6;
		break;
		case MICROBIT_ID_IO_P7:  // pin0
			pinNum = 7;
		break;	
		case MICROBIT_ID_IO_P8:  // pin0
			pinNum = 8;
		break;
		case MICROBIT_ID_IO_P9:  // pin0
			pinNum = 9;
		break;
		case MICROBIT_ID_IO_P10:  // pin0
			pinNum = 10;
		break;
		case MICROBIT_ID_IO_P11:  // pin0
			pinNum = 11;
		break;
		case MICROBIT_ID_IO_P12:  //Not Used
			pinNum = 12;
		break;
		case MICROBIT_ID_IO_P13:  // pin0
			pinNum = 13;
		break;
		case MICROBIT_ID_IO_P14:  // pin0
			pinNum = 14;
		break;
		case MICROBIT_ID_IO_P15:  // pin0
			pinNum = 15;
		break;
		case MICROBIT_ID_IO_P16:  // pin0
			pinNum = 16;
		break;		
		default:
			return;
	}
	notificationData[pinNum] = evt.timestamp & VALUE_MASK;
	notificationData[pinNum] |= DIGITAL_PULSE_PIN_MASK; 
	notificationData[pinNum] |= INPUT_PIN_MASK; // Set the 17th input bit to 1
	notificationData[pinNum] |= MODE_MASK; // Set the 16th mode bit to 1	
	notificationData[pinNum] |= DIGITAL_PULSE_HIGH_LOW_MASK;
 } 


void setDigitalPins (int pinNum, int value) {
	MicroBitPin *pin = NULL;
	int pinId = -1;

	pin = getPinFromPinNum(pinNum);
	if (pin != NULL) {
			
			if (isDigitalPulsePin(setAllData[pinNum])) {
				//uBit.display.print("S");
				int pulse = MICROBIT_PIN_EVT_PULSE_HI;
				if (isBitSet(setAllData[pinNum], 22)) {
					pulse = MICROBIT_PIN_EVT_PULSE_HI;
					notificationData[pinNum] |= DIGITAL_PULSE_HIGH_LOW_MASK;	
				} else {
					notificationData[pinNum] &=  ~(DIGITAL_PULSE_HIGH_LOW_MASK);
				}

				//https://github.com/lancaster-university/microbit-dal/issues/161
				//https://lancaster-university.github.io/microbit-docs/ubit/messageBus/
				//https://lancaster-university.github.io/microbit-docs/ubit/io/
				pinId = getPinIdFromPinNum(pinNum);
				pin->eventOn(MICROBIT_PIN_EVENT_ON_PULSE);
				uBit.messageBus.listen(pinId, pulse, onPulse);

				notificationData[pinNum] |= DIGITAL_PULSE_PIN_MASK; 
				notificationData[pinNum] |= INPUT_PIN_MASK; // Set the 17th input bit to 1
				notificationData[pinNum] |= MODE_MASK; // Set the 16th mode bit to 1
				//return;
			}  else {
				uBit.messageBus.ignore(pinId, MICROBIT_PIN_EVT_PULSE_HI, onPulse); // Remove the event associated with the pin
				if (isInputPin(setAllData[pinNum])) {
							//uBit.display.print("I");
							pin->getDigitalValue();   // This is not needed, we're only here because we're setting the pin configuration.

							// Not Needed as these are set in setPins
							notificationData[pinNum] &=  ~(DIGITAL_PULSE_PIN_MASK);
							notificationData[pinNum] &=  ~(DIGITAL_PULSE_HIGH_LOW_MASK);
							notificationData[pinNum] |= INPUT_PIN_MASK;
				} else {
						pin->setDigitalValue(value);

						// Not Needed as these are set in setPins
						notificationData[pinNum] &=  ~(DIGITAL_PULSE_PIN_MASK);
						notificationData[pinNum] &=  ~(DIGITAL_PULSE_HIGH_LOW_MASK);
						notificationData[pinNum] &=  ~(INPUT_PIN_MASK);						
				}	
			}
	}

}


void setAnalogPins (int pinNum, int value) {
	MicroBitPin *pin = NULL;
	switch (pinNum) {
		case 0:  // pin0
			pin = &uBit.io.P0;					
		break;
		case 1:  // pin1
			pin = &uBit.io.P1;
		break;
		case 2:  // pin2
			pin = &uBit.io.P2;	
		break;
		case 3:  // pin3
   			pin = &uBit.io.P3;	
		break;							
		case 4:  // pin4
			pin = &uBit.io.P4;
		break;
		case 10:  // pin10
			pin = &uBit.io.P10;
		break;		
		default:
			return; // do nothing if pin number isn't in range
	}

	if (pin != NULL) {
		if (isInputPin(setAllData[pinNum])) {
			//uBit.display.print("i");
			if (isAnalogPulsePin(setAllData[pinNum])) {
				//pin->getAnalogValue(); // configure the pin for analog input
				//pin->getAnalogPeriodUs();  // configure analog period pin and designate it as such for notifications
				
				// TODO SPecial hidden function call exposed in V1 include code. Leave it out for now
				//pin->obtainAnalogChannel();


				//pin->setServoPulseUs(12345);  // This should configure the pin to be an analog output pin

				// Not Needed as these are set in setPins
				notificationData[pinNum] |= ANALOG_PULSE_PIN_MASK; 
				notificationData[pinNum] |= INPUT_PIN_MASK;
			}
			else {
				pin->getAnalogValue(); // configure the pin for next read

				//uBit.serial.printf("Pin: %d:  Setting analog value for next iteration", pinNum);
				// Not Needed as these are set in setPins
				notificationData[pinNum] &=  ~(ANALOG_PULSE_PIN_MASK);
				notificationData[pinNum] |= INPUT_PIN_MASK;
			}
		}
		else if (isServoPin(setAllData[pinNum])) {
				pin->setServoValue(value);

				// Not Needed as these are set in setPins
				notificationData[pinNum] &=  ~(ANALOG_PULSE_PIN_MASK);
				notificationData[pinNum] &=  ~(INPUT_PIN_MASK);
			}
		else if (isServoPulsePin(setAllData[pinNum])) {
				pin->setServoPulseUs(value);

				// Not Needed as these are set in setPins
				notificationData[pinNum] &=  ~(ANALOG_PULSE_PIN_MASK);
				notificationData[pinNum] &=  ~(INPUT_PIN_MASK);
		}
		else if (isAnalogPulsePin(setAllData[pinNum])) {
				//uBit.display.print("i");
				pin->setAnalogPeriodUs(value);

				// Not Needed as these are set in setPins
				notificationData[pinNum] &=  ~(ANALOG_PULSE_PIN_MASK);
				notificationData[pinNum] &=  ~(INPUT_PIN_MASK);
			}
		else  { // The remaining pin configuration must be an analog output pin.
			pin->setAnalogValue(value);

			// Not Needed as these are set in setPins
			notificationData[pinNum] &=  ~(ANALOG_PULSE_PIN_MASK);
			notificationData[pinNum] &=  ~(INPUT_PIN_MASK);
			}	
	}
}


void setLEDBits (int data) {
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++) {
			image.setPixelValue(i, j, data & 0x1);
			data = data >> 1; 
		}

	uBit.display.print(image);
}



void runDisplayCommand (int commandInt) {
	uBit.serial.printf("\runDisplayCommand\n");
	int val = commandInt & PRINT_LED_DISPLAY;    
    if (val == PRINT_LED_DISPLAY)
    {
    	// parse bits 0 - 24 and turn on/off led accordingly
    	setLEDBits(commandInt);
    } else {
	    val = commandInt & ENABLE_LED_DISPLAY;
	    if (val == ENABLE_LED_DISPLAY) {
	    	uBit.display.enable();
	    } else {
			val = commandInt & DISABLE_LED_DISPLAY;
		    if (val == DISABLE_LED_DISPLAY) {
		    	uBit.display.clear();
		    	uBit.display.disable();
		    }
    	} 
    }  	
}

void runPrintCommand () { } // Hacked the original Serial command. 

void runPrintCommandBLE () {
	uBit.serial.printf("\nrunPrintCommandBLE");
	//uBit.serial.printf("\nSetAllData Frame 4: %x  %x  %x  %x  %x \n", setAllData[20], setAllData[21], setAllData[22], setAllData[23], setAllData[24]);

	//uint8_t charBuf[16] = {};  
	// Convert setAllData[20 -23] into byte array
	//std::memcpy(charBuf, &setAllData[20], 16 );

	
	// Convert setAllData[20 -23] into byte array
	std::memcpy(charBuf, &setAllData[21], 12 );
	//uBit.serial.printf("CharBuf : %d  %d  %d  %d  %d %d  %d  %d  %d  %d\n", charBuf[0], charBuf[1], charBuf[2], charBuf[3], charBuf[4], charBuf[5], charBuf[6], charBuf[7], charBuf[8], charBuf[9]);

	// first byte is size, remaining bytes are chars
	uint8_t size = charBuf[0];
	//uBit.serial.printf("\nString size : %d", size);

    //char str[(sizeof charBuf) +1];
    char str[size +1];
    std::memcpy(str, &charBuf[1], size);
    str[size] = 0; // Null termination.	
 	//uBit.serial.printf("\nString : %d  %d  %d  %d  %d %d  %d  %d \n", str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7]);

    ManagedString s(str);
    //uBit.serial.printf("\n%s", s);
	uBit.display.printAsync(s);	
}


void runStopCommand () {
	uBit.serial.printf("\runStopCommand");
	uBit.serial.printf("\nSetAllData Frame 4: %x  %x  %x  %x  %x \n", setAllData[20], setAllData[21], setAllData[22], setAllData[23], setAllData[24]);


	int pinId = -1;
	MicroBitPin *pin = NULL;

	uBit.display.enable();
	uBit.display.print("X");
	uBit.sleep(1000);

	uBit.display.clear();
	uBit.display.disable();	
	uBit.sleep(10);

	for (int i = 0; i < TOTAL_PINS_NUM; i++){
		if ((i < 17) && (i != 12)) { // pin set 0 - 16, excluding pin 12
			pin =  getPinFromPinNum(i);
			if (isDigitalPulsePin(setAllData[i])) {
				pinId = getPinIdFromPinNum(i);
				uBit.messageBus.ignore(pinId, MICROBIT_PIN_EVT_PULSE_HI, onPulse);
			} 
			pin->setDigitalValue(0);  // Setting all pins to digital zero turns everything off. 
		}

		setAllData[i] = 0;
		notificationData[i] = 0;
	}
}



// Iterate through setAll array and assign pins accordingly
// Byte 
void setPins () {
	int value;
	// iterate through sets of TOTAL_PINS_NUM pins
	for (int i = 0; i < TOTAL_WORKING_PINS; i++){
		if (setAllData[i] != 0){
			// GPIO pins 0-16
			//if (i < TOTAL_WORKING_PINS) {

				// Clear the info bits 
				notificationData[i] &= 0x0000FFFF;

				// with Reverse Endian conversion from driver
				value = (setAllData[i] & VALUE_MASK);
				// Digital Pins  mode = 1
				if (isDigitalPin(setAllData[i])) {
					setDigitalPins(i, value);
				}
				// Analog Pins
				else {
					setAnalogPins(i, value);
				}
			//}
		}
	}

	if (setAllData[DISPLAY_COMMAND] != 0){
			runDisplayCommand(setAllData[DISPLAY_COMMAND]);
	}

	if (setAllData[PRINT_COMMAND] != 0){
			runPrintCommand();
	}
 
	//if (setAllData[STOP_COMMAND] == 0xFFFFFFF){
	if (isBitSet(setAllData[STOP_COMMAND], 0)) {
			runStopCommand();
	}
}

// Iterate through a 5 pin data frame which has just arrived
//  
void setPinsInFrame (int frame) {
	int value;

	//uBit.serial.printf("setPinsInFrame: frame Number %d", frame);

	//uBit.display.print(frame);

	int startPin = frame * 5; // 5 pin frames
	int endPin = startPin + 5;

	// iterate through sets of TOTAL_PINS_NUM pins
	for (int i = startPin; i < endPin; i++){
		//uBit.serial.printf("Pin: %d, int Value = %x", i, setAllData[i]);
		if (setAllData[i] != 0){
			// GPIO pins 0-16
			if (i < TOTAL_WORKING_PINS) {

				// Clear the info bits 
				notificationData[i] &= 0x0000FFFF;

				// with Reverse Endian conversion from driver
				value = (setAllData[i] & VALUE_MASK);

				//uBit.serial.printf("Pin: %d, value = %x", i, value);
				// Digital Pins  mode = 1
				if (isDigitalPin(setAllData[i])) {
					setDigitalPins(i, value);
				}
				// Analog Pins
				else {
					setAnalogPins(i, value);
				}
			} else {
				// Pins 17, 20, 24
					if (i == DISPLAY_COMMAND)
						if (setAllData[DISPLAY_COMMAND] != 0){ /// TODO Elimate this check. setAllData is already checked for zero above
								runDisplayCommand(setAllData[DISPLAY_COMMAND]);
						}
					if (i == PRINT_COMMAND)
						if (setAllData[PRINT_COMMAND] != 0){
								runPrintCommandBLE();
						}
			
					if (i == STOP_COMMAND)
						if (isBitSet(setAllData[STOP_COMMAND], 0)) {
								uBit.serial.printf("\nStop Command\n");
								runStopCommand();
						}	
			}
		}
	}
}

void onSetAllData(MicroBitEvent evt) {
	//uBit.serial.printf("\nonSetAllData: %d\n", evt.value);
	setPinsInFrame(evt.value);   	
}


void sendFirmwareVersion () {
	uint8_t verBuf[FIRMWARE_SIZE] = {};
	std::memcpy(verBuf, &FIRMWARE_VERSION, FIRMWARE_SIZE );
	uBit.serial.send(verBuf, FIRMWARE_SIZE);
}


void serialListener () {
	uint8_t data[32] = {};
	uint8_t packetType;
	//uint8_t *setAllDataBytes = (uint8_t *) setAllData;

	while (1) {
		//int i = 0;
		int numBytesRead = -1;

		// First 5 int  frame
		numBytesRead = uBit.serial.read(data, DATA_LEN_BYTES);
		packetType = (uint8_t)(data[3]);  // byte 4 in little endian contains commands
		
		if (packetType == 0){   //  data from PC

			std::memcpy(&setAllData, data, numBytesRead);

			numBytesRead = uBit.serial.read(data, DATA_LEN_BYTES);	
			std::memcpy(&setAllData[5], data, numBytesRead ); 

			numBytesRead = uBit.serial.read(data, DATA_LEN_BYTES);	
			std::memcpy(&setAllData[10], data, numBytesRead ); 

			numBytesRead = uBit.serial.read(data, DATA_LEN_BYTES);	
			std::memcpy(&setAllData[15], data, numBytesRead ); 

			numBytesRead = uBit.serial.read(data, DATA_LEN_BYTES);	
			std::memcpy(&setAllData[20], data, numBytesRead ); 

			setPins();	
					
		} else {  
			// Other commands besides pin commands
			if (packetType == START_NOTIFICATIONS) {
				bootedUp = true;

				// make the smiley peep up from the bottom of the screen...
				for (int y=4; y >= 0; y--)
				{
				    uBit.display.image.paste(smiley,0,y);
				    uBit.sleep(500);
				}

				uBit.display.print(smileyWink);
				uBit.sleep(1000);
				uBit.display.print(smiley);
				uBit.sleep(1000);
				// Initiate periodic update of board and pin state sent over serial
				notificationsOn = true;
				create_fiber(notificationFiber);
				uBit.display.clear();
				uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE);
				uBit.display.disable();
			} 
			else if (packetType == STOP_NOTIFICATIONS) {
				//uBit.display.print(STOPPED);
				// Kill the thread
				notificationsOn = false;
				uBit.sleep(100);
			}
			else if (packetType == FIRMWARE_VERSION_QUERY) {
				//uBit.display.print("F");
				sendFirmwareVersion();
				uBit.sleep(100);
			}
			else {
				uBit.display.print("x");
				uBit.sleep(100);					
			}
		}
	}
	release_fiber();
}


void spinner() {
	while (!bootedUp) {
		uBit.display.print(waiting);
		uBit.sleep(250);
		uBit.display.clear();
		uBit.sleep(250);
	}
	release_fiber();
}


int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.display.enable();
    uBit.display.setDisplayMode(DISPLAY_MODE_GREYSCALE);//
    uBit.display.setDisplayMode(DISPLAY_MODE_BLACK_AND_WHITE); // Brighter

    //create_fiber(spinner);  // Causes false uprgrades to happen 

    // Initialize data structures
	for (int i = 0; i < TOTAL_PINS_NUM; i++){
		/*  Failed experiment - Causes Distance sensor to not work and does not eliminate LEDs lighting from devices.
		if (i < TOTAL_WORKING_PINS) {
			getPinFromPinNum(i)->setDigitalValue(0);
			notificationData[i] |= MODE_MASK;
		} */
		setAllData[i] = 0;
		notificationData[i] = 0;
	}
    
    for (int i=0;i<20;i++)
        uBit.serial.printf("\n");
    uBit.serial.printf("\nWelcome\n");
    for (int i=0;i<20;i++)
        uBit.serial.printf("\n");
	// BLE
    // Services/Pairing Config/Power Level
    //uBit.display.scroll("BLE");

    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_CONNECTED, onConnected);
    uBit.messageBus.listen(MICROBIT_ID_BLE, MICROBIT_BLE_EVT_DISCONNECTED, onDisconnected);

    uBit.messageBus.listen(MICROBIT_ID_BLE_NOTIFICATION_BUS, MICROBIT_EVT_ANY, onSetAllData, MESSAGE_BUS_LISTENER_IMMEDIATE);
  

    //new MicroBitAccelerometerService(*uBit.ble, uBit.accelerometer);
    //new MicroBitIOPinService(*uBit.ble, uBit.io);
   	//mbs = new MicroBitSetAllService(*uBit.ble, uBit.display, uBit.serial, setAllData);


	mbe = new MicrobitBLEExcite(uBit);
	mbe->printFriendlyName();

	uBit.display.print(smileyWink);
	uBit.sleep(1000);
	uBit.display.print(smiley);
	uBit.sleep(1000);
	// Initiate periodic update of board and pin state sent over serial
	notificationsOn = true;
	create_fiber(notificationFiber);





    // Listen on serial port
    //create_fiber(serialListener);

    // We're done, so just enter a power efficient sleep while we wait for an event.
    while (1) {
		while (BLEconnected !=1){
			uBit.display.enable();
			mbe->printFriendlyName(); 
			if (BLEconnected !=1) {
				uBit.display.print(smileyWink);
				uBit.sleep(1000);
			}
			if (BLEconnected !=1){
				uBit.display.print(smiley);
				uBit.sleep(1000);
			}			  	
		}
        uBit.sleep(1000);
    }
}


// Turn the negative error codes to positive as blocks don't display negative numbers
int debugStatus(int period) {
	if ((period == MICROBIT_NOT_SUPPORTED)
			|| (period == MICROBIT_INVALID_PARAMETER)
			|| (period == MICROBIT_CALIBRATION_IN_PROGRESS)
			|| (period == MICROBIT_CALIBRATION_REQUIRED)
			|| (period == MICROBIT_NO_RESOURCES)
			|| (period == MICROBIT_BUSY)
			|| (period == MICROBIT_CANCELLED)
			|| (period == MICROBIT_NO_DATA)) {
		return period *= -1;
	} 
	return period;
}