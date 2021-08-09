
                  function setupBirdbrain() {
                    window.birdbrain = {};
                    window.birdbrain.sensorData = {};
                    window.birdbrain.sensorData.A = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
                    window.birdbrain.sensorData.B = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
                    window.birdbrain.sensorData.C = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];
                    window.birdbrain.microbitIsV2 = {};
                    window.birdbrain.microbitIsV2.A = false;
                    window.birdbrain.microbitIsV2.B = false;
                    window.birdbrain.microbitIsV2.C = false;
                    window.birdbrain.robotType = {
                      FINCH: 1,
                      HUMMINGBIRDBIT: 2,
                      MICROBIT: 3,
                      //connected robots default to type MICROBIT
                      A: 3,
                      B: 3,
                      C: 3
                    };

                    //console.log("setting up message channel")
                    window.birdbrain.messageChannel = new MessageChannel();
                    window.birdbrain.messageChannel.port1.onmessage = function (e) {
                        //console.log("Got a message: ");
                        //console.log(e.data);
                        if (e.data.sensorData != null && e.data.robot != null) {
                          let robot = e.data.robot;
                          window.birdbrain.sensorData[robot] = e.data.sensorData;
                          window.birdbrain.robotType[robot] = e.data.robotType;
                          window.birdbrain.microbitIsV2[robot] = e.data.hasV2Microbit;
                        }
                    }
                    window.parent.postMessage("hello from snap", "*", [window.birdbrain.messageChannel.port2]);

                    window.birdbrain.sendCommand = function(command) {
                      window.parent.postMessage(command, "*");
                    }

                    window.birdbrain.finchIsMoving = function(finch) {
                      return (window.birdbrain.sensorData[finch][4] > 127);
                    }

                    //  Converts byte range 0 - 255 to -127 - 127 represented as a 32 bit signe int
                    function byteToSignedInt8 (byte) {
                      var sign = (byte & (1 << 7));
                      var value = byte & 0x7F;
                      if (sign) { value  = byte | 0xFFFFFF00; }
                      return value;
                    }

                    //  Converts byte range 0 - 255 to -127 - 127 represented as a 32 bit signe int
                    function byteToSignedInt16 (msb, lsb) {
                      var sign = msb & (1 << 7);
                      var value = (((msb & 0xFF) << 8) | (lsb & 0xFF));
                      if (sign) {
                        value = 0xFFFF0000 | value;  // fill in most significant bits with 1's
                      }
                      return value;
                    }

                    window.birdbrain.getMicrobitAcceleration = function(axis, robot) {
                      const rawToMperS = 196/1280; //convert to meters per second squared
                      let sensorData = window.birdbrain.sensorData[robot];
                      let accVal = 0;
                      switch (axis) {
                        case 'X':
                          accVal = byteToSignedInt8(sensorData[4]);
                          break;
                        case 'Y':
                          accVal = byteToSignedInt8(sensorData[5]);
                          break;
                        case 'Z':
                          accVal = byteToSignedInt8(sensorData[6]);
                          break;
                      }
                      return (accVal * rawToMperS);
                    }

                    window.birdbrain.getMicrobitMagnetometer = function(axis, finch) {
                      const rawToUT = 1/10; //convert to uT
                      let sensorData = window.birdbrain.sensorData[finch];
                      let msb = 0;
                      let lsb = 0;
                      switch (axis) {
                        case 'X':
                          msb = sensorData[8];
                          lsb = sensorData[9];
                          break;
                        case 'Y':
                          msb = sensorData[10];
                          lsb = sensorData[11];
                          break;
                        case 'Z':
                          msb = sensorData[12];
                          lsb = sensorData[13];
                          break;
                      }
                      let magVal = byteToSignedInt16(msb, lsb);
                      return Math.round(magVal * rawToUT);
                    }

                    window.birdbrain.getFinchAcceleration = function(axis, finch) {
                      let sensorData = window.birdbrain.sensorData[finch];
                      let accVal = 0;
                      switch (axis) {
                        case 'X':
                          accVal = byteToSignedInt8(sensorData[13]);
                          break;
                        case 'Y':
                        case 'Z':
                          const rawY = byteToSignedInt8(sensorData[14]);
                          const rawZ = byteToSignedInt8(sensorData[15]);
                          const rad = 40 * Math.PI / 180; //40° in radians

                          switch(axis) {
                            case 'Y':
                              accVal = (rawY*Math.cos(rad) - rawZ*Math.sin(rad));
                              break;
                            case 'Z':
                              accVal = (rawY*Math.sin(rad) + rawZ*Math.cos(rad));
                              break;
                          }
                        }
                        return (accVal * 196/1280);
                    }

                    window.birdbrain.getFinchMagnetometer = function(axis, finch) {
                      let sensorData = window.birdbrain.sensorData[finch];
                      switch (axis) {
                        case 'X':
                          return byteToSignedInt8(sensorData[17]);
                        case 'Y':
                        case 'Z':
                          const rawY = byteToSignedInt8(sensorData[18]);
                          const rawZ = byteToSignedInt8(sensorData[19]);
                          const rad = 40 * Math.PI / 180 //40° in radians

                          let magVal = 0;
                          switch(axis) {
                            case 'Y':
                              magVal = (rawY*Math.cos(rad) + rawZ*Math.sin(rad));
                              break;
                            case 'Z':
                              magVal = (rawZ*Math.cos(rad) - rawY*Math.sin(rad));
                              break;
                          }
                          return Math.round(magVal);
                      }
                    }

                    window.birdbrain.getFinchDistance = function (robot) {
                      if (window.birdbrain.microbitIsV2[robot]) {
                        return window.birdbrain.sensorData[robot][1];
                      } else {
                        const cmPerDistance = 0.0919;
                        const msb = window.birdbrain.sensorData[robot][0];
                        const lsb = window.birdbrain.sensorData[robot][1];

                        const distance = msb << 8 | lsb;
                        return Math.round(distance * cmPerDistance);
                      }
                    }
                  }

                  let currentVersion = 1
                  if (window.birdbrain === undefined ||
                    window.birdbrain.version === undefined ||
                    window.birdbrain.version < currentVersion) {

                    setupBirdbrain()
                    window.birdbrain.version = currentVersion
                  } else {
                    //console.log("BirdBrain already set up");
                  }
                