var pin;
var robot = "A";
if (dim == 'X')
	pin = 17;
if (dim == 'Y')
	pin = 18;
if (dim == 'Z')
	pin = 19;
return Math.round((window.snapMicrobit.pinToInt32(robot, pin) / 100)) * 0.98;