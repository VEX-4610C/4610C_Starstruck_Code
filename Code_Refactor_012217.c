#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    gyro,           sensorGyro)
#pragma config(Sensor, in2,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_3,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           frontRight,    tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           clawA,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           catapultRightB, tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_3)
#pragma config(Motor,  port5,           backRight,     tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port6,           catapultLeftA, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           backLeft,      tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port9,           catapultLeftB, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          frontLeft,     tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"
#define SIGN(n) (n<0 ? -1 : 1)

// LCD Display Code
// Some utility strings
#define LEFT_ARROW  247
#define RIGHT_ARROW 246
static  char l_arr_str[4] = { LEFT_ARROW,  LEFT_ARROW,  LEFT_ARROW,  0};
static  char r_arr_str[4] = { RIGHT_ARROW, RIGHT_ARROW, RIGHT_ARROW, 0};

/*-----------------------------------------------------------------------------*/
/*  This function is used to get the LCD hutton status but also acts as a      */
/*  "wait for button release" feature.                                         */
/*  Use it in place of nLcdButtons.                                            */
/*  The function blocks until a button is pressed.                             */
/*-----------------------------------------------------------------------------*/

// Little macro to keep code cleaner, masks both disable/ebable and auton/driver
#define vexCompetitionState (nVexRCReceiveState & (vrDisabled | vrAutonomousMode))

TControllerButtons
getLcdButtons()
{
	TVexReceiverState   competitionState = vexCompetitionState;
	TControllerButtons  buttons;

	// This function will block until either
	// 1. A button is pressd on the LCD
	//    If a button is pressed when the function starts then that button
	//    must be released before a new button is detected.
	// 2. Robot competition state changes

	// Wait for all buttons to be released
	while( nLCDButtons != kButtonNone ) {
		// check competition state, bail if it changes
		if( vexCompetitionState != competitionState )
			return( kButtonNone );
		wait1Msec(10);
	}

	// block until an LCD button is pressed
	do  {
		// we use a copy of the lcd buttons to avoid their state changing
		// between the test and returning the status
		buttons = nLCDButtons;

		// check competition state, bail if it changes
		if( vexCompetitionState != competitionState )
			return( kButtonNone );

		wait1Msec(10);
	} while( buttons == kButtonNone );

	return( buttons );
}


static int MyAutonomous = -1;

/*-----------------------------------------------------------------------------*/
/*  Display autonomous selection                                               */
/*-----------------------------------------------------------------------------*/

// max number of auton choices
#define MAX_CHOICE  5

void
LcdAutonomousSet( int value, bool select = false )
{
	// Cleat the lcd
	clearLCDLine(0);
	clearLCDLine(1);

	// Display the selection arrows
	displayLCDString(1,  0, l_arr_str);
	displayLCDString(1, 13, r_arr_str);

	// Save autonomous mode for later if selected
	if(select)
		MyAutonomous = value;

	// If this choice is selected then display ACTIVE
	if( MyAutonomous == value )
		displayLCDString(1, 5, "ACTIVE");
	else
		displayLCDString(1, 5, "select");

	// Show the autonomous names
	switch(value) {
	case    0:
		displayLCDString(0, 0, "Cube Right");
		break;
	case    1:
		displayLCDString(0, 0, "Cube Left");
		break;
	case    2:
		displayLCDString(0, 0, "Star Left");
		break;
	case    3:
		displayLCDString(0, 0, "Star Right");
		break;
	case    4:
		displayLCDString(0, 0, "Drive Backwards");
		break;
	case    5:
		displayLCDString(0, 0, "Programming Skills");
		break;
	default:
		displayLCDString(0, 0, "SCREAM AT ALEX");
		break;
	}
}

void
LcdAutonomousSelection()
{
	TControllerButtons  button;
	int  choice = 0;

	// Turn on backlight
	bLCDBacklight = true;

	// diaplay default choice
	LcdAutonomousSet(0);

	while( bIfiRobotDisabled )
	{
		// this function blocks until button is pressed
		button = getLcdButtons();

		// Display and select the autonomous routine
		if( ( button == kButtonLeft ) || ( button == kButtonRight ) ) {
			// previous choice
			if( button == kButtonLeft )
				if( --choice < 0 ) choice = MAX_CHOICE;
			// next choice
			if( button == kButtonRight )
				if( ++choice > MAX_CHOICE ) choice = 0;
			LcdAutonomousSet(choice);
		}

		// Select this choice
		if( button == kButtonCenter )
			LcdAutonomousSet(choice, true );

		// Don't hog the cpu !
		wait1Msec(10);
	}
}

void clawopen() // Open Claw Function
{
	motor[clawA] = -127;
	wait1Msec(650);
	motor[clawA] = 0;
}
void clawclose() // Close Claw Function
{
	motor[clawA] =127;
	wait1Msec(650);
	motor[clawA] = 0;
}
void liftChange(int degs) // Lift Up using encoders
{
	int true_goal = SensorValue[liftPot] + degs;
	if(true_goal > SensorValue[liftPot]) // Lift UP
	{
		while(SensorValue[liftPot] < abs(true_goal))
		{
			motor[catapultLeftA]  = 127;
			motor[catapultLeftB]  = 127;
			motor[catapultRightB] = 127;
		}
	}
	else // Lift DOWN
	{
		while(SensorValue[liftPot] > abs(true_goal))
		{
			motor[catapultLeftA]  = -127;
			motor[catapultLeftB]  = -127;
			motor[catapultRightB] = -127;
		}
	}
	motor[catapultLeftA]  = 15; // Hold Power
	motor[catapultLeftB]  = 15;
	motor[catapultRightB] = 15;
	return;
}
void gyroturn(int deg) // Turn Function using Gyroscope
{
	int gyroticks = deg*10; // Ticks of Gyro
	int P_Factor = 2.5; // P Factor for P Turn
	SensorValue[gyro] = 0; // Reset Gyro Value
	while(abs(SensorValue[gyro]) < abs(gyroticks)) // Wait for Gyro to Finish
	{
		motor[backLeft] =  SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		motor[backRight] =  SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		motor[frontLeft] = -1*SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		motor[frontRight] = -1*SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		wait1Msec(15);
	}
	motor[backLeft] = -1*SIGN(gyroticks)*15; // Brake Wheels
	motor[backRight] = -1*SIGN(gyroticks)*15;
	motor[frontLeft] = -1*SIGN(gyroticks)*15;
	motor[frontRight] = -1*SIGN(gyroticks)*15;
	wait1Msec(150);
	motor[backLeft] = 0; // Turn Off Wheels
	motor[backRight] = 0;
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
	wait1Msec(150);

}
void moveStrafeInches(int inches) // Strafe, Pos=Left Neg=Right
{
	int wheeldegs = (inches/12.566)*360;
	float P_Factor = 0.75;
	nMotorEncoder[backLeft] = 0;
	writeDebugStreamLine("%d", wheeldegs);
	while(abs(nMotorEncoder[backLeft]) < abs(wheeldegs))
	{
		motor[backLeft] = (30+ ((abs(nMotorEncoder[backLeft])-abs(wheeldegs))*P_Factor)*-1)*SIGN(wheeldegs);
		motor[backRight] = (30+ ((abs(nMotorEncoder[backLeft])-abs(wheeldegs))*P_Factor)*-1)*SIGN(wheeldegs);
		motor[frontLeft] = (30+ ((abs(nMotorEncoder[backLeft])-abs(wheeldegs))*P_Factor)*-1)*SIGN(wheeldegs);
		motor[frontRight] = (30+ ((abs(nMotorEncoder[backLeft])-abs(wheeldegs))*P_Factor)*-1)*SIGN(wheeldegs);
		wait1Msec(15);
	}
	motor[backLeft] = -1*SIGN(wheeldegs)*15;
	motor[backRight] = -1*SIGN(wheeldegs)*15;
	motor[frontLeft] = -1*SIGN(wheeldegs)*15;
	motor[frontRight] = -1*SIGN(wheeldegs)*15;
	wait1Msec(150);
	motor[backLeft] = 0;
	motor[backRight] = 0;
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
	wait1Msec(150);

}
void moveForwardsInches(int inches) // Y Axis, Pos=Forward Neg=Backwards
{
	int wheeldegs = (inches/12.566)*360; // Calculating Wheel Degrees
	nMotorEncoder[backLeft] = 0; // Reset Encoder
	writeDebugStreamLine("%d", wheeldegs);
	while(abs(nMotorEncoder[backLeft]) < abs(wheeldegs)) // Wait for Finish
	{
		motor[backLeft] = 127 * SIGN(wheeldegs);
		motor[backRight] = -1 * 127 * SIGN(wheeldegs);
		motor[frontLeft] = -1 * 127 * SIGN(wheeldegs);
		motor[frontRight] = 127 * SIGN(wheeldegs);
		wait1Msec(15);
	}
	motor[backLeft] = -1*SIGN(wheeldegs)*15; // Brake Wheels
	motor[backRight] = -1*SIGN(wheeldegs)*15;
	motor[frontLeft] = -1*SIGN(wheeldegs)*15;
	motor[frontRight] = -1*SIGN(wheeldegs)*15;
	wait1Msec(150);
	motor[backLeft] = 0; // Turn Off Wheels
	motor[backRight] = 0;
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
	wait1Msec(150);

}
int LIFT_TOP = 2086;
int LIFT_FLOOR = 0;
int LIFT_PERIMETER = 800;
int LIFT_FLING = 1400;

