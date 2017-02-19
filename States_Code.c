#pragma config(UART_Usage, UART1, uartVEXLCD, baudRate19200, IOPins, None, None)
#pragma config(UART_Usage, UART2, uartNotUsed, baudRate4800, IOPins, None, None)
#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    liftPot,        sensorPotentiometer)
#pragma config(Sensor, in2,    gyro,           sensorGyro)
#pragma config(Sensor, in3,    clawPot,        sensorPotentiometer)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           clawB,         tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           catapultRightB, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           frontLeft,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           catapultLeftA, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           backLeft,      tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port6,           backRight,     tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port7,           clawA,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           catapultLeftB, tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           frontRight,    tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          frontLeft,     tmotorNone, openLoop)
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
#define MAX_CHOICE  6

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
		displayLCDString(0, 0, "Preload Star");
		break;
	case    3:
		displayLCDString(0, 0, "Back Star Left");
		break;
	case    4:
		displayLCDString(0, 0, "Back Star Right");
		break;
	case    5:
		displayLCDString(0, 0, "Drive Backwards");
		break;
	case    6:
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

	while( 1 )
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
int lift_completed = 0;
task liftWatchdog
{
	wait1Msec(250);
	int oldpos = SensorValue[liftPot];
	int change_pos = 0;
	float esti_change = 1000;
	while(1)
	{
		change_pos = abs(SensorValue[liftPot] - oldpos);
		esti_change = change_pos / .1;
		if(esti_change < 25)
		{
			lift_completed = 1;
			break;
		}
		oldpos = SensorValue[liftPot];
		wait1Msec(100);
	}
}
void liftChange(int degs) // Lift Up using encoders
{
	int true_goal = SensorValue[liftPot] + (degs*-1);
	lift_completed = 0;
	startTask(liftWatchdog);
	if(true_goal < SensorValue[liftPot]) // Lift UP
	{
		while(SensorValue[liftPot] > true_goal || lift_completed)
		{
			motor[catapultLeftA]  = 127;
			motor[catapultLeftB]  = 127;
			motor[catapultRightB] = 127;
		}
	}
	else // Lift DOWN
	{
		while(SensorValue[liftPot] < abs(true_goal) || lift_completed)
		{
			motor[catapultLeftA]  = -127;
			motor[catapultLeftB]  = -127;
			motor[catapultRightB] = -127;
		}
	}
	motor[catapultLeftA]  = 15; // Hold Power
	motor[catapultLeftB]  = 15;
	motor[catapultRightB] = 15;
	stopTask(liftWatchdog);
	return;
}
void gyroturn(int deg) // Turn Function using Gyroscope
{
	int gyroticks = deg*9; // Ticks of Gyro
	int P_Factor = 2.5; // P Factor for P Turn
	SensorValue[gyro] = 0; // Reset Gyro Value
	while(abs(SensorValue[gyro]) < abs(gyroticks)) // Wait for Gyro to Finish
	{
		motor[backLeft] =  SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		motor[backRight] =  SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor);
		motor[frontLeft] = -1 * (SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor));
		motor[frontRight] = -1 * (SIGN(gyroticks)*((abs(gyroticks)-abs(SensorValue[gyro]))*P_Factor));
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
	writeDebugStreamLine("wheeldegs %d %d", wheeldegs, SIGN(wheeldegs));
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

