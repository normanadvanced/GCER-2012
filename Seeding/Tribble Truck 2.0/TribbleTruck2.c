
#include <stdio.h>
#include "cbcnavlib.h"
#include "CBCservo.hpp"


#define claw_servo 2
#define arm_servo 0
#define dump_mot 1
#define top_pos 0
#define bot_pos 1800
#define mid_pos 1400
#define open_pos 1500
#define close_pos 600
#define et_left 1
#define et_right 0
#define touch_right 8
#define touch_left 9

int arm_pos = top_pos;
int claw_pos = close_pos;
int dump_pos;


void lower_arm(int speed)
{
        servo_mtp(arm_servo, speed, bot_pos);

}

void raise_arm(int speed)
{
        servo_mtp(arm_servo, speed, top_pos);
}

void close_claw(int speed)
{

        servo_mtp(claw_servo, speed, close_pos);
}

void open_claw(int speed)
{
        servo_mtp(claw_servo, speed, open_pos);
}

//Align the cbc with the top hat sensors
void cbc_align(long speed)
{
    mav(robot.leftWheel, speed);
    mav(robot.rightWheel, speed);
    printf("left et: %d, right et: %d\n", analog10(et_left), analog10(et_right));
    int t_out=0;
    while(((analog10(et_left) < 850)||(analog10(et_right) < 850)) && t_out<500)
    {
        if(analog10(et_left) >= 850)
        {
            off(robot.leftWheel);
        }
        if(analog10(et_right) >= 850)
        {
            off(robot.rightWheel);
        }
        msleep(10);
        printf("left top: %d, right top: %d\n", analog10(et_left), analog10(et_right));
        t_out++;
    }
}

int cbc_align_white(long speed)
{
    mav(robot.leftWheel, speed);
    mav(robot.rightWheel, speed);
    while(analog10(et_left) > 900 || analog10(et_left) > 900)
    {
        if(analog10(et_left) <= 900)
        {
            off(robot.leftWheel);
        }
        if(analog10(et_right) <= 900)
        {
            off(robot.rightWheel);
        }
        msleep(10);
    }
}

void arm_mid(int speed)
{
    servo_mtp(arm_servo, speed, mid_pos);
}

void cbc_touch(long speed, long timeout)
{
    int i = 0;

	mav(robot.leftWheel, speed);
    mav(robot.rightWheel, speed);

    while(!(digital(touch_left) && digital(touch_right)) && i < (timeout / 50))
    {
		if(digital(touch_left))
		{
			mav(robot.leftWheel, speed/10);
		}
		if(digital(touch_right))
		{
			mav(robot.rightWheel, speed/10);
		}
		i++;
		msleep(50L);
    }
	off(robot.leftWheel);
	off(robot.rightWheel);
}

void follow_tape_left(int speed)
{
	int i;
    while(!digital(touch_left) && i < 50)
	{
		if(analog10(et_left)<850)
		{
			mav(robot.rightWheel,(int)((float)speed*1.15));
			mav(robot.leftWheel,(int)((float)speed*0.85));
		}
		else
		{
			mav(robot.rightWheel,(int)((float)speed*0.85));
			mav(robot.leftWheel,(int)((float)speed*1.15));
		}
		msleep(50L);
		i++;
	}
	off(robot.rightWheel);
	off(robot.leftWheel);
}

void light_it_up(int light_port)
{
	int l_on_, l_off_, l_mid_, OK=0;
	float s;

	while (!OK) {
		cbc_display_clear();
		cbc_printf (0,0,"CALIBRATE: sensor port #%d", light_port);

		cbc_printf(0,1,"  press <-- when light on");
		while(!left_button()){/*sensor value when light is on*/
			cbc_printf(0,2,"  value is %d, bright = low   ",l_on_=analog10 (light_port));
			msleep(50);
		}

		cbc_printf(0,1,"  light on value is = %d        ", l_on_);
		sleep(1.);

		cbc_printf(0,2,"  press --> when light off             ");
		while(!right_button()){	/*sensor value when light is off*/
			cbc_printf(0,3,"   value is %d, dark = high   ",l_off_=analog10 (light_port));
			msleep(50);
		}

		cbc_printf(0,2,"  light off value is = %d         ", l_off_);
		sleep(1.);

		cbc_printf(0,3,"                              ");

		if((l_off_-l_on_)>=120){ /*bright = small values */
			OK=1;
			l_mid_=(l_on_+l_off_)/2;
			cbc_printf(0,4,"Good Calibration!");
			cbc_printf(0,6,"Diff = %d:  WAITING",l_off_-l_on_);
			while(analog10(light_port)>l_mid_){
				cbc_printf(0,7,"Value = %d; Threshold = %d   ",analog10(light_port),l_mid_);
				msleep(25);
			}
			cbc_printf(0,6,"Going!                      ");
			cbc_printf(0,7,"Value = %d; Threshold = %d   ",analog10(light_port),l_mid_);
		}
		else{
			s=seconds();
			cbc_printf(0,6,"BAD CALIBRATION");
			if(l_off_<512){
				cbc_printf(0,7,"   Add Shielding!!");
			}
			else{
				cbc_printf(0,7,"   Aim sensor!!");
			}
		}
	}
}


