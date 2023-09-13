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

#ifndef MICROBIT_BLE_EXCITE_H
#define MICROBIT_BLE_EXCITE_H

#include "MicroBit.h"
#include "MicroBitConfig.h"
#include "BLE.h"
#include "MicroBitDisplay.h"

// Defines the buffer size for scrolling text over BLE, hence also defines
// the maximum string length that can be scrolled via the BLE service.
#define MICROBIT_BLE_MAXIMUM_SCROLLTEXT         20
#define MICROBIT_BLE_MAX_PACKET_BYTES           20

// UUIDs for our service and characteristics
extern const uint8_t  MicroBitSetAllServiceUUID[];
extern const uint8_t  MicroBitSetAllServiceMatrixUUID[];
extern const uint8_t  MicroBitSetAllServiceTextUUID[];
extern const uint8_t  MicroBitSetAllServiceScrollingSpeedUUID[];


/**
  * Class definition for the custom MicroBit LED Service.
  * Provides a BLE service to remotely read and write the state of the LED display.
  */
class MicrobitBLEExcite
{
    public:

    /**
      * Constructor.
      * Create a representation of the LEDService
      * @param _ble The instance of a BLE device that we're running on.
      * @param _display An instance of MicroBitDisplay to interface with.
      */
    MicrobitBLEExcite(MicroBit &_uBit);

    /**
      * 
      */
    void printFriendlyName();

    private:
      MicroBit     &uBit;

};


#endif