void trueMoveForwardsInches(int inches) // Y Axis, Pos=Forward Neg=Backwards
{
	int wheeldegs = (inches/12.566)*360; // Calculating Wheel Degrees
	nMotorEncoder[backLeft] = 0; // Reset Encoder
	writeDebugStreamLine("wheeldegs %d %d", wheeldegs, SIGN(wheeldegs));
	while(abs(nMotorEncoder[backLeft]) < abs(wheeldegs)) // Wait for Finish
	{
		motor[backLeft] = -1*127 * SIGN(wheeldegs);
		motor[backRight] = 127 * SIGN(wheeldegs);
		motor[frontLeft] = 127 * SIGN(wheeldegs);
		motor[frontRight] = -1*127 * SIGN(wheeldegs);
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
int LIFT_TOP = 1450;
int LIFT_FLOOR = 3350;
// int LIFT_PERIMETER = 3000;
int LIFT_FLING = 1450;

int liftgoal = LIFT_FLOOR;
int liftdone = 1;
task liftPosition // Task to Control Lift Position simultaneously with other subsystems
{
	int oldliftposition = LIFT_FLOOR; // Start Lift Positio
	while(1)
	{
		if(oldliftposition != liftgoal)
		{
			writeDebugStreamLine("New Goal");
			writeDebugStreamLine("Lift Change: %d", oldliftposition - liftgoal);
			writeDebugStreamLine("Lift Goal: %d", liftgoal);
			writeDebugStreamLine("Lift Old Position: %d", oldliftposition);
			liftdone = 0;
			liftChange(oldliftposition - liftgoal); // If Goal is lower than current, lower lift.
			oldliftposition = liftgoal;
			liftdone = 1;
		}
		wait1Msec(15);
	}
}

void clawopen() // Open Claw Function
{
	motor[clawA] = -127;
	motor[clawB] = -127;
	wait1Msec(650);
	motor[clawA] = 00;
	motor[clawB] = 00;
}
void clawclose() // Close Claw Function
{
	motor[clawA] = 127;
	motor[clawB] = 127;
	wait1Msec(650);
	motor[clawA] = 00;
	motor[clawB] = 00;
}
void clawopentime(int time) // Open Claw Function
{
	motor[clawA] = -127;
	motor[clawB] = -127;
	wait1Msec(time);
	motor[clawA] = 00;
	motor[clawB] = 00;
}
void clawclosetime(int time) // Close Claw Function
{
	motor[clawA] = 127;
	motor[clawB] = 127;
	wait1Msec(time);
	motor[clawA] = 00;
	motor[clawB] = 00;
}
void clawhold()
{
	motor[clawA] = 70;
	motor[clawB] = 70;
}
void getPreload()
{
	liftgoal = LIFT_FLOOR;
	wait1Msec(150);
	while(!liftdone){}
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	trueMoveForwardsInches(-45);
	clawclose();
	motor[clawA] = 70;
	motor[clawB] = 70;
	moveForwardsInches(-50);
}

void flingShot()
{
	liftgoal = LIFT_TOP;
	while(SensorValue[liftPot] > LIFT_FLING){}
	clawopen();
	while(!liftdone){}
	wait1Msec(800);
}
void liftDown()
{
	liftgoal = LIFT_FLOOR;
	wait1Msec(150);
	while(!liftdone){}
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;

}
void auto_true_cube(int right)
{
	clawclosetime(250);
	startTask(liftPosition);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	clawclose();
	trueMoveForwardsInches(-40);
	int turn = 75 * (right == 1 ? -1 : 1);
	gyroturn(turn);
	trueMoveForwardsInches(-45);
	clawclose();
	clawhold();
	motor[catapultLeftA]  = 127;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = 127;
	motor[catapultRightB] = 127;
	wait1Msec(300);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	wait1Msec(500);

	moveForwardsInches(-40);
	gyroturn(turn);
	moveForwardsInches(-40);
	LIFT_FLING = 1850;
	flingShot();
}
void lifttouch()
{
	motor[catapultLeftA]  = 127;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = 127;
	motor[catapultRightB] = 127;
	wait1Msec(350);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
}
void auto_drive_backwards()
{
	clawclosetime(250);
	moveForwardsInches(-70);
}
void preloadSkills()
{
	startTask(liftPosition);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	moveStrafeInches(18);
	// Row of 3 Near Wall
	clawclosetime(825);
	moveStrafeInches(-24);
	trueMoveForwardsInches(-52);
	clawclose();
	clawhold();
	moveForwardsInches(-48);
	lifttouch();
	moveStrafeInches(30);

	gyroturn(80);
	moveForwardsInches(-35);
	LIFT_FLING = 1850;
	writeDebugStreamLine("here1");
	flingShot();
	writeDebugStreamLine("here2");
	getPreload();
	flingShot();
	LIFT_FLING += 80;
	getPreload();
	flingShot();
	LIFT_FLING += 80;
	getPreload();
	flingShot();

	liftDown();
	trueMoveForwardsInches(-36);
	gyroturn(-90);
	trueMoveForwardsInches(-40);
	gyroturn(90);
	moveForwardsInches(-14);
	LIFT_FLING += 80;
	flingShot();

	liftDown();
	trueMoveForwardsInches(-14);
	gyroturn(-90);
	trueMoveForwardsInches(-40);
	gyroturn(90);
	trueMoveForwardsInches(-18);
	clawclose();
	clawhold();
	moveForwardsInches(-50);
	LIFT_FLING += 80;
	flingShot();

}
void star_true()
{
	clawclosetime(250);
	startTask(liftPosition);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	moveForwardsInches(-14);
	clawclose();
	clawclose();
	clawclose();
	clawhold();
	moveForwardsInches(-60);
	motor[catapultLeftA]  = 127;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = 127;
	motor[catapultRightB] = 127;
	wait1Msec(2000);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	clawopen();
}
void back_star_auto(int turn)
{
	clawclosetime(250);
	startTask(liftPosition);
	motor[catapultLeftA]  = -15;
	//motor[catapultRightA] = 15;
	motor[catapultLeftB]  = -15;
	motor[catapultRightB] = -15;
	moveStrafeInches(18);
	// Row of 3 Near Wall
	clawclosetime(825);
	moveStrafeInches(-24);
	trueMoveForwardsInches(-52);
	clawclose();
	clawhold();
	moveForwardsInches(-48);
	lifttouch();
	moveStrafeInches(30);
	int turn_deg = 80 * (turn == 0 ? 1 : -1);
	gyroturn(turn_deg);
	moveForwardsInches(-35);
	LIFT_FLING = 1850;
	writeDebugStreamLine("here1");
	flingShot();
	writeDebugStreamLine("here2");
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
		motor[catapultLeftA]  = -15;
		//motor[catapultRightA] = 15;
		motor[catapultLeftB]  = -15;
		motor[catapultRightB] = -15;
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
		motor[clawB]=127;
	}
	else if(vexRT[Btn5D]) // Open Claw
	{
		claw_toggle = 0;
		motor[clawA]=-127;
		motor[clawB]=-127;
		writeDebugStreamLine("%d", SensorValue[liftPot]);
	}
	else // No Claw Power
	{
		if(claw_toggle)
		{
			motor[clawA] = 45;
			motor[clawB] = 45;
		}
		else
		{
			motor[clawA]=0;
			motor[clawB]=0;
		}
	}
}

// AUTO SELECTION OVERRIDES
/*
0 - Cube RIGHT
1 - Cube LEFT
2 - Star
3 - Back Star Left
4 - Back Star Right
5 - Drive Backwards
6 - Programming Skills
7 - Fling Shot Tester
*/
int OVERRIDE_AUTO = 0; // To Override: Change to 1
int OVERRIDE_AUTO_SELECTION = 5; // Auto Selection (Above)
task LCDControl
{
	if(OVERRIDE_AUTO == 0)
		LcdAutonomousSelection();
	else
		bDisplayCompetitionStatusOnLcd = true;
}
void pre_auton()
{
	stopTask(autonomous);
	stopTask(usercontrol);
	stopTask(liftPosition);
	bStopTasksBetweenModes = false;
	bDisplayCompetitionStatusOnLcd = false;
	startTask(LCDControl);
}
task autonomous()
{
	stopTask(usercontrol);
	stopTask(liftPosition);
	// ..........................................................................
	// Insert user code here.
	// ..........................................................................
	if((MyAutonomous == 0 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 0 && OVERRIDE_AUTO == 1)) // CUBE RIGHT
	{
		auto_true_cube(1);
	}
	else if((MyAutonomous == 1 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 1 && OVERRIDE_AUTO == 1)) // CUBE LEFT
	{
		auto_true_cube(0);
	}
	else if((MyAutonomous == 2 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 2 && OVERRIDE_AUTO == 1)) // PRELOAD STAR
	{
		star_true();
	}
	else if((MyAutonomous == 3 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 3 && OVERRIDE_AUTO == 1)) // BACK STAR LEFT
	{
		back_star_auto(0);
	}
	else if((MyAutonomous == 4 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 4 && OVERRIDE_AUTO == 1)) // BACK STAR RIGHT
	{
		back_star_auto(1);
	}
	else if((MyAutonomous == 5 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 5 && OVERRIDE_AUTO == 1)) // DRIVE BACKWARDS
	{
		auto_drive_backwards();
	}
	else if((MyAutonomous == 6 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 6 && OVERRIDE_AUTO == 1)) // PROGRAMMING SKILLS
	{
		preloadSkills();
	}
	else if((MyAutonomous == 7 && OVERRIDE_AUTO == 0) || (OVERRIDE_AUTO_SELECTION == 7 && OVERRIDE_AUTO == 1)) // FLING SHOT TESTER
	{
			startTask(liftPosition);
			flingShot();
	}
}
task usercontrol()
{
	stopTask(autonomous);
	stopTask(liftPosition);
	// User control code here, inside the loop
	while(1)
	{
		usercontrolfunction();
		wait1Msec(25);
	}
}
