              // Do not send redundant command if the position has not changed.
              if (window.snapMicrobit.servo.position.lastValue == position){
                //console.log("position Server value steady, doing nothing");
                return;
              }
  
              var thisCommand = {
                robot: 'A',
                pin: pin,
                value: position,
                isDigital: 0,
                isOutput: 0,
                isServo: 1,
                isServoPulse: 0,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.sendCommand(thisCommand);
              window.snapMicrobit.servo.position.lastValue = position;
            