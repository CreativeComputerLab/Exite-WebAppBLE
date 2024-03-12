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

#include "MicroBitConfig.h"

#if CONFIG_ENABLED(DEVICE_BLE)

#include "MicroBitBLEManager.h"
#include "MicroBitBLEService.h"
#include "MicroBitSerial.h"
#include "MicroBitConfig.h"
#include "MicroBitDisplay.h"

// WE may need this is if events have changed
#include "EventModel.h"  // Defined in codal-core/inc

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

/**
  * Class definition for the custom MicroBit Temperature Service.
  * Provides a BLE service to remotely read the silicon temperature of the SoC.
  */
class MicroBitSetAllService : public MicroBitBLEService
{
    public:

    /**
      * Constructor.
      * Create a representation of the TemperatureService
      * @param _thermometer An instance of MicroBitThermometer to use as our temperature source.
      */
    MicroBitSetAllService(BLEDevice &_ble, MicroBitDisplay &_display, MicroBitSerial &_serial, int * setAllData);

    void sendNotificationData (uint8_t *notificationData);
    
    private:

    /**
      * Set up or tear down event listers
      */
    void listen( bool yes);

    /**
      * Invoked when BLE connects.
      */
    void onConnect( const microbit_ble_evt_t *p_ble_evt);

    /**
      * Invoked when BLE disconnects.
      */
    void onDisconnect( const microbit_ble_evt_t *p_ble_evt);

    /**
      * Callback. Invoked when any of our attributes are written via BLE.
      */
    void onDataWritten(const microbit_ble_evt_write_t *params);

    /**
      * Callback. Invoked when any of our attributes are read via BLE.
      */
    void onDataRead( microbit_onDataRead_t *params);

    void overlayDataFrame (int * bigArray, int * subArray, int frameNumber);


    // Bluetooth stack we're running on.
    BLEDevice           &ble;
    MicroBitDisplay     &display;
    MicroBitSerial      &serial;
    int                 *setAllData;

    // Note these may not be needed if we use the mbbs_cIdx enumeration
    // memory for our 8 bit control characteristics.
    uint8_t             setAllCharacteristicBuffer[MICROBIT_BLE_MAX_PACKET_BYTES];
    uint8_t             setAllCommandCharacteristicBuffer[MICROBIT_BLE_MAX_PACKET_BYTES];

    //uint32_t



    // Index for each charactersitic in arrays of handles and UUIDs
    typedef enum mbbs_cIdx
    {
        mbbs_cIdxNOTIFY,
        mbbs_cIdxCOMMAND,
        mbbs_cIdxCOUNT
    } mbbs_cIdx;
    
    // UUIDs for our service and characteristics
    static const uint16_t serviceUUID;
    static const uint16_t charUUID[ mbbs_cIdxCOUNT];


    /* Obsolete here for reference
    const uint8_t  MicroBitSetAllServiceUUID[] = {
      0xe9,0x5d,0x00,0x01,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
    };

    const uint8_t  MicroBitSetAllNotificationUUID[] = {
      0xe9,0x5d,0x00,0x02,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
    };

    const uint8_t  MicroBitSetAllCommandUUID[] = {
      0xe9,0x5d,0x00,0x03,0x25,0x1d,0x47,0x0a,0xa0,0x62,0xfa,0x19,0x22,0xdf,0xa9,0xa8
    };
    */

    // local variable to indicate when to start processing incoming messages
    bool processIncomingData = false;


/*****************************************************************/
    /* May not be needed below this line */

  /*
    MicroBitBLEChar      chars[ mbbs_cIdxCOUNT];

    public:
    
    int              characteristicCount()          { return mbbs_cIdxCOUNT; };
    MicroBitBLEChar *characteristicPtr( int idx)    { return &chars[ idx]; };
    */
};


#endif
#endif
