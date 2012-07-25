	// Created on Fri Jun 29 2012

#include <pthread.h>
#include <stdio.h>
#include "cbclib.h"
#include "createlib.h"
#include "lightstart.h"

#define KELP_PORT 3
#define KELP_TOP 940
#define KELP_BOTTOM 515
#define KELP_SCORE 1600
#define GRAB_PORT 1
#define GRAB_DOWN 320
#define GRAB_HOLD 1100
#define GRAB_SCORE 1570
#define DUMP_UP 300
#define DUMP_SPEED 1000
#define DUMP_PORT 3
#define BOTGUY_ARM_PORT 2
#define BOTGUY_ARM_MOVE 370
#define BOTGUY_CLAW_PORT 1
#define BOTGUY_CLAW_MOVE 230
#define LOW_SPEED 150
#define MID_SPEED 300
#define HIGH_SPEED 400

#define LIGHT_PORT 7
#define ACCEL_MAX 300.0

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
void init_servo()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1450, 6, 6);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 6, 6);
	set_servo_position(kelp_arm->port, KELP_SCORE);
	set_servo_position(kelp_grab->port, GRAB_SCORE);
}
void lateral_shift()
{
	create_drive_segment(LOW_SPEED + 50, 100);
	create_drive_arc(LOW_SPEED, 110, 45);
	create_drive_arc(LOW_SPEED, -110, -39);
	create_drive_segment(LOW_SPEED, -116);
	create_cease();
}
void dump_kelp()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 15, 5);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 15, 5);
	wait_servo(kelp_arm, KELP_SCORE + 200);
	wait_servo(kelp_grab, GRAB_SCORE);
	mrp(DUMP_PORT, DUMP_SPEED, DUMP_UP);
	bmd(DUMP_PORT);
	mrp(DUMP_PORT, DUMP_SPEED / 2, -DUMP_UP);
	bmd(DUMP_PORT);
	mrp(DUMP_PORT, DUMP_SPEED, DUMP_UP);
	bmd(DUMP_PORT);
	mrp(DUMP_PORT, DUMP_SPEED / 2, -DUMP_UP);
	bmd(DUMP_PORT);
	off(DUMP_PORT);
}
void dump_kelp_de()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 15, 5);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 15, 5);
	wait_servo(kelp_arm, KELP_SCORE + 200);
	wait_servo(kelp_grab, GRAB_SCORE);
	mrp(DUMP_PORT, DUMP_SPEED, DUMP_UP);
	bmd(DUMP_PORT);
	mrp(DUMP_PORT, DUMP_SPEED / 2, -DUMP_UP);
	bmd(DUMP_PORT);
	off(DUMP_PORT);
}
void set_top()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 15, 5);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 15, 5);
	wait_servo(kelp_arm, KELP_TOP);
	wait_servo(kelp_grab, GRAB_DOWN);
}
void grab_top()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 12, 6);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 10, 6);
	wait_servo(kelp_grab, GRAB_HOLD);
	start_process(lateral_shift);
	wait_servo(kelp_arm, KELP_SCORE);
	wait_servo(kelp_grab, GRAB_SCORE);
	msleep(500L);
}
void grab_top_de()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 12, 6);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 10, 6);
	wait_servo(kelp_grab, GRAB_HOLD);
	//start_process(lateral_shift);
	wait_servo(kelp_arm, KELP_SCORE);
	wait_servo(kelp_grab, GRAB_SCORE);
	msleep(500L);
}
void grab_bottom()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 15, 5);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 15, 5);
	wait_servo(kelp_arm, KELP_BOTTOM);
	wait_servo(kelp_grab, GRAB_DOWN);
	create_accel_straight(0, -ACCEL_MAX + 60.0, -70.0);
	create_stop();
	kelp_arm = build_servo(GRAB_PORT, 440, 1650, 12, 6);
	kelp_grab = build_servo(KELP_PORT, 300, 1810, 10, 6);
	wait_servo(kelp_grab, GRAB_HOLD);
	start_process(lateral_shift);
	wait_servo(kelp_arm, KELP_SCORE);
	wait_servo(kelp_grab, GRAB_SCORE);
	msleep(500L);
}
void grab_single()
{
	servo kelp_arm = build_servo(GRAB_PORT, 440, 1650, 15, 5);
	servo kelp_grab = build_servo(KELP_PORT, 300, 1810, 15, 5);
	wait_servo(kelp_arm, KELP_TOP);
	wait_servo(kelp_grab, GRAB_DOWN);
	create_accel_straight(0, -ACCEL_MAX + 60.0, -70.0);
	create_stop();
	kelp_arm = build_servo(GRAB_PORT, 440, 1650, 12, 6);
	kelp_grab = build_servo(KELP_PORT, 300, 1810, 10, 6);
	wait_servo(kelp_grab, GRAB_HOLD);
	wait_servo(kelp_arm, KELP_SCORE);
	wait_servo(kelp_grab, GRAB_SCORE);
	msleep(500L);
}
void align_twall()
{
	create_straight(-LOW_SPEED);
	msleep(1000L);
	create_cease();
	create_drive_segment(LOW_SPEED, 33);
	create_cease();
}
int main()
{
	create_connect();
	init_servo();


	//light_it_up(LIGHT_PORT);
	lightstart(LIGHT_PORT,120.0);

	create_full();
	create_drive_segment(HIGH_SPEED, -150);
	start_process(set_top);
	create_drive_arc(HIGH_SPEED, -200, 155);
	create_drive_segment(HIGH_SPEED, -365);
	create_drive_arc(HIGH_SPEED, 90, -64);
	create_drive_segment(HIGH_SPEED, -85);
	create_cease();
	set_top();
	align_twall();
	grab_top_de();
	create_spin_angle(400,-165);
	create_drive_segment(HIGH_SPEED, -320);
	//align_wall();
	create_stop();
	create_sync();
	dump_kelp_de();
	create_drive_segment(HIGH_SPEED,30);//go away from starting box
	create_spin_angle(HIGH_SPEED, -78);//turn to gate part 1
	create_drive_segment(HIGH_SPEED, 938);//go to gate
	create_drive_arc(HIGH_SPEED,185,84);//arc to gate
	//create_drive_time(HIGH_SPEED,1800);//go through the gate
	/*
	create_drive_segment(HIGH_SPEED,1470);
	create_drive_segment(HIGH_SPEED,-100);
	create_spin_angle(HIGH_SPEED,78);
	*/
	create_cease();
	create_drive_straight(HIGH_SPEED);//go onto oppponent's side of board
	msleep(1400);
	create_stop();
	create_arc(350,550);//arc to the opponent's mpa
	msleep(2500L);
	create_stop();//pause to keep the creat
	create_drive_straight(HIGH_SPEED);//go onto oppponent's side of board
	msleep(750);
	create_stop();
	create_drive_straight(-HIGH_SPEED);//go onto oppponent's side of board
	msleep(5000);
	create_stop();
	/*
	create_drive_segment(500,-500);//go past the IC
	create_drive_arc(320,-250,25);//arc to
	create_drive_arc(320,250,-8);
	*/
	//create_drive_segment(HIGH_SPEED,-750);
	//create_drive_arc(HIGH_SPEED,100,84);
	//create_drive_segment(HIGH_SPEED,1300);
}
