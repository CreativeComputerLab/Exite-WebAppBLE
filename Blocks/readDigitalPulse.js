
                  if (window.snapMicrobit.readDigitalPulsePinBlock === undefined) {
                    window.snapMicrobit.readDigitalPulsePinBlock = function (pin, level) {
                      let robot = "A";
                      var isDigital, isInput, isServo, isServoPulse, isAnalogPeriod, isDigitalPulse, digitalPulseLevel;
                      isDigital = isInput = isServo = isServoPulse = isAnalogPeriod = isDigitalPulse = digitalPulseLevel= 0; 

                      // Set Pin Config Here:
                      // Analog Pin, is an output:
                      isDigital = 1; // 0 is for analog mode
                      isInput = 1;  // 1 is for read
                      isDigitalPulse = 1; // Read the digital pulse  in microseconds
                      digitalPulseLevel = level;  // This is 0 or 1




                      // Check for first read here
                      if (window.snapMicrobit.isFirstRead(robot, pin, isDigital, isInput, isAnalogPeriod, isDigitalPulse, digitalPulseLevel))
                      {
                          //isDigital=0 and isOutput=1 means this is an analog read pin.
                          var thisCommand = {
                            robot: robot,
                            pin: pin,
                            value: digitalPulseLevel, // Value necessary to know which pulse to count i.e. high or low.
                            isDigital: isDigital,
                            isInput: isInput,
                            isServo: 0,
                            isServoPulse: 0,
                            isAnalogPeriod: 0,
                            isDigitalPulse: isDigitalPulse,
                            digitalPulseLevel: digitalPulseLevel
                          }
                            window.snapMicrobit.sendCommand(thisCommand);
                            return -1;
                      }
                      else {
                        var pinVal = window.snapMicrobit.pinToInt(robot, pin);
                        return pinVal & 0xFFFF
                      }
                    }
                  }

                  // TODO  this loop below doesn't work. Need to find another way to wait for notification after first read

                  var numCalls = 5;
                  var callNum = 0;
                  var value = -1;

                  value = window.snapMicrobit.readDigitalPulsePinBlock(pin, level);

                  //while ((value == -1) && (callNum++ < numCalls)) {
                  //   value = window.snapMicrobit.readPinBlock(pin);
                  //}

                  return value;
                