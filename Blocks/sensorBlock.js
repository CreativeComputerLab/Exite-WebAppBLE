
                  if (window.snapMicrobit.readPinBlock === undefined) {
                    window.snapMicrobit.readPinBlock = function (pin) {
                      let robot = "A";
                      // TODO  Check for first read here
                      //  isFirstRead()....
                      return window.snapMicrobit.notificationData[robot][pin] & 0xFFFF
                    }
                  }

                  return window.snapMicrobit.readPinBlock(pin);