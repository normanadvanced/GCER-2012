// Created on Thu Jun 28 2012

#include <stdio.h>

#include <stdio.h>
#define left_motor 0
#define right_motor 3
#define left_motor_speed 800
#define stuffer 1
#define stuffer_open 250
#define stuffer_closed 1525
#define catcher_closed 830
#define catcher_open 1750
#define release_motor 2
#define et_sensor 0
#define	ideal_distance 650
#define top_hat 1
#define dropper 3
#define Light 2


void stuff()
{
	int n;
	n=0;
	for(n=0;n<2;++n)
	{
		//open and close stuffer
		set_servo_position(stuffer,stuffer_closed);
		msleep(500);
		set_servo_position(stuffer,stuffer_open);
		msleep(500);
		//shake?
	}
}
void shake()
{
	mrp(left_motor,900,75);
	mrp(right_motor,900,75);
	bmd(left_motor);
	bmd(right_motor);
	mrp(left_motor,900,-75);
	mrp(right_motor,900,-75);
	bmd(left_motor);
	bmd(right_motor);
}

void move_straight()
{
	int r_speed = (int)((float)left_motor_speed*((float)analog10(et_sensor)/(float)ideal_distance));
	mav(left_motor,left_motor_speed);
	mav(right_motor,r_speed);
	msleep(50);
	printf("ET=%d.\tSpeed=%d\tTop Hat=%d\n",analog10(et_sensor), r_speed, analog10(top_hat));
}

void light_start()
{
	int light_off,light_on;
	while(1)
	{
		printf("Hold A for OFF calibration\n");
		while(!a_button())
		{
			msleep(50);
		}
		while(a_button())
		{
			light_off=analog10(Light);
			msleep(50);
		}
		printf("Off value=%d\n",light_off);
		printf("Hold B for ON calibration\n");
		while(!b_button())
		{
			msleep(50);
		}
		while(b_button())
		{
			light_on=analog10(Light);
			msleep(50);
		}
		printf("On value=%d\n",light_on);
		if(((light_on-light_off)>120)||((light_on-light_off)<-120))
		{
			printf("Calibrated\n");
			break;
		}
	}
	printf("Press the A button\n");
	while(!a_button())
	{
		msleep(50);
	}
	while(a_button())
	{
		msleep(50);
	}
	printf("Ready to start\n");
	while(analog10(Light)>((light_on+light_off)/2))
	{
		msleep(50);
	}
}

void setup()
{
	printf("Am I running with a light?\n");
	printf("Press A if with a light, B if not.\n");
	//wait until a button is pressed
	while(!a_button()&&!b_button())
	{
		msleep(50);
	}
	//command for button A
	if(a_button())
	{
		while(a_button())
		{
			msleep(50);
		}
		printf("Running with a light.\n");
		light_start();
	}
	//command for button B
	if(b_button())
	{
		while(b_button())
		{
			msleep(50);
		}
		printf("Running without a light.\n");
	}
}

int main()
{
	int m;
	int seconds_initial,seconds_final;
	float time_start,time_safe;
	//
	set_each_analog_state(1,0,0,0,0,0,0,0);
	set_servo_position(dropper,1500);
	//
	setup();
	//
	shut_down_in(115.0);
	time_start=seconds();
	set_servo_position(stuffer,stuffer_open);

	mrp(left_motor,950,-691);
	mrp(right_motor,975,720);
	bmd(left_motor);
	bmd(right_motor);
	//move some before edge sensing
	mrp(left_motor,950,6000);
	mrp(right_motor,975,6025);
	bmd(left_motor);
	bmd(right_motor);
	set_servo_position(dropper,640);
	msleep(200);

	//move to stuff
	while(analog10(1)<825)
	{
		move_straight();
	}
	ao();


	//drop basket
	set_servo_position(dropper,50);
	msleep(200);

    mrp(left_motor,975,-160);
	mrp(right_motor,975,-160);
	bmd(left_motor);
	bmd(right_motor);
	off(left_motor);
	off(right_motor);

	//put in 3 tribbles here while shaking
	msleep(1250);
	seconds_initial=seconds();
	seconds_final=(seconds_initial+1.25);
	while(seconds()<seconds_final)
	{
		shake();
	}
	stuff();


	//put in the rest of tribbles while shaking
	seconds_initial=seconds();
	seconds_final=(seconds_initial+1.25);
	while(seconds()<seconds_final)
	{
		shake();
	}


	//move to load ping-pong balls
	seconds_initial=seconds();
	seconds_final=(seconds_initial+1);
	while(seconds()<seconds_final)
	{
		move_straight();
	}


	//move to load ping-pong balls & stuff
	mrp(left_motor,975,1160);
	mrp(right_motor,975,1160);
	bmd(left_motor);
	bmd(right_motor);

	//jerk to fix basket
	mrp(right_motor,950,-700);
	bmd(right_motor);
	msleep(100);
	mrp(right_motor,950,700);
	bmd(right_motor);

	//load ping-pong balls while stuffing and shaking
	seconds_initial=seconds();
	seconds_final=(seconds_initial+5);
	while(seconds()<seconds_final)
	{
		stuff();
		shake();
	}


	//make sure robot is aligned
	mrp(left_motor,9501,1150);
	mrp(right_motor,950,1150);
	bmd(left_motor);
	bmd(right_motor);
	stuff();

	//
	time_safe=seconds();
	printf("Time to get safe=%f.\n",seconds());
	//


	//unload tribbles
	mrp(release_motor,950,-500);
	bmd(release_motor);
	msleep(200);


	//back up to load any unloaded ping-pong balls
	mrp(left_motor,900,-400);
	mrp(right_motor,900,-400);
	bmd(left_motor);
	bmd(right_motor);


	//turn to load ping-pong balls
	mrp(left_motor,950,-500);
	bmd(left_motor);


	for(m=0;m<3;++m);
	{
		shake();
		msleep(20);
	}

	//
	printf("Done %f.\n",(seconds()-time_start));
	ao();
}
