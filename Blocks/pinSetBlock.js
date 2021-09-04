              var thisCommand = {
                robot: 'A',
                pin: pin_num,
                value: level,
                isDigital: 0,
                isInput: 0,
                isServo: 0,
                isServoPulse: 0,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.sendCommand(thisCommand);