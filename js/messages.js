/**
 * This file handles Message Channel communications with the snap iframe.
 */

window.addEventListener('message', onMessage);
var messagePort;

/**
 * onMessage - Handle message received by this window.
 *
 * @param  {Object} e Message event.
 */
function onMessage(e) {
  //console.log("message.js incoming message");
  //console.log(e.data);

  if (e.ports[0] != undefined) {
    //This message sets up the message port so that the app can send updates.
    messagePort = e.ports[0]
    // Use the transfered port to post a message back to the main frame
    messagePort.postMessage('Port received');
  } else if (e.data.message == "SPEAK") {
    console.log("tts " + e.data.val)
    textToSpeech(e.data.val)
  } else {
    // incoming command from a snap block
    let robot = getRobotByLetter(e.data.robot);
    //console.log("incoming command from a snap block");
    //console.log(e.data);
    updateSetAll(robot, e.data.pin, e.data.value, e.data.isDigital, e.data.isInput, e.data.isServo, e.data.isServoPulse, e.data.isAnalogPeriod, e.data.isDigitalPulse, e.data.digitalPulseLevel);

  } 
}

function updateSetAll(robot, pin, value, isDigital, isInput, isServo, isServoPulse, isAnalogPeriod, isDigitalPulse, digitalPulseLevel) {
       
            var pinData = 0;

            if (pin < 17) {  //IO pins
                pinData = value & 0xFFFF;  // The 16 Least Signifigant bytes contain the value

                // Set the bit(s) to determine the type of the pin
                pinData = writeBit(pinData, 16, isDigital);  // The 16th bit contains the A or D mode
                pinData = writeBit(pinData, 17, isInput);  // The 17th bit contains the input/output status
                pinData = writeBit(pinData, 18, isServo);     
                pinData = writeBit(pinData, 19, isServoPulse);      
                pinData = writeBit(pinData, 20, isAnalogPeriod);      
                pinData = writeBit(pinData, 21, isDigitalPulse);  

                // High low parameter for digital pulse edge must be 0 or 1
                if (isDigitalPulse == 1)
                    if (value != 0)
                        value = 1;
                pinData = writeBit(pinData, 22, value);                
                pinData = writeBit(pinData, 23, 1);  // The 23rd bit set to 1 indicates touched i.e. fresh data. 
                                                     // This is needed otherwise pin 0 set to 0 would be taken to be no new data.

                //LOG.debug("updateSetAll:  pinData = {}", Integer.toHexString(pinData));
                //LOG.debug("updateSetAll().isdigitalpulsepin value = {}", isDigitalPulsePin(pinData));
                //LOG.debug("pinData Reverse Endian = {}", Integer.toHexString(reverseIntEndian(pinData)));

            }

            else if (pin == 17) { // LED Display matrix
                 pinData = value & 0xFFFFFF; //3 bytes make up LED array data
                if (value == 1){
                    //LOG.debug("Sending Enable Display Command");
                    pinData = writeBit(pinData, 30, 1); // enable
                    pinData = writeBit(pinData, 29, 0); // overwrite previous disable command if configured                  
                }
                else if (value == 0){
                    //LOG.debug("Sending Disable Display Command");
                    pinData = writeBit(pinData, 29, 1); // disable
                    pinData = writeBit(pinData, 30, 0); // overwrite previous enable command if configured                  
                }
                else {  // Matrix Data
                    pinData = value;
                    pinData = writeBit(pinData, 30, 0); // disable display enable and disable bits
                    pinData = writeBit(pinData, 29, 0); //                  
                    pinData = writeBit(pinData, 28, 1); // set LED display matrix bit                  
                }

                pinData = writeBit(pinData, 31, 1);  // Fresh data bit indicator


                //LOG.debug("Display pinData = {}", Integer.toHexString(pinData));
            }

            var frameNum = getFrameNumberFromPin(pin);
            //console.log("Frame Number = " + frameNum);
            //console.log("Pin Data int = " + pinData);
            //console.log("Pin Data int hex = " + pinData.toString(16));
            
            pinDataArr = intToByteArray(pinData);
            //console.log("Pin Data Bytes:");
            //console.log(pinDataArr);

            

            overlayPinData(robot.setAllData, pinDataArr, pin);

            robot.setAllChanged[frameNum] = true;
            //LOG.debug("set_all Data updated: {}", bytesToString(intToByteArray(setAllData)));
    }

function byteArrayToInt(arr, pin) {
  pin*=4;  // 4 bytes per pin
  var byteBuff = new Uint8Array([arr[pin], arr[pin+1], arr[pin+3]]);
  var uint32 = new Uint32Array(byteBuff);
  // TODO - check whether reverse endian is required
  return uint32[0];
}

