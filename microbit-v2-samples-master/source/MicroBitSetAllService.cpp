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

/**
  * Class definition for the custom MicroBit LED Service.
  * Provides a BLE service to remotely read and write the state of the LED display.
  */
#include "MicroBitSerial.h"
#include "MicroBitConfig.h"
#include "UUID.h"
#include "GattCallbackParamTypes.h"

#include "MicroBitSetAllService.h"

/**
  * Constructor.
  * Create a representation of the LEDService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _display An instance of MicroBitDisplay to interface with.
  */
MicroBitSetAllService::MicroBitSetAllService(BLEDevice &_ble, MicroBitDisplay &_display, MicroBitSerial &_serial, int *setAllData) :
        ble(_ble), display(_display), serial(_serial)
{
    this->setAllData = setAllData;

    // Create the data structures that represent each of our characteristics in Soft Device.

    GattCharacteristic  setAllNotifyCharacteristic(MicroBitSetAllNotificationUUID, (uint8_t *)&setAllCharacteristicBuffer, 0,
    sizeof(setAllCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY);

    GattCharacteristic  setAllCommandCharacteristic(MicroBitSetAllCommandUUID, (uint8_t *)setAllCommandCharacteristicBuffer, 0, MICROBIT_BLE_MAX_PACKET_BYTES,
    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    ///// TODO Get rid of these

    GattCharacteristic  textCharacteristic(MicroBitSetAllServiceTextUUID, (uint8_t *)textCharacteristicBuffer, 0, MICROBIT_BLE_MAXIMUM_SCROLLTEXT,
    GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

    GattCharacteristic  scrollingSpeedCharacteristic(MicroBitSetAllServiceScrollingSpeedUUID, (uint8_t *)&scrollingSpeedCharacteristicBuffer, 0,
    sizeof(scrollingSpeedCharacteristicBuffer), GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ);

    // Initialise our characteristic values.
    memclr(matrixCharacteristicBuffer, sizeof(matrixCharacteristicBuffer));
    textCharacteristicBuffer[0] = 0;
    scrollingSpeedCharacteristicBuffer = MICROBIT_DEFAULT_SCROLL_SPEED;

    //matrixCharacteristic.setReadAuthorizationCallback(this, &MicroBitSetAllService::onDataRead);

    ///////


    // Set default security requirements
    //matrixCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    textCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    scrollingSpeedCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    setAllNotifyCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);
    setAllCommandCharacteristic.requireSecurity(SecurityManager::MICROBIT_BLE_SECURITY_LEVEL);

    GattCharacteristic *characteristics[] = {&setAllNotifyCharacteristic, &setAllCommandCharacteristic,     &textCharacteristic, &scrollingSpeedCharacteristic};
    GattService         service(MicroBitSetAllServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    setAllNotifyCharacteristicHandle = setAllNotifyCharacteristic.getValueHandle();
    setAllCommandCharacteristicHandle = setAllCommandCharacteristic.getValueHandle();

    /// Get Rid of these
    //matrixCharacteristicHandle = matrixCharacteristic.getValueHandle();
    textCharacteristicHandle = textCharacteristic.getValueHandle();
    scrollingSpeedCharacteristicHandle = scrollingSpeedCharacteristic.getValueHandle();
    ///

    //ble.gattServer().write(scrollingSpeedCharacteristicHandle, (const uint8_t *)&scrollingSpeedCharacteristicBuffer, sizeof(scrollingSpeedCharacteristicBuffer));
    //ble.gattServer().write(matrixCharacteristicHandle, (const uint8_t *)&matrixCharacteristicBuffer, sizeof(matrixCharacteristicBuffer));

    ble.onDataWritten(this, &MicroBitSetAllService::onDataWritten);
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  * This handles the incoming setAllData COMMAND array from Snap!  
  */
void MicroBitSetAllService::onDataWritten(const GattWriteCallbackParams *params)
{
    uint8_t *data = (uint8_t *)params->data;
    int *intArr = (int *) params->data;
    //int event;

/*
    serial.printf("onDataWritten Param Len:  %d\n", params->len);
    
    for (int i=0;i<params->len/4;i++)
        serial.printf("int: %x\n", intArr[i]);
    serial.printf("--\n" );   


    for (int i=0;i<params->len;i++)
        serial.printf("byte: %x\n", data[i]);
    serial.printf("--\n" ); 
*/


    if (params->handle == setAllCommandCharacteristicHandle && params->len == MICROBIT_BLE_MAX_PACKET_BYTES)
    {   
        // filter out connection event messages
        if (!processIncomingData) {
            if (data[0] == 0xff) {  // Signal to start processing subsequent incoming packets
                serial.printf("Setting processIncomingData = true");
                processIncomingData = true;
                return;   
            }
        } else {
            // Get the frame number from *data
            int frameNum = (data[3] & 0x7);      
            //serial.printf("onDataWritten Firing setall event  frame %d\n", frameNum);

            
            if (frameNum >= 0 && frameNum < 5) {
                //overlay the frame into *setAllData
                overlayDataFrame(setAllData, intArr, frameNum);

                // Fire the event
                MicroBitEvent evt(MICROBIT_ID_BLE_NOTIFICATION_BUS, frameNum);
            }
        }
    }
}

/**
  * Callback. Invoked when any of our attributes are read via BLE.
  */
void MicroBitSetAllService::onDataRead(GattReadAuthCallbackParams *params)
{
    if (params->handle == matrixCharacteristicHandle)
    {
        for (int y=0; y<5; y++)
        {
            matrixCharacteristicBuffer[y] = 0;

            for (int x=0; x<5; x++)
            {
                if (display.image.getPixelValue(x, y))
                    matrixCharacteristicBuffer[y] |= 0x01 << (4-x);
            }
        }

        ble.gattServer().write(matrixCharacteristicHandle, (const uint8_t *)&matrixCharacteristicBuffer, sizeof(matrixCharacteristicBuffer));
    }
}

void MicroBitSetAllService::sendNotificationData (uint8_t *notificationData)
{
    if (ble.getGapState().connected) {
        ble.gattServer().notify(setAllNotifyCharacteristicHandle, notificationData, MICROBIT_BLE_MAX_PACKET_BYTES);
    }
}


void MicroBitSetAllService::overlayDataFrame (int * bigArray, int * subArray, int frameNumber)
{
  int start = frameNumber * 5;  // 5 ints per frame
  int stop  = start + 5;
  int j = 0;
  int i;
  //serial.printf("overlayDataFrame:\n");
  for (i = start; i < stop; i++){
    bigArray[i] = subArray[j++];
    //serial.printf("subArray[%d]: %x\n",j-1, subArray[j-1]);
    //serial.printf("bigArray[%d]:%x\n",i, bigArray[i]);

  }
  /*
    for (int i=0;i<20;i++)
        serial.printf("overlayDataFrame SetAllData[%d]: %x\n", i, setAllData[i]);
    serial.printf("----\n" ); 
    */  
}


const uint8_t  MicroBitSetAllServiceUUID[] = {
    0xe9,0x5d,0x00,0x01,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitSetAllNotificationUUID[] = {
    0xe9,0x5d,0x00,0x02,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitSetAllCommandUUID[] = {
    0xe9,0x5d,0x00,0x03,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitSetAllServiceTextUUID[] = {
    0xe9,0x5d,0x00,0x04,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitSetAllServiceScrollingSpeedUUID[] = {
    0xe9,0x5d,0x00,0x05,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};

const uint8_t  MicroBitSetAllServiceMatrixUUID[] = {
    0xe9,0x5d,0x00,0x06,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
};
