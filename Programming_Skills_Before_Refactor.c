
void trueProgrammingSkills()
{
	moveForwardsInches(-10);
	clawclose();
	wait1Msec(1000);
	clawclose();
	clawclose();
	clawclose()
	motor[clawA] = 70;
	moveForwardsInches(-50);
	liftup(LIFT_TOP);
	clawopen();

	liftdown(LIFT_TOP);
	liftdown(LIFT_TOP);
	liftdown(LIFT_TOP);
	moveStrafeInches(-12);
	moveForwardsInches(25);
	wait1Msec(2000);
	clawclose();
	motor[clawA] = 70;
	liftup(250);
	moveForwardsInches(-50);
	liftup(LIFT_TOP);
	clawopen();

	liftdown(LIFT_TOP);
	liftdown(LIFT_TOP);
	liftdown(LIFT_TOP);
	moveStrafeInches(-12);
	moveForwardsInches(40);
	wait1Msec(2000);
	clawclose();
	motor[clawA] = 70;
	liftup(500);
	moveForwardsInches(-50);
	liftup(LIFT_TOP);
	clawopen();

}
