              // Do not send redundant command if the position has not changed.
              if (window.snapMicrobit.servo.rotation.lastValue == speed){
                //console.log("Rotation Server value steady, doing nothing");
                return;
              }
  
              var thisCommand = {
                robot: 'A',
                pin: pin,
                value: speed,
                isDigital: 0,
                isOutput: 0,
                isServo: 0,
                isServoPulse: 1,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.sendCommand(thisCommand);
              window.snapMicrobit.servo.rotation.lastValue = speed;
            