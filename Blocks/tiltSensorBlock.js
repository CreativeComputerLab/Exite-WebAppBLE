                    var robot = "A";
                    value = window.snapMicrobit.pinToInt(robot, 12); // pin 12 is tilt sensor
                    switch (dim) { //XYZ number to byte mapping
                        case "Tilt Left":   //X axis
                            if (value == 3)   //MICROBIT_ACCELEROMETER_EVT_TILT_LEFT
                                return true;
                            else 
                                return false;
                        break;
                        case "Tilt Right":                        
                            if (value == 4)   //MICROBIT_ACCELEROMETER_EVT_TILT_RIGHT
                                return true;
                            else 
                                return false;
                        break;
                        case "Logo Up":     // Y axis
                            if (value == 1)   
                                return true;
                            else 
                                return false;
                        break;
                        case "Logo Down": //MICROBIT_ACCELEROMETER_EVT_TILT_DOWN
                            if (value == 2)   
                                return true;
                            else 
                                return false;
                        break;                            
                        case "Screen Up":     // Z axis //MICROBIT_ACCELEROMETER_EVT_FACE_UP
                            if (value == 5)   
                                return true;
                            else 
                                return false;
                        break;
                        case "Screen Down":   //MICROBIT_ACCELEROMETER_EVT_FACE_DOWN
                            if (value == 6)   
                                return true;
                            else 
                                return false;    
                        break;
                        case "Shake" : //MICROBIT_ACCELEROMETER_EVT_SHAKE
                            if (value == 11)   
                                return true;
                            else 
                                return false;
                        break;
                        default:
                            return false;
                        break;
                    }