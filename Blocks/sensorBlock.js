
                  if (window.birdbrain.readPinBlock === undefined) {
                    window.birdbrain.getHBSensor = function (pin) {
                      const distanceScaling = 117/100;
                      const dialScaling = 100/230;
                      const lightScaling = 100/255;
                      const soundScaling = 200/255;
                      const voltageScaling = 3.3/255;

                      let value = window.birdbrain.sensorData[robot][port - 1];

                      switch(sensor) {
                        case "Distance (cm)":
                          return Math.round(value * distanceScaling);
                        case "Dial":
                          if (value > 230) { value = 230; }
                          return Math.round(value * dialScaling);
                        case "Light":
                          return Math.round(value * lightScaling);
                        case "Sound":
                          return Math.round(value * soundScaling);
                        case "Other (V)":
                          return Math.round(value * voltageScaling * 100) / 100;
                      }
                    }
                  }

                  return window.birdbrain.readPinBlock(pin);
                