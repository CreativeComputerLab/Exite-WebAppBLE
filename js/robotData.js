
/**
 * RobotData - Data set ready to send to a robot when changed.
 *
 * @param  {[number]} initialValues values to initialize the data set with
 */
function RobotData(initialValues) {
  this.values = new Uint8Array(initialValues);
  this.oldValues = this.values.slice();
  this.originalValues = this.values.slice();
  this.isNew = true;
  this.length = initialValues.length;
  this.pendingChanges = [];
}

/**
 * RobotData - Compare two value arrays
 *
 * @param  {Uint8Array} data1 first array to compare
 * @param  {Uint8Array} data2 second array to compare
 * @return {boolean}          true if data is same in both arrays
 */
RobotData.isEqual = function(data1, data2) {

  if (data1.length != data2.length) { return false; }

  var equal = true;
  for (var i = 0; i < data1.length; i++) {
    if (data1[i] != data2[i]) { equal = false; }
  }

  //console.log("returning result " + equal);
  return equal;
}


/**
 * RobotData.prototype.segmentHasChanged - Determines whether a specified
 * portion of the data has changed.
 *
 * @param  {number} startIndex position to start the check
 * @param  {number} length     number of values to check
 * @return {boolean}           true if the specified portion of the data has changed
 */
RobotData.prototype.segmentHasChanged = function(startIndex, length) {
  var hasChanged = false;
  for (var i = 0; i < length; i++) {
    if (this.values[startIndex + i] != this.oldValues[startIndex + i]) {
      hasChanged = true;
    }
  }
  return hasChanged;
}

/**
 * RobotData.prototype.update - Update the data with new values
 *
 * @param  {type} startIndex position in the value array to start the update
 * @param  {type} valueArray new values
 */
RobotData.prototype.update = function(startIndex, valueArray) {
  //console.log("updateData starting at " + startIndex + " to " + valueArray);

  //values must be between 0 and 255
  for(let i = 0; i < valueArray.length; i++) {
    if (valueArray[i] < 0 || valueArray[i] > 255) {
      console.error("updateData invalid value: " + value);
      return;
    }
  }

  //make sure the index is not out of bounds
  if (startIndex < 0 || (startIndex + valueArray.length) > this.length) {
    console.error("updateData invalid index: " + startIndex);
    return;
  }

  this.pendingChanges.push({startIndex: startIndex, valueArray: valueArray});
  this.updatePending();
}

/**
 * RobotData.prototype.updatePending - Performs the next update in the queue if
 * possible. Called after each change is received to see if the update can be
 * made immediately, and called after each write to see if there are more
 * changes pending. Also called if an update is successfully made to see if the
 * next change can be made as well.
 */
RobotData.prototype.updatePending = function() {
  if (this.pendingChanges.length == 0) {
    //console.log("no pending changes found.");
    return;
  } else if (this.pendingChanges.length > 10) {
    for (let i = 0; i < 2; i++) {
      this.pendingChanges.shift();
    }
  }

  //if the existing data hasn't been sent to the robot yet...
  if (this.isNew) {
    //check if the segment in question needs to be sent
    const next = this.pendingChanges[0];
    const hasChanged = this.segmentHasChanged(next.startIndex, next.valueArray.length);
    //if a change to this section is waiting to be sent, we cannot make this next change now.
    if (hasChanged) {
      console.log("cannot update pending changes at this time.")
      return;
    }
  }

  //console.log("about to make an update. current changes = " + this.pendingChanges.length);
  const nextChange = this.pendingChanges.shift();
  let newData = this.values.slice();
  for(let i = 0; i < nextChange.valueArray.length; i++) {
    newData[nextChange.startIndex + i] = nextChange.valueArray[i];
  }

  if (!RobotData.isEqual(this.values, newData)) {
    //console.log("processing pending change {" + nextChange.startIndex + ", [" + nextChange.valueArray + "]}");
    //console.log(newData);
    this.length = newData.length;
    this.values = newData;
    this.isNew = true;
  } else {
    //console.log("change did not include new data {" + nextChange.startIndex + ", [" + nextChange.valueArray + "]}");
    //console.log(newData);
  }

  this.updatePending(); //See if there is another change we can add.
}

/**
 * RobotData.prototype.reset - Returns all or a portion of the data to its
 * original values.
 *
 * @param  {number} startIndex Start the reset at this index (or 0 if null)
 * @param  {number} length     Length of the section to reset (or reset everything if null)
 */
RobotData.prototype.reset = function(startIndex, length) {
  if (startIndex == null || length == null) {
    this.values = this.originalValues.slice();
    this.oldValues = this.originalValues.slice();
  } else {
    for (let i = 0; i < length; i++) {
      this.values[startIndex + i] = this.originalValues[startIndex + i];
      this.oldValues[startIndex + i] = this.originalValues[startIndex + i];
    }
  }
}

/**
 * RobotData.prototype.getSendable - Returns a Uint8Array of values to send to
 * the robot. Saves a copy of these values for comparison, or resets the data
 * if necessary. The data is now no longer new, and the next update is
 * performed if there is one.
 *
 * @param  {boolean} reset True if the data should be reset after sending. Used for motor data and print arrays so that the same command can be sent again (but is not sent accidentially)
 * @return {Uint8Array}    Data array ready to be sent to the robot
 */
RobotData.prototype.getSendable = function(reset) {
  const sendable = this.values.slice();
  if (reset) {
    this.reset();
  } else {
    this.oldValues = sendable.slice();
  }
  this.isNew = false;
  this.updatePending();
  return sendable;
}
