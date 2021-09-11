            //if (window.snapMicrobit.print.lastValue == string){
                //return;
            //}
			console.log(string);

			var charArray = Array.from(string);
			console.log(charArray);

			if (charArray.length > 15)
				charArray.length = 15;

			// prepend the length to the index 0 of the array
			//charArray.unshift(charArray.length);
			//console.log(charArray);

			var printString = new Uint8Array(16);

			printString[0] = charArray.length;
			for (var i = 1; i < charArray.length+1; i++) {
				printString[i] = charArray[i-1].charCodeAt();
			}
			console.log(printString);
  			
              var thisCommand = {
                robot: 'A',
                pin: 20,       // print command is encoded on pins 20-23 position
                value: printString,
                isDigital: 0,  // print commands are all zeros
                isOutput: 0,
                isServo: 0,
                isServoPulse: 0,
                isAnalogPeriod: 0,
                isDigitalPulse: 0,
                digitalPulseLevel: 0
              }

              window.snapMicrobit.print.lastValue = string;
              window.snapMicrobit.sendCommand(thisCommand);




