              // Do not send redundant command if the state has not changed.
              if (window.snapMicrobit.led.enable.lastValue == state){
                return;
              }
  
              var thisCommand = {
                robot: 'A',
                pin: 17,       // LED command is encoded on pin 17's position
                value: state,
                isDigital: 0,  // LED commands are all zeros
                isOutput: 0,
                isServo: 0,
                isServoPulse: 0,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.led.enable.lastValue = state;
              window.snapMicrobit.sendCommand(thisCommand);
              
            