function intToByteArray (num) {
    arr = new Uint8Array([
         (num & 0xff000000) >> 24,
         (num & 0x00ff0000) >> 16,
         (num & 0x0000ff00) >> 8,
         (num & 0x000000ff)
    ]);
    return arr;
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

// overlay 4 byte pin data on a 100 byte Uint8 array 
function overlayPinData(arr, subArr, pin) {
  var frameNum = getFrameNumberFromPin(pin);
  //console.log("pin: " + pin + "  Frame Number " + frameNum);
  var start = (frameNum * 20) + (pin % 5) * 4;
  var stop  = start + subArr.length;
  var j = 0;
  var i;
  for (i = start; i < stop; i++)
    arr[i] = subArr[j++];
}


// Writes the given bit with the specified value to myData
function writeBit (myData,  bit,  value) {
    if (value == 1) 
        myData |= 1 << bit;
    else if (value == 0)
        myData &= ~(1 << bit);
    //LOG.debug("writeBit debug: {}", myData);
    return myData;
}

/**
 * parseLegacyMessage - Parse commands for the original finch and hummingbird
 * and send to connected robot.
 *
 * @param  {Object} request command object
 */
function parseLegacyMessage(request) {
  console.log(request)
  if (hidRobot === undefined || hidRobot == null) { return }

  var bytes = new Uint8Array(8); //array of bytes to send to Hummingbird
  var counter = 0;
  for (var prop in request) { //read through request, adding each property to byte array
      if (request.hasOwnProperty(prop)) {
          bytes[counter] = request[prop];
          counter++;
      }
  }
  for (var i = counter; i < bytes.length; ++i) {
      bytes[i] = 0;
  }
  console.log("Sending " + bytes)
  hidRobot.sendBytes(bytes)
}

/**
 * parseMessage - Function for parsing commands for micro:bit based robots.
 *
 * @param  {Object} message Object containing command information
 */
function parseMessage(message) {
  let robot = getRobotByLetter(message.robot);
  if (FinchBlox) { robot = finchBloxRobot }
  if (robot == null) {
    console.error("Unable to find robot " + message.robot);
    return;
  }

  switch(message.cmd) {
    case "triled":
      robot.setTriLED(message.port, message.red, message.green, message.blue);

      break;
    case "playNote":
      robot.setBuzzer(message.note, message.duration)

      break;
    case "symbol":
      robot.setSymbol(message.symbolString)

      break;
    case "print":
      robot.setPrint(message.printString.split(""))

      break;
    case "wheels":
      robot.setMotors(message.speedL, 0, message.speedR, 0);

      break;
    case "turn":
      shouldFlip = (message.angle < 0);
      const shouldTurnRight = (message.direction == "Right" && !shouldFlip) || (message.direction == "Left" && shouldFlip);
      const shouldTurnLeft = (message.direction == "Left" && !shouldFlip) || (message.direction == "Right" && shouldFlip);
      const turnTicks = Math.abs(message.angle * FINCH_TICKS_PER_DEGREE);

      if (turnTicks != 0) { //ticks=0 is the command for continuous motion
        if (shouldTurnRight) {
          robot.setMotors(message.speed, turnTicks, -message.speed, turnTicks);
        } else if (shouldTurnLeft) {
          robot.setMotors(-message.speed, turnTicks, message.speed, turnTicks);
        }
      }
      break;
    case "move":
      shouldFlip = (message.distance < 0);
      const shouldGoForward = (message.direction == "Forward" && !shouldFlip) || (message.direction == "Backward" && shouldFlip);
      const shouldGoBackward = (message.direction == "Backward" && !shouldFlip) || (message.direction == "Forward" && shouldFlip);
      const moveTicks = Math.abs(message.distance * FINCH_TICKS_PER_CM);

      if (moveTicks != 0) { //ticks=0 is the command for continuous motion
        if (shouldGoForward) {
          robot.setMotors(message.speed, moveTicks, message.speed, moveTicks);
        } else if (shouldGoBackward) {
          robot.setMotors(-message.speed, moveTicks, -message.speed, moveTicks);
        }
      }
      break;
    case "motors": //FinchBlox specific
      robot.setMotors(message.speedL * 100/36, message.ticksL, message.speedR * 100/36, message.ticksR);
      break;
    case "stopFinch":
      robot.setMotors(0, 0, 0, 0);

      break;
    case "resetEncoders":
      robot.resetEncoders();

      break;
    case "stopAll":
      robot.stopAll();

      break;
    case "servo":
      robot.setServo(message.port, message.value);

      break;
    case "led":
      robot.setLED(message.port, message.intensity);

      break;
    case "glowboard":
      robot.setGlowBoard(message.color, message.brightness, message.symbolString)

      break;
    case "setPoint":
      robot.setGBPoint(message.xPos, message.yPos, message.color, message.brightness)

      break;
    default:
      console.error("Command not implemented: " + message.cmd);
  }
}

/**
 * sendMessage - Function for sending data back to snap. Does nothing if message
 * channel has not been set up yet.
 *
 * @param  {Object} message Information to send
 */
function sendMessage(message) {
  if (messagePort == undefined) {
    //console.log("Message channel not set up. Trying to send: ");
    //console.log(message);
    return;
  }
  //console.log("Sending: ");
  //console.log(message);
  messagePort.postMessage(message);
}

function textToSpeech(text) {
  var msg = new SpeechSynthesisUtterance(text);
  window.speechSynthesis.speak(msg);
}
