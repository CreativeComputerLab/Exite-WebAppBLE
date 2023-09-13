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
  * 
  */

#include "MicrobitBLEExcite.h"

/**
  * Constructor.
  * Create a representation of the LEDService
  * @param _ble The instance of a BLE device that we're running on.
  * @param _display An instance of MicroBitDisplay to interface with.
  */
MicrobitBLEExcite::MicrobitBLEExcite(MicroBit &_uBit) :
    uBit(_uBit)
{
    
}


/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicrobitBLEExcite::printFriendlyName()
{
    //uBit.display.print(uBit.getName());
}

