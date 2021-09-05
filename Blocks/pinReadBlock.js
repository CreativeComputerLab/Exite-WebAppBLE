
                  if (window.snapMicrobit.readPinBlock === undefined) {
                    window.snapMicrobit.readPinBlock = function (pin) {
                      let robot = "A";
                      var isDigital, isInput, isServo, isServoPulse, isAnalogPeriod, isDigitalPulse, digitalPulseLevel;
                      isDigital = isInput = isServo = isServoPulse = isAnalogPeriod = isDigitalPulse = digitalPulseLevel= 0; 

                      // Set Pin Config Here:
                      // Analog Pin, is an output:
                      isInput = 1;  // a pin read is an input pin  i.e input voltage to the micro:bit
                      isDigital = 0;

                      // Check for first read here
                      if (window.snapMicrobit.isFirstRead(robot, pin, isDigital, isInput, isAnalogPeriod, isDigitalPulse, digitalPulseLevel))
                      {
                          //isDigital=0 and isOutput=1 means this is an analog read pin.
                          var thisCommand = {
                            robot: robot,
                            pin: pin,
                            value: 0, // No value necessary, setting pin state
                            isDigital: 0,
                            isInput: isInput,
                            isServo: 0,
                            isServoPulse: 0,
                            isAnalogPeriod: 0,
                            isDigitalPulse: 0,
                            digitalPulseLevel: 0
                          }
                            window.snapMicrobit.sendCommand(thisCommand);
                            return -1;
                      }
                      else {
                        var pinVal = window.snapMicrobit.pinToInt(robot, pin);
                        //return window.snapMicrobit.notificationData[robot][pin] & 0xFFFF
                        return pinVal & 0xFFFF
                      }
                    }
                  }

                  // TODO  this loop below doesn't work. Need to find another way to wait for notification after first read

                  var numCalls = 5;
                  var callNum = 0;
                  var value = -1;

                  value = window.snapMicrobit.readPinBlock(pin);

                  while ((value == -1) && (callNum++ < numCalls)) {
                     value = window.snapMicrobit.readPinBlock(pin);
                  }

                  return value;
                