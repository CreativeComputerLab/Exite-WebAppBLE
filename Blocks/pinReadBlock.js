
                  if (window.snapMicrobit.readPinBlock === undefined) {
                    window.snapMicrobit.readPinBlock = function (pin) {
                      let robot = "A";
                      var isDigital, isInput, isServo, isServoPulse, isAnalogPeriod, isDigitalPulse, digitalPulseLevel;
                      isDigital = isInput = isServo = isServoPulse = isAnalogPeriod = isDigitalPulse = digitalPulseLevel= 0;                      
                      // TODO  Check for first read here
                      //  isFirstRead()....
                      if (window.snapMicrobit.isFirstRead(robot, pin, isDigital, isInput, isAnalogPeriod, isDigitalPulse, digitalPulseLevel))
                      {
                          //isDigital=0 and isOutput=1 means this is an analog read pin.
                          var thisCommand = {
                            robot: robot,
                            pin: pin,
                            value: 0, // No value necessary, setting pin state
                            isDigital: 0,
                            isOutput: 1,
                            isServo: 0,
                            isServoPulse: 0,
                            isAnalogPeriod: 0,
                            isDigitalPulse: 0,
                            digitalPulseLevel: 0
                          }
                            window.snapMicrobit.sendCommand(thisCommand);
                            return -1;
                      }
                      else
                        return window.snapMicrobit.notificationData[robot][pin] & 0xFFFF
                    }
                  }

                  return window.snapMicrobit.readPinBlock(pin);
                