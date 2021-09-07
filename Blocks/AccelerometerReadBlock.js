var pin;
var robot = "A";
if (dim == 'X')
	pin = 17;
if (dim == 'Y')
	pin = 18;
if (dim == 'Z')
	pin = 19;
return (window.snapMicrobit.pinToInt(robot, pin) / 100) * .98;