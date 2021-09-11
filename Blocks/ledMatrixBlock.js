              // Do not send redundant command if the state has not changed.

              var matrixBits = 0;  // 32 bit int which contains bits indicating the desired state of the LED matrix.
              var params = [r1c1, r1c2, r1c3, r1c4, r1c5, r2c1, r2c2, r2c3, r2c4, r2c5, r3c1, r3c2, r3c3, r3c4, r3c5, r4c1, r4c2, r4c3, r4c4, r4c5, r5c1, r5c2, r5c3, r5c4, r5c5 ];
              var symbolCommand = new Uint8Array(4);

              symbolCommand[0] |= (1 << 6);  // This way a blank matrix is never 0 


              symbolCommand[0] = params[24] == true ?   (symbolCommand[0] | (1 << 0)) :   (symbolCommand[0] & ~(1 << 0)); // R5 C5


              symbolCommand[1] = params[19] == true ?   (symbolCommand[1] | (1 << 7)) :   (symbolCommand[1] & ~(1 << 7)); // R4 C5
              symbolCommand[1] = params[14] == true ?   (symbolCommand[1] | (1 << 6)) :   (symbolCommand[1] & ~(1 << 6)); // R3 C5
              symbolCommand[1] = params[9] == true ?   (symbolCommand[1] | (1 << 5)) :   (symbolCommand[1] & ~(1 << 5)); // R2 C5
              symbolCommand[1] = params[4] == true ?   (symbolCommand[1] | (1 << 4)) :   (symbolCommand[1] & ~(1 << 4)); // R1 C5

              symbolCommand[1] = params[23] == true ?   (symbolCommand[1] | (1 << 3)) :   (symbolCommand[1] & ~(1 << 3)); // R5 C4
              symbolCommand[1] = params[18] == true ?   (symbolCommand[1] | (1 << 2)) :   (symbolCommand[1] & ~(1 << 2)); // R4 C4
              symbolCommand[1] = params[13] == true ?   (symbolCommand[1] | (1 << 1)) :   (symbolCommand[1] & ~(1 << 1)); // R3 C4
              symbolCommand[1] = params[8] == true ?   (symbolCommand[1] | (1 << 0)) :   (symbolCommand[1] & ~(1 << 0)); // R2 C4



              symbolCommand[2] = params[3] == true ?   (symbolCommand[2] | (1 << 7)) :   (symbolCommand[2] & ~(1 << 7)); // R1 C4

              symbolCommand[2] = params[22] == true ?   (symbolCommand[2] | (1 << 6)) :   (symbolCommand[2] & ~(1 << 6)); // R5 C3
              symbolCommand[2] = params[17] == true ?   (symbolCommand[2] | (1 << 5)) :   (symbolCommand[2] & ~(1 << 5)); // R4 C3
              symbolCommand[2] = params[12] == true ?   (symbolCommand[2] | (1 << 4)) :   (symbolCommand[2] & ~(1 << 4)); // R3 C3
              symbolCommand[2] = params[7] == true ?   (symbolCommand[2] | (1 << 3)) :   (symbolCommand[2] & ~(1 << 3)); // R2 C3
              symbolCommand[2] = params[2] == true ?   (symbolCommand[2] | (1 << 2)) :   (symbolCommand[2] & ~(1 << 2)); // R1 C3

              symbolCommand[2] = params[21] == true ?   (symbolCommand[2] | (1 << 1)) :   (symbolCommand[2] & ~(1 << 1)); // R5 C2
              symbolCommand[2] = params[16] == true ?   (symbolCommand[2] | (1 << 0)) :   (symbolCommand[2] & ~(1 << 0)); // R4 C2



              symbolCommand[3] = params[11] == true ?   (symbolCommand[3] | (1 << 7)) :   (symbolCommand[3] & ~(1 << 7)); // R3 C2
              symbolCommand[3] = params[6] == true ?   (symbolCommand[3] | (1 << 6)) :   (symbolCommand[3] & ~(1 << 6)); // R2 C2 
              symbolCommand[3] = params[1] == true ?   (symbolCommand[3] | (1 << 5)) :   (symbolCommand[3] & ~(1 << 5)); //  R1 C2

              symbolCommand[3] = params[20] == true ?   (symbolCommand[3] | (1 << 4)) :   (symbolCommand[3] & ~(1 << 4)); //24 R5 C1
              symbolCommand[3] = params[15] == true ?   (symbolCommand[3] | (1 << 3)) :   (symbolCommand[3] & ~(1 << 3)); //24 R4 C1
              symbolCommand[3] = params[10] == true ?   (symbolCommand[3] | (1 << 2)) :   (symbolCommand[3] & ~(1 << 2)); //24 R3 C1
              symbolCommand[3] = params[5] == true ?   (symbolCommand[3] | (1 << 1)) :   (symbolCommand[3] & ~(1 << 1)); //24 R2 C1
              symbolCommand[3] = params[0] == true ?   (symbolCommand[3] | (1 << 0)) :   (symbolCommand[3] & ~(1 << 0)); //   R1 C1


              //console.log(params);
              //console.log("symbolCommand = ");
              //console.log(symbolCommand);

              var dataview = new DataView(symbolCommand.buffer);
              matrixBits =  dataview.getUint32(0);
              matrixBits |= 1 << 27 // set bit 27 to indicate LED Matrix Data in bits 0 - 24

              //console.log("MatrixBits = 0x" + matrixBits.toString(16));


              //if (window.snapMicrobit.led.matrix.lastValue == matrixBits){
                //return;
              //}
  
              var thisCommand = {
                robot: 'A',
                pin: 17,       // LED command is encoded on pin 17's position
                value: matrixBits,
                isDigital: 0,  // LED commands are all zeros
                isOutput: 0,
                isServo: 0,
                isServoPulse: 0,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.led.matrix.lastValue = matrixBits;
              window.snapMicrobit.sendCommand(thisCommand);
              
            