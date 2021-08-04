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
  //console.log(e.data);

  if (e.ports[0] != undefined) {
    //This message sets up the message port so that the app can send updates.
    messagePort = e.ports[0]
    // Use the transfered port to post a message back to the main frame
    messagePort.postMessage('Port received');
  } else if (e.data.message == "SPEAK") {
    console.log("tts " + e.data.val)
    textToSpeech(e.data.val)

  } else if (useHID) {
    //This is a legacy robot command
    parseLegacyMessage(e.data)
  } else {
    //This message is a micro:bit robot command
    parseMessage(e.data);
  }
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