int main()
{
    struct cbcRobot robot = {0,3,150,55,1100};  //Define the robot. 0 and 3 are the motor ports,
												//150 mm is the wheel distance
                                                //55 is wheel diameter, 1100 is ticks per rotation

	light_it_up(4);                  //Wait for the light to turn on

	set_servo_position(arm_servo, top_pos);     //Set the arm servo to the top position

    shut_down_in(118.0);             //Shut down in 118 seconds

    //msleep(5000);                    //Wait 5 seconds

	float initial_time=seconds();    //Create a variable that records the initial time

    cbc_align(400);                  //Align with both of the top hats on the black line
    ao();
    cbc_align_white(300);

    lower_arm(600);                  //Lower claw and open arm
	open_claw(600);

	cbc_straight(140, 800);          //Straight out to get away from pvc
	wait_for_cbc();

	cbc_arc_left(160, 135, 100);     //Get past pvc to lower arm and open claw
	wait_for_cbc();

	cbc_straight(180, 800);          //Go straight into tribbles
	wait_for_cbc();

	close_claw(1000);                //Close claw on tribbles
	cbc_straight(280, 800);
	wait_for_cbc();

	raise_arm(1200);                 //Raise the arm
	msleep(800);

    cbc_arc_right(180, 34, 80);      //Arc right to get into position to approach the lattice wall
    wait_for_cbc();

    cbc_touch(700, 5000);            //Bump into lattice wall
    msleep(300);

    cbc_straight(-100, 600);         //Back up 10 cm from wall
    wait_for_cbc();

    open_claw(800);                  //Open claw to release tribbles and turn 94 degrees right
    cbc_spin(-94, 500);
    wait_for_cbc();

    cbc_straight(120, 700);          //Go forward into the turnstile and lower arm to mid position
    arm_mid(2000);
    msleep(1200);

    cbc_spin(20, 500);               //Turn 20 degrees left to hit the turnstile out of the way
    wait_for_cbc();
    msleep(200);

    cbc_spin(-16, 700);              //Turn back 16 degrees to the right and close the claw
    close_claw(800);
    wait_for_cbc();

    cbc_straight(-280, 800);         //Go backwards to get into position to grab the tribbles
    wait_for_cbc();

    lower_arm(1000);                 //Lower the arm and open the claw
	msleep(800);
    open_claw(800);
    msleep(1000);

    cbc_spin(8,700);                 //Turn 8 degrees to get ready to grab the tribbles
    wait_for_cbc();

    cbc_arc_left(1500, 6, 100);      //Go nearly straight into the tribbles
    wait_for_cbc();

    close_claw(200);                 //Go forward and grab the tribbles
    cbc_straight(200, 600);
    wait_for_cbc();
    close_claw(600);
    msleep(800);

    cbc_straight(-60, 600);          //Back up to clear the turnstile
    raise_arm(1000);
    wait_for_cbc();

    cbc_spin(4,700);                 //Turn slightly left
    wait_for_cbc();

    follow_tape_left(600);           //Run into the turnstile for horizonal alignment

    cbc_straight(-100, 400);         //Back up before crossing over to the other side
    wait_for_cbc();

    cbc_spin(90, 500);               //Turn 90 degrees to the left
    wait_for_cbc();

    cbc_straight(-80, 600);          //Back up and align with the black tape
    wait_for_cbc();
    cbc_align(400);
    ao();

    cbc_straight(-50, 300);          //Back up and align with the black tape
    wait_for_cbc();
    cbc_align(400);
    ao();

	cbc_straight(540,800);           //Go forward to the other side
	wait_for_cbc();

    open_claw(1000);                 //Open the claw to release tribbles and arc right approximately 100 degrees
	mrp(robot.leftWheel,780,2340);
	mrp(robot.rightWheel,400,1200);
	bmd(robot.leftWheel);
	bmd(robot.rightWheel);
	msleep(200L);

    lower_arm(1400);//Go forward and lower the arm
    msleep(500L);
	cbc_straight(370, 800);
	wait_for_cbc();

    close_claw(1000);                //Go forward and close the claw
	cbc_straight(300,800);
    wait_for_cbc();

    raise_arm(1500);                 //Raise the arm
	msleep(800);

    cbc_spin(-73,500);               //Turn 73 degrees right
    wait_for_cbc();

    open_claw(1000);                 //Run into the lattice wall and open the claw to release the tribbles
	cbc_touch(500, 5000);

    cbc_straight(-80, 600);          //Back up 8 cm from wall
    wait_for_cbc();

    cbc_spin(-92, 500);              //Turn 92 degrees right and close the claw
	close_claw(1000);
    wait_for_cbc();

    cbc_straight(-80, 600);          //Go backwards to get into position to grab the tribbles
    wait_for_cbc();

    lower_arm(1500);                 //Lower the arm and open the claw
	msleep(800);
    open_claw(800);
    msleep(1000);

    cbc_spin(4,700);                 //Turn a bit in order to put the robot in an optimal tribble gathering position
    wait_for_cbc();

    cbc_arc_left(1500, 6, 100);      //Go nearly straight into the tribbles
    wait_for_cbc();

    close_claw(200);                 //Go forward and grab tribbles
    cbc_straight(180, 600);
    wait_for_cbc();
    close_claw(600);
    msleep(800);

    cbc_straight(-80, 400);          //Back up to clear the turnstile
    wait_for_cbc();

    raise_arm(1200);                 //Raise the arm
    msleep(500);

    cbc_straight(220, 700);          //Go forward into the turnstile
    wait_for_cbc();

    cbc_spin(80, 500);               //Turn 80 degrees to the left
    wait_for_cbc();

    cbc_straight(-80, 600);          //Back up and align with the black tape
    wait_for_cbc();
    cbc_align(400);
    ao();

    cbc_straight(-50, 300);          //Back up and align with the black tape
    wait_for_cbc();
    cbc_align(400);
    ao();

    //cbc_straight(1080, 800);       //Old go forward function
    //wait_for_cbc();

	mrp(robot.rightWheel,825,7700);  //Go forward and correct the non-straightness of the motors
	mrp(robot.leftWheel,800,7700);
	wait_for_cbc();

    cbc_touch(500, 5000);            //Run into the wall
    msleep(300);

    cbc_straight(-125, 600);         //Back up 12.5 cm from wall
    wait_for_cbc();

    cbc_spin(90, 500);               //Turn 90 degrees to the left
    wait_for_cbc();

	while(analog10(et_right)<850)    //While the tape is not seen by the right top hat
	{
		mav(robot.rightWheel,615);   //Go forward
		mav(robot.leftWheel,600);
		msleep(25L);
	}
	off(robot.rightWheel);           //Turn the motors off
	off(robot.leftWheel);

    cbc_straight(240, 700);          //Go forward to get under the injection chute
    wait_for_cbc();

	msleep(8000);                    //Wait for the injection chute tribbles to be loaded up

	cbc_straight(250, 700);          //Go forward into the wall
    wait_for_cbc();

    cbc_straight(-40, 600);          //Back up 4 cm from pvc
    wait_for_cbc();

    arm_mid(1000);                   //Lower the arm to the mid position and open the claw
    msleep(800);
    open_claw(800);
    msleep(800);

    cbc_straight(-40, 600);          //Back up 4 cm from pvc
    wait_for_cbc();

	close_claw(1000);                //Turn around 180 degrees and close the claw
	cbc_spin(180,600);
    wait_for_cbc();

    cbc_straight(-80, 480);          //Back up to wall
    wait_for_cbc();

    lower_arm(1400);                 //Lower the arm
	msleep(800L);

    mrp(dump_mot, 200, 500);         //Dump the tribble into the MPA
    bmd(dump_mot);
    msleep(1000);

	mrp(dump_mot, 200, -500);        //Dump the tribble into the MPA
    bmd(dump_mot);

	mrp(dump_mot, 200, 500);         //Dump the tribble into the MPA
    bmd(dump_mot);

	mrp(dump_mot, 200, -500);        //Dump the tribble into the MPA
    bmd(dump_mot);

	float time = seconds() - initial_time;   //Create a variable that represents the time taken for the robot to complete its program

	cbc_display_clear();             //Clear the CBC display and then print the time the robot took to complete its program
	printf("Time: %f",time);

    return 0;
}
