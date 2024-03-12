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
  * Class definition for the custom MicroBit Temperature Service.
  * Provides a BLE service to remotely read the silicon temperature of the nRF51822.
  */
#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitSetAllService.h"


const uint16_t MicroBitSetAllService::serviceUUID               = 0x0001;
const uint16_t MicroBitSetAllService::charUUID[ mbbs_cIdxCOUNT] = { 0x0002, 0x0003 };


/**
  * Constructor.
  * Create a representation of the Setall service
  * @param _ble The instance of a BLE device that we're running on.
  * @param _display An instance of MicroBitDisplay to interface with.
  */

MicroBitSetAllService::MicroBitSetAllService(BLEDevice &_ble, MicroBitDisplay &_display, MicroBitSerial &_serial, int *setAllData) :
        ble(_ble), display(_display), serial(_serial)
{
    // Initialise our characteristic values.
    //setAllCharacteristicBuffer   = 0;
    //setAllCommandCharacteristicBuffer = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxNOTIFY, charUUID[ mbbs_cIdxNOTIFY],
                         (uint8_t *)&setAllCharacteristicBuffer,
                         sizeof(setAllCharacteristicBuffer), sizeof(setAllCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxCOMMAND, charUUID[ mbbs_cIdxCOMMAND],
                         (uint8_t *)&setAllCommandCharacteristicBuffer,
                         sizeof(setAllCommandCharacteristicBuffer), sizeof(setAllCommandCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);


/*  THis is in V1 code, not sure its needed here TODO - find out
 ble.onDataWritten(this, &MicroBitSetAllService::onDataWritten);
*/

/* Not needed. Handled in main.cpp */
/*
    if ( getConnected())
        listen( true);
*/
}



/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitSetAllService::onDataWritten(const microbit_ble_evt_write_t *params)
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


    if (params->handle == valueHandle(mbbs_cIdxCOMMAND) && params->len == MICROBIT_BLE_MAX_PACKET_BYTES)
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

void MicroBitSetAllService::onDataRead( microbit_onDataRead_t *params)
{
    /* TODO find out if this is used to set the LED matrix
    
    if ( params->handle == valueHandle( mbbs_cIdxNOTIFY))
    {}
    */
}



void MicroBitSetAllService::sendNotificationData (uint8_t *notificationData)
{

    if ( getConnected())
    {
            notifyChrValue( mbbs_cIdxNOTIFY, notificationData, MICROBIT_BLE_MAX_PACKET_BYTES);
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



/**
  * Not Needed

void MicroBitTemperatureService::listen( bool yes)
{
    if (EventModel::defaultEventBus)
    {
        if ( yes)
        {
            // Ensure thermometer is being updated
            temperatureDataCharacteristicBuffer   = thermometer.getTemperature();
            temperaturePeriodCharacteristicBuffer = thermometer.getPeriod();
            EventModel::defaultEventBus->listen(MICROBIT_ID_THERMOMETER, MICROBIT_THERMOMETER_EVT_UPDATE, this, &MicroBitTemperatureService::temperatureUpdate, MESSAGE_BUS_LISTENER_IMMEDIATE);
        }
        else
        {
            EventModel::defaultEventBus->ignore(MICROBIT_ID_THERMOMETER, MICROBIT_THERMOMETER_EVT_UPDATE, this, &MicroBitTemperatureService::temperatureUpdate);
        }
    }
}


void MicroBitTemperatureService::onConnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( true);
}



void MicroBitTemperatureService::onDisconnect( const microbit_ble_evt_t *p_ble_evt)
{
    listen( false);
}


void MicroBitTemperatureService::temperatureUpdate(MicroBitEvent)
{
    if ( getConnected())
    {
        temperatureDataCharacteristicBuffer = thermometer.getTemperature();
        notifyChrValue( mbbs_cIdxDATA, (uint8_t *)&temperatureDataCharacteristicBuffer, sizeof(temperatureDataCharacteristicBuffer));
    }
}
*/
#endif
