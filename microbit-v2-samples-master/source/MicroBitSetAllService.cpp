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
    setAllCharacteristicBuffer   = 0;
    setAllCommandCharacteristic = 0;
    
    // Register the base UUID and create the service.
    RegisterBaseUUID( bs_base_uuid);
    CreateService( serviceUUID);

    // Create the data structures that represent each of our characteristics in Soft Device.
    CreateCharacteristic( mbbs_cIdxNOTIFY, charUUID[ mbbs_cIdxNOTIFY],
                         (uint8_t *)&setAllCharacteristicBuffer,
                         sizeof(temperatureDataCharacteristicBuffer), sizeof(setAllCharacteristicBuffer),
                         microbit_propREAD | microbit_propNOTIFY);

    CreateCharacteristic( mbbs_cIdxCOMMAND, charUUID[ mbbs_cIdxCOMMAND],
                         (uint8_t *)&setAllCommandCharacteristicBuffer,
                         sizeof(setAllCommandCharacteristicBuffer), sizeof(setAllCommandCharacteristicBuffer),
                         microbit_propREAD | microbit_propWRITE);



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
    if (params->handle == valueHandle( mbbs_cIdxPERIOD) && params->len >= sizeof(temperaturePeriodCharacteristicBuffer))
    {
        memcpy(&temperaturePeriodCharacteristicBuffer, params->data, sizeof(temperaturePeriodCharacteristicBuffer));
        thermometer.setPeriod(temperaturePeriodCharacteristicBuffer);

        // The accelerometer will choose the nearest period to that requested that it can support
        // Read back the ACTUAL period it is using, and report this back.
        temperaturePeriodCharacteristicBuffer = thermometer.getPeriod();
        setChrValue( mbbs_cIdxPERIOD, (const uint8_t *)&temperaturePeriodCharacteristicBuffer, sizeof(temperaturePeriodCharacteristicBuffer));
    }
}

void MicroBitSetAllService::onDataRead( microbit_onDataRead_t *params)
{
    if ( params->handle == valueHandle( mbbs_cIdxNOTIFY))
    {}
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
