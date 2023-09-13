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

#ifndef MICROBIT_SETALL_SERVICE_H
#define MICROBIT_SETALL_SERVICE_H

#include "MicroBitSerial.h"
#include "MicroBitConfig.h"
#include "BLE.h"
#include "MicroBitDisplay.h"

#include "GattCallbackParamTypes.h"
#include "Gap.h"
#include "GattAttribute.h"


// Defines the buffer size for scrolling text over BLE, hence also defines
// the maximum string length that can be scrolled via the BLE service.
#define MICROBIT_BLE_MAXIMUM_SCROLLTEXT         20
#define MICROBIT_BLE_MAX_PACKET_BYTES           20

#define MICROBIT_BLE_EVT_INCOMING_DATA_FRAME_1  1025
#define MICROBIT_BLE_EVT_INCOMING_DATA_FRAME_2  1026
#define MICROBIT_BLE_EVT_INCOMING_DATA_FRAME_3  1027
#define MICROBIT_BLE_EVT_INCOMING_DATA_FRAME_4  1028
#define MICROBIT_BLE_EVT_INCOMING_DATA_FRAME_5  1029
#define MICROBIT_ID_BLE_NOTIFICATION_BUS        1030

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitSetAllServiceUUID[];
extern const uint8_t  MicroBitSetAllNotificationUUID[];
extern const uint8_t  MicroBitSetAllCommandUUID[];

extern const uint8_t  MicroBitSetAllServiceMatrixUUID[];
extern const uint8_t  MicroBitSetAllServiceTextUUID[];
extern const uint8_t  MicroBitSetAllServiceScrollingSpeedUUID[];


/**
  * Class definition for the custom MicroBit LED Service.
  * Provides a BLE service to remotely read and write the state of the LED display.
  */
class MicroBitSetAllService
{
    public:

    /**
      * Constructor.
      * Create a representation of the LEDService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _display An instance of MicroBitDisplay to interface with.
      */
    MicroBitSetAllService(BLEDevice &_ble, MicroBitDisplay &_display, MicroBitSerial &_serial, int * setAllData);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const GattWriteCallbackParams *params);

    /**
      * Callback. Invoked when any of our attributes are read via BLE.
      */
    void onDataRead(GattReadAuthCallbackParams *params);


    void sendNotificationData (uint8_t *notificationData);

    private:
    void overlayDataFrame (int * bigArray, int * subArray, int frameNumber);

    // Bluetooth stack we're running on.
    BLEDevice           &ble;
    MicroBitDisplay     &display;
    MicroBitSerial      &serial;
    int                 *setAllData;

    // memory for our 8 bit control characteristics.
    uint8_t             matrixCharacteristicBuffer[MICROBIT_BLE_MAX_PACKET_BYTES];
    uint16_t            scrollingSpeedCharacteristicBuffer;
    uint8_t             textCharacteristicBuffer[MICROBIT_BLE_MAXIMUM_SCROLLTEXT];
    uint8_t             setAllCharacteristicBuffer[MICROBIT_BLE_MAX_PACKET_BYTES];
    uint8_t             setAllCommandCharacteristicBuffer[MICROBIT_BLE_MAX_PACKET_BYTES];

    // Handles to access each characteristic when they are held by Soft Device.
    GattAttribute::Handle_t setAllNotifyCharacteristicHandle;
    GattAttribute::Handle_t setAllCommandCharacteristicHandle;

    GattAttribute::Handle_t matrixCharacteristicHandle;
    GattAttribute::Handle_t textCharacteristicHandle;
    GattAttribute::Handle_t scrollingSpeedCharacteristicHandle;

    // local variable to indicate when to start processing incoming messages
    bool processIncomingData = false;


    // We hold a copy of the GattCharacteristic, as mbed's BLE API requires this to provide read callbacks (pity!).
    //GattCharacteristic  matrixCharacteristic;
};


#endif
