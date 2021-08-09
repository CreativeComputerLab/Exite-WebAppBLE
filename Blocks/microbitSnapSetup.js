
                  function setupSnapMicrobit() {
                    window.snapMicrobit = {};
                    window.snapMicrobit.notificationData.A = Uint8Array(100).fill(0);
                    window.snapMicrobit.microbitIsV2.A = false;
                    window.snapMicrobit.robotType.A = 3;

                    //console.log("setting up message channel")
                    window.snapMicrobit.messageChannel = new MessageChannel();
                    window.snapMicrobit.messageChannel.port1.onmessage = function (e) {
                        console.log("Got a message: ");
                        console.log(e.data);
                        if (e.data.notificationData != null && e.data.robot != null) {
                          let robot = e.data.robot;
                          overlaySubArray(window.snapMicrobit.notificationData[robot], e.data.newNotificationData, e.data.frameNumber);
                          window.snapMicrobit.robotType[robot] = e.data.robotType;
                          window.snapMicrobit.microbitIsV2[robot] = e.data.hasV2Microbit;
                        }
                    }
                    window.parent.postMessage("hello from snap", "*", [window.snapMicrobit.messageChannel.port2]);

                    window.snapMicrobit.sendCommand = function(command) {
                      window.parent.postMessage(command, "*");
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


                    // overlay 20 byte frames on a 100 byte Uint8 array 
                    function overlaySubArray(arr, subArr, frameNum) {
                      var start = frameNum * 20;
                      var stop  = start + 20;
                      var j = 0;
                      var i;
                      for (i = start; i < stop; i++)
                        arr[i] = subArr[j++];
                    }

                    window.snapMicrobit.getMicrobitAcceleration = function(axis, robot) {
                      let accVal = 0;
                      return (accVal);
                    }

                    window.snapMicrobit.getDistance = function (robot) {
                      let distVal = 0;
                      return (distVal);
                    }

                  let currentVersion = 1
                  if (window.snapMicrobit === undefined ||
                    window.snapMicrobit.version === undefined ||
                    window.snapMicrobit.version < currentVersion) {

                    setupSnapMicrobit()
                    window.snapMicrobit.version = currentVersion
                  } else {
                    //console.log("BirdBrain already set up");
                  }
                