
                  function setupSnapMicrobit() {
                    window.snapMicrobit = {};
                    window.snapMicrobit.notificationData = {};
                    window.snapMicrobit.notificationData.A = new Uint8Array(100).fill(0);
                    window.snapMicrobit.microbitIsV2 = {};
                    window.snapMicrobit.microbitIsV2.A = false;
                    window.snapMicrobit.robotType = {};
                    window.snapMicrobit.robotType.A = 3;

                    //console.log("setting up message channel")
                    window.snapMicrobit.messageChannel = new MessageChannel();
                    window.snapMicrobit.messageChannel.port1.onmessage = function (e) {
                        //console.log("Snap: Got a message: ");
                        //console.log(e);
                        //console.log(e.data);
                        if (e.data.newNotificationData != null && e.data.robot != null) {
                          let robot = e.data.robot;
                          overlaySubArray(window.snapMicrobit.notificationData[robot], e.data.newNotificationData, e.data.frameNumber);
                          //console.log("New Notification Data for pin " + e.data.pin);
                          //console.log(window.snapMicrobit.notificationData[robot]);
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

                    // 5 pins per frame,  5 frames,  25 "pins"
                    function getFrameNumberFromPin(pin) {
                      if (pin < 5)
                        return 0;
                      if (pin < 10)
                        return 1;
                      if (pin < 15)
                        return 2;
                      if (pin < 20)
                        return 3;
                      if (pin < 25)
                        return 4;  
                    }


                    // overlay 20 byte frames on a 100 byte Uint8 array 
                    function overlaySubArray(arr, subArr, frameNum) {
                      //console.log("Frame Number " + frameNum);
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

                    function isDigitalPin ( pinData) {
                        // THis is little endian
                        //console.log("isDigitalPin: input =  " + pinData.toString(16));
                        //console.log((pinData & 0x10000).toString(16));                        
                        return (pinData & 0x10000) == 0x10000; // bit 16 0x10000 big endian
                    }

                    function isInputPin ( pinData) {
                        // THis is little endian
                        //console.log("isInputPin: input =  " + pinData.toString(16));
                        //console.log((pinData & 0x20000).toString(16));
                        return (pinData & 0x20000) == 0x20000; // bit 17  0x20000
                    }

                    function isAnalogPeriodPin ( pinData) {
                        // THis is little endian
                        return (pinData & 0x100000) == 0x100000; // bit 20  0x100000
                    }

                    function isDigitalPulsePin ( pinData) {
                        // THis is little endian
                        return (pinData & 0x200000) == 0x200000; // bit 21  0x200000
                    }

                    function isDigitalPulseHigh ( pinData) {
                        // THis is little endian
                        return (pinData & 0x400000) == 0x400000; // bit 22  0x200000
                    }

                    // Input is an int.
                    // Returns a reversed endian int.
                    function reverseIntEndian(i) {
                        return (i&0xff)<<24 | (i&0xff00)<<8 | (i&0xff0000)>>8 | (i>>24)&0xff;
                    }

                    window.snapMicrobit.pinToInt = function(robot, pin) {
                      pin*=4;  // 4 bytes per pin
                      //console.log(window.snapMicrobit.notificationData[robot]);
                      //console.log(window.snapMicrobit.notificationData[robot][pin] +"  " + window.snapMicrobit.notificationData[robot][pin+1]+"  " + window.snapMicrobit.notificationData[robot][pin+2] +"  " + window.snapMicrobit.notificationData[robot][pin+3]);
                      var byteBuff = new Uint8Array([window.snapMicrobit.notificationData[robot][pin], window.snapMicrobit.notificationData[robot][pin+1], window.snapMicrobit.notificationData[robot][pin+2], window.snapMicrobit.notificationData[robot][pin+3]]);
                      var dataview = new DataView(byteBuff.buffer);
                      return dataview.getUint32(0);
                    }

                    window.snapMicrobit.pinToInt32 = function(robot, pin) {
                      pin*=4;  // 4 bytes per pin
                      //console.log(window.snapMicrobit.notificationData[robot]);
                      //console.log(window.snapMicrobit.notificationData[robot][pin] +"  " + window.snapMicrobit.notificationData[robot][pin+1]+"  " + window.snapMicrobit.notificationData[robot][pin+2] +"  " + window.snapMicrobit.notificationData[robot][pin+3]);
                      var byteBuff = new Uint8Array([window.snapMicrobit.notificationData[robot][pin], window.snapMicrobit.notificationData[robot][pin+1], window.snapMicrobit.notificationData[robot][pin+2], window.snapMicrobit.notificationData[robot][pin+3]]);
                      var dataview = new DataView(byteBuff.buffer);
                      return dataview.getInt32(0);
                    }


                    window.snapMicrobit.isFirstRead  = function (robot, pin, isDigital, isInput, isAnalogPeriod, isDigitalPulse, digitalPulseLevel) {
                      var firstRead = false;
                      //console.log("isFirstRead() input pin = 0x" + pin.toString(16));
                      var pinVal = window.snapMicrobit.pinToInt(robot, pin);
                      //console.log("Value of pin: " + pin + ": 0x" + pinVal.toString(16));
                      //console.log("IsInputPin is " + isInputPin(pinVal));
                      //console.log("IsDigitalPin is " + isDigitalPin(pinVal));
                      //console.log("isDigital is " + isDigital);
                      //console.log("isInput is " + isInput);
                      //console.log((isDigitalPin(pinVal) && (isDigital == 0)));
                      //console.log((!isDigitalPin(pinVal)) && (isDigital == 1));
                      firstRead = (               (isDigitalPin(pinVal) && (isDigital == 0)) || ((!isDigitalPin(pinVal)) && (isDigital == 1)) 
                                               || (isInputPin(pinVal) && (isInput == 0)) || (!isInputPin(pinVal) && (isInput == 1))
                                               || (isAnalogPeriodPin(pinVal) && (isAnalogPeriod == 0)) || (!isAnalogPeriodPin(pinVal) && (isAnalogPeriod == 1)) 
                                               || (isDigitalPulsePin(pinVal) && (isDigitalPulse == 0)) || (!isDigitalPulsePin(pinVal) && (isDigitalPulse == 1)) 
                                               || (isDigitalPulseHigh(pinVal) && (digitalPulseLevel == 0)) || (!isDigitalPulseHigh(pinVal) && (digitalPulseLevel == 1))            
                                  ); 
                      console.log("FirstRead returns " + firstRead);                      
                      return firstRead;
                    }
                  }  // end of setupSnapMicrobit


//window.snapMicrobit = undefined;

                  let currentVersion = 1
                  //console.log("BirdBrain ready to set up");
                  if (window.snapMicrobit === undefined ||
                    window.snapMicrobit.version === undefined ||
                    window.snapMicrobit.version < currentVersion) {

                    setupSnapMicrobit()
                    window.snapMicrobit.version = currentVersion
                  } else {
                    //console.log("BirdBrain already set up");
                  }
                