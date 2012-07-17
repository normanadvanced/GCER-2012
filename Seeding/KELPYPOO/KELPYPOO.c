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
#define GRAB_HOLD 1170
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
void grab_top()
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
	start_process(lateral_shift);
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
void align_wall()
{
	create_straight(-LOW_SPEED);
	msleep(3000L);
	create_drive_segment(LOW_SPEED + 75, 180);
	create_cease();
}
void grab_botguy()
{}
int main()
{
	create_connect();
	init_servo();

	//light_it_up(LIGHT_PORT);
	lightstart(LIGHT_PORT,120.0);
	msleep(7000L);
	create_drive_segment(MID_SPEED, 1000);
	create_drive_arc(MID_SPEED, 400, 88);
	create_drive_arc(MID_SPEED, -400, -88);
	create_spin_angle(MID_SPEED, 175);
	create_cease();
	create_straight(-LOW_SPEED);
	msleep(1500L);
	create_drive_segment(MID_SPEED, 240);
	create_spin_angle(LOW_SPEED, 86);
	create_cease();

	align_wall();
	grab_top();
	grab_bottom();
	grab_single();

	create_drive_arc(MID_SPEED, -250, -88);
	create_drive_arc(MID_SPEED, 300,45);
	create_drive_arc(MID_SPEED,-300,-45);
	create_drive_segment(MID_SPEED, 100);
	create_drive_arc(MID_SPEED,-400,-45);
	create_drive_segment(MID_SPEED, 200);
	create_drive_arc(MID_SPEED, 400,45);
	create_drive_segment(200, 230);
	create_spin_angle(LOW_SPEED, -86);
	create_cease();
	create_drive_segment(200, -200);
	create_cease();
	create_cease();

	dump_kelp();

	create_drive_arc(LOW_SPEED, -180, -88);
	create_cease();
	create_straight(-LOW_SPEED);
	msleep(3000L);
	create_cease();
	create_drive_segment(LOW_SPEED, 15);
	create_drive_arc(LOW_SPEED, -60, -86);
	create_drive_segment(LOW_SPEED, -50);
	create_cease();

	align_wall();
	grab_top();
	grab_bottom();
	grab_single();

	create_drive_arc(MID_SPEED, -250, -88);
	create_drive_segment(LOW_SPEED, -20);
	create_spin_angle(LOW_SPEED, -88);
	create_cease();
	create_straight(-LOW_SPEED);
	msleep(1000L);
	create_cease();

	dump_kelp();
}

