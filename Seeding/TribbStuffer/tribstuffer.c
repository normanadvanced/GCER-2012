// Created on Thu Jul 5 2012

#include <stdio.h>
#include "opencode.h"

#define BELT_PORT 1
#define BELT_SPEED 1000
#define STUFF_PORT 2
#define STUFFING 970
#define STUFF_SPEED 900

#define LOW_SPEED 300
#define MID_SPEED 550
#define HIGH_SPEED 900
void stuff_tribbles()
{
	while(1)
	{
		while(analog10(2) < 900)
		{
			mav(2, -STUFF_SPEED);
			msleep(10L);
		}
		off(2);
		mrp(2, 600, 1000L);
		bmd(2);
	}
}
void stuff_tribbles_hard()
{
	while(1)
	{
		clear_motor_position_counter(2);
		while(analog10(2) < 900 && get_motor_position_counter(2) > -1050)
		{
			mav(2, -STUFF_SPEED);
			msleep(10L);
		}
		off(2);
		mrp(2, 600, 1130L);
		bmd(2);
	}
}
void stuff_once()
{
	clear_motor_position_counter(2);
	while(analog10(2) < 900 && get_motor_position_counter(2) > -1050)
	{
		mav(2, -STUFF_SPEED);
		msleep(10L);
	}
	off(2);
	mrp(2, 600, 1130L);
	bmd(2);
}
void eject_tribbles()
{
	mrp(2, 1000, -1500L);
	bmd(2);
}
void tribble_belt(int speed)
{
	mav(1, speed);
}
int main() 
{
	build_left_wheel(0, 1005, 0.97, 53.5, 78.0);
	build_right_wheel(3, 1040, 1.0, 53.5, 78.0);
	
	build_left_tophat(0, 250, 900, 100, 3000L);
	build_right_tophat(1, 250, 900, 100, 3000L);
	
	int lets_stuff; 
	
	lightstart(5, 120.0);
	
	cbc_direct(-MID_SPEED, -MID_SPEED);
	msleep(1500L);
	cbc_straight(MID_SPEED, 25.0);
	cbc_spin(HIGH_SPEED, -48.0); 
	cbc_straight(HIGH_SPEED, 1200.0);
	
	cbc_arc(MID_SPEED, 200.0, 45.0);
	tribble_belt(1000);
	
	cbc_straight(HIGH_SPEED, 150.0);
	
	cbc_direct(-MID_SPEED, -MID_SPEED);
	cbc_align_black();
	msleep(100L);
	cbc_straight(MID_SPEED, 150.0);
	lets_stuff = start_process(stuff_tribbles);
	cbc_arc(HIGH_SPEED - 130, -425.0, -50.0);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_spin(HIGH_SPEED, -30.0);
	cbc_spin(HIGH_SPEED, 30.0);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_arc(LOW_SPEED, -405.0, -10.0);
	cbc_halt();
	msleep(700L);
	cbc_spin(HIGH_SPEED, -30.0);
	cbc_spin(HIGH_SPEED, 30.0);
	cbc_arc(LOW_SPEED, 405.0, 10.0);
	cbc_halt();
	msleep(700L);
	cbc_arc(LOW_SPEED, 405.0, 10.0);
	cbc_halt();
	msleep(700L);
	cbc_spin(MID_SPEED, 170.0);
	cbc_wait();
	cbc_direct(-HIGH_SPEED, -HIGH_SPEED);
	msleep(3000L);
	cbc_arc(MID_SPEED, 200.0, 50.0);
	cbc_arc(MID_SPEED, -200.0, -50.0);
	cbc_halt();
	cbc_spin(MID_SPEED, -90.0);
	kill_process(lets_stuff);
	stuff_once();
	cbc_direct(-HIGH_SPEED, -HIGH_SPEED);
	msleep(3000L);
	cbc_straight(MID_SPEED, 30.0);
	cbc_spin(MID_SPEED, 90.0);
	lets_stuff = start_process(stuff_tribbles);
	cbc_straight(LOW_SPEED, 535.0);
	cbc_spin(MID_SPEED, 90.0);
	cbc_straight(MID_SPEED, -60.0);
	cbc_wait();
	cbc_direct(LOW_SPEED, LOW_SPEED);
	cbc_align_black();
	cbc_halt();
	cbc_straight(HIGH_SPEED, 100.0);
	kill_process(lets_stuff);
	lets_stuff = start_process(stuff_tribbles_hard);
	cbc_arc(MID_SPEED, 700.0, 70.0);
	cbc_halt();
	cbc_spin(MID_SPEED, 105.0);
	cbc_straight(MID_SPEED, -300.0);
	cbc_wait();
	kill_process(lets_stuff);
	stuff_once();
	cbc_direct(-MID_SPEED, -MID_SPEED);
	msleep(4000L);
	cbc_straight(MID_SPEED, 130.0);
	start_process(stuff_tribbles_hard);
	cbc_spin(MID_SPEED, 90.0);
	cbc_direct(-MID_SPEED, -MID_SPEED);
	msleep(4000L);
	cbc_halt();
	cbc_straight(1000, 10.0);
	cbc_straight(1000, -10.0);
	cbc_straight(1000, 10.0);
	cbc_straight(1000, -10.0);
	cbc_direct(-MID_SPEED, -MID_SPEED);
	msleep(1000L);
	kill_process(lets_stuff);
	stuff_once();
	eject_tribbles();
	return 0;
}