int liftgoal = 0;
int liftdone = 1;
task liftPosition // Task to Control Lift Position simultaneously with other subsystems
{
	int oldliftposition = LIFT_FLOOR; // Start Lift Positio
	while(1)
	{
		if(oldliftposition != liftgoal)
		{
			liftdone = 0;
			liftChange(liftgoal - oldliftposition); // If Goal is lower than current, lower lift.
			oldliftposition = liftgoal;
			liftdone = 1;
		}
		wait1Msec(15);
	}
}

void getPreload()
{
	liftgoal = LIFT_FLOOR;
	moveForwardsInches(30);
	wait1Msec(50);
	while(!liftdone){}
	clawclose();
	motor[clawA] = 70;
	moveForwardsInches(-50);
}
void flingShot()
{
	liftgoal = LIFT_TOP;
	while(SensorValue[liftPot] < LIFT_FLING){}
	clawopen();
	while(!liftdone){}
	wait1Msec(800);
}
void trueProgrammingSkills()
{
	startTask(liftPosition);

	// Row of 3 Near Wall
	clawclose();
	clawclose();
	moveForwardsInches(42);
	clawclose();
	moveForwardsInches(-42);
	moveStrafeInches(40);
	gyroturn(90);
	moveForwardsInches(-30);
	flingShot();

	// Preloads
	getPreload();
	flingShot();
	getPreload();
	flingShot();
	getPreload();
	flingShot();


	// Get Front Row of Three
	liftgoal = LIFT_FLOOR;
	wait1Msec(150);
	while(!liftdone){}
	gyroturn(-90);
	moveForwardsInches(72);
	clawclose();
	gyroturn(90);
	flingShot();

	// Get Tile Cube
	liftgoal = LIFT_FLOOR;
	wait1Msec(150);
	while(!liftdone){}
	moveForwardsInches(48);
	clawclose();
	moveForwardsInches(-48);
	flingShot();

	//Get Center Cube
	liftgoal = LIFT_FLOOR;
	wait1Msec(150);
	while(!liftdone){}
	gyroturn(55);
	moveForwardsInches(45);
	clawclose();
	moveForwardsInches(-45);
	gyroturn(-55);
	flingShot();

}
void auto_right_tile() // Cube Auto for Right Tile -- Same Comments as Above
{
	startTask(liftPosition);
	clawclose();
	gyroturn(-55);
	moveForwardsInches(45);
	clawclose();
	clawclose();
	motor[clawA] = 70;
	liftgoal = LIFT_PERIMETER;
	while(!liftdone){}
	wait1Msec(150);
	gyroturn(-110);
	moveStrafeInches(60);
	wait1Msec(500);
	moveForwardsInches(-48);
	flingShot();
	motor[backLeft] =- 80; // Turn Off Wheels
	motor[backRight] = 80;
	motor[frontLeft] = 80;
	motor[frontRight] = -80;
	liftChange(LIFT_FLOOR);
	motor[backLeft] = 0; // Turn Off Wheels
	motor[backRight] = 0;
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
}
void auto_left_tile()
{
	startTask(liftPosition);
	nMotorEncoder[backLeft] = 0;
	nMotorEncoder[catapultRightB] = 0;
	clawclose();
	gyroturn(55);
	moveForwardsInches(45);
	clawclose();
	clawclose();
	motor[clawA] = 70;
	liftgoal = LIFT_PERIMETER;
	while(!liftdone){}
	wait1Msec(150);
	gyroturn(110);
	moveStrafeInches(-60);
	wait1Msec(500);
	moveForwardsInches(-48);
	flingShot();
	motor[backLeft] =- 80; // Turn Off Wheels
	motor[backRight] = 80;
	motor[frontLeft] = 80;
	motor[frontRight] = -80;
	liftChange(LIFT_FLOOR);
	motor[backLeft] = 0; // Turn Off Wheels
	motor[backRight] = 0;
	motor[frontLeft] = 0;
	motor[frontRight] = 0;
}
void auto_star()
{
	startTask(liftPosition);
	moveForwardsInches(-12);
	clawclose();
	clawclose();
	clawclose();
	motor[clawA] = 70;
	moveForwardsInches(-60);
	flingShot();
}
void auto_drive_backwards()
{
	moveForwardsInches(-70);
}
float changeRange(float x, float in_min, float in_max, float out_min, float out_max) // Change Range Function, for squared Drive
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
int claw_toggle = 0;
void usercontrolfunction()
{
	// Drive
	int reverse = 1;
	float slow = 1;
	float x,y,z;
	x = changeRange(vexRT[Ch1],-127,127,-1,1); // Changing the Range for Squared Inputs
	y = changeRange(vexRT[Ch2],-127,127,-1,1);
	z = changeRange(vexRT[Ch4],-127,127,-1,1);
	x = x*x; // Squared Inputs for Better Control
	y = y*y;
	z = z*z;
	x = changeRange(x,-1,1,-127,127) * reverse * slow; // Change the Range back and add some multipliers
	y = changeRange(y,-1,1,-127,127) * reverse * slow;
	z = changeRange(z,-1,1,-127,127) * reverse * slow;
	if(vexRT[Ch1] < 0)
		x = x*-1;
	if(vexRT[Ch2] < 0)
		y = y*-1;
	if(vexRT[Ch4] < 0)
		z = z*-1;
	writeDebugStreamLine("%d %d %d", x, y, z); // Debug
	motor[frontLeft]  =  -y - z -x; // Just which way each motor needs to turn to go each direction
	motor[frontRight] =  y - z - x;
	motor[backLeft]   =  y + z - x;
	motor[backRight]  =  -y + z - x;
	wait1Msec(20);
	// Lift
	if(vexRT[Btn5U]) // Manual Negative
	{
		motor[catapultLeftA]  = -127;
		motor[catapultLeftB]  = -127;
		//motor[catapultRightA]  = -127;
		motor[catapultRightB]  = -127;
	}
	else if(vexRT[Btn6U]) // Manual Posiitve
	{
		motor[catapultLeftA]  = 127;
		//motor[catapultRightA] = 127;
		motor[catapultLeftB]  = 127;
		motor[catapultRightB] = 127;
	}
	else
	{
		motor[catapultLeftA]  = 15;
		//motor[catapultRightA] = 15;
		motor[catapultLeftB]  = 15;
		motor[catapultRightB] = 15;
	}
	// Claw

	if(vexRT[Btn7D])
	{
		while(vexRT[Btn7D]){}
		claw_toggle = (claw_toggle == 0 ? 1 : 0);
	}
	if(vexRT[Btn6D]) // Close Claw
	{
		claw_toggle = 0;
		motor[clawA]=127;
		//motor[ClawB]=127;
	}
	else if(vexRT[Btn5D]) // Open Claw
	{
		claw_toggle = 0;
		motor[clawA]=-127;
		//motor[ClawB]=-127;
	}
	else // No Claw Power
	{
		if(claw_toggle)
			motor[clawA] = 40;
		else
			motor[clawA]=0;
	}
}
// AUTO SELECTION OVERRIDES
/*
0 - Cube RIGHT
1 - Cube LEFT
2 - Star RIGHT
3 - Star LEFT
4 - Drive Backwards
5 - Programming Skills
*/
int OVERRIDE_AUTO = 0; // To Override: Change to 1
int OVERRIDE_AUTO_SELECTION = 0; // Auto Selection (Above)
void pre_auton()
{
	bStopTasksBetweenModes = true;
	if(OVERRIDE_AUTO == 0)
		LcdAutonomousSelection();
}
task autonomous()
{
	// ..........................................................................
	// Insert user code here.
	// ..........................................................................
	if(OVERRIDE_AUTO == 0)
	{
		if(MyAutonomous == 0)
		{
			auto_right_tile();
		}
		if(MyAutonomous == 1)
		{
			auto_left_tile();
		}
		if(MyAutonomous == 2)
		{
			auto_star();
		}
		if(MyAutonomous == 3)
		{
			auto_star();
		}
		if(MyAutonomous == 4)
		{
			auto_drive_backwards();
		}
		if(MyAutonomous == 5)
		{
			trueProgrammingSkills();
		}
	}
	else
	{
		if(OVERRIDE_AUTO_SELECTION == 0)
		{
			auto_right_tile();
		}
		if(OVERRIDE_AUTO_SELECTION == 1)
		{
			auto_left_tile();
		}
		if(OVERRIDE_AUTO_SELECTION == 2)
		{
			auto_star();
		}
		if(OVERRIDE_AUTO_SELECTION == 3)
		{
			auto_star();
		}
		if(OVERRIDE_AUTO_SELECTION == 4)
		{
			auto_drive_backwards();
		}
		if(OVERRIDE_AUTO_SELECTION == 5)
		{
			trueProgrammingSkills();
		}
	}
}
task usercontrol()
{
	// User control code here, inside the loop
	while(1)
	{
		usercontrolfunction();
		wait1Msec(25);
	}
}