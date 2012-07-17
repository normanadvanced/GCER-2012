// Added by Jeremy Rand / Biolizard89
long sim_motor_velocity0, sim_motor_velocity1, sim_motor_velocity2, sim_motor_velocity3;
long sim_motor_position0, sim_motor_position1, sim_motor_position2, sim_motor_position3;
int sim_motor_has_goal0, sim_motor_has_goal1, sim_motor_has_goal2, sim_motor_has_goal3;
long sim_motor_goal0, sim_motor_goal1, sim_motor_goal2, sim_motor_goal3;
int sim_backemf_process, sim_backemf_running;

void sim_backemf();
long *sim_motor_position_pointer(int motor_number);
long *sim_motor_goal_pointer(int motor_number);
int *sim_motor_has_goal_pointer(int motor_number);
long *sim_motor_velocity_pointer(int motor_number);
long rcservotraj_get_motor_position_counter(int motor);
void rcservotraj_set_motor_position_counter(int motor, long value);
void rcservotraj_clear_motor_position_counter(int motor);
void rcservotraj_move_at_velocity(int motor_number, long velocity);
void rcservotraj_move_to_position(int motor_number, long velocity, long goal_pos);
void rcservotraj_move_relative_position(int motor, long velocity, long delta_pos);
void rcservotraj_mrp(int motor, long velocity, long delta_pos);
void rcservotraj_block_motor_done(int motor);
void rcservotraj_bmd(int motor);
void rcservotraj_mav(int motor, long velocity);
void rcservotraj_mtp(int motor, long velocity, long position);
int rcservotraj_get_motor_done(int motor);
void rcservotraj_mtp_instant(int motor, long position);

#define servo_get_motor_position_counter rcservotraj_get_motor_position_counter
#define servo_set_motor_position_counter rcservotraj_set_motor_position_counter
#define servo_clear_motor_position_counter rcservotraj_clear_motor_position_counter
#define servo_move_at_velocity rcservotraj_move_at_velocity
#define servo_move_to_position rcservotraj_move_to_position
#define servo_move_relative_position rcservotraj_move_relative_position
#define servo_mrp rcservotraj_mrp
#define servo_block_motor_done rcservotraj_block_motor_done
#define servo_bmd rcservotraj_bmd
#define servo_mav rcservotraj_mav
#define servo_mtp rcservotraj_mtp
#define servo_get_motor_done rcservotraj_get_motor_done
#define servo_mtp_instant rcservotraj_mtp_instant
//long mseconds();
#define mseconds() ((long)(seconds()*1000.0))

void sim_backemf()
{
    // Added by Jeremy Rand / Biolizard89
    // Call as a separate process

    int active_motor;

    long init_mseconds[4];
    long init_position[4];
    long old_velocity[4] = {0, 0, 0, 0};
    int old_has_goal[4] = {0, 0, 0, 0};
    long old_goal[4] = {0L, 0L, 0L, 0L};

    sim_backemf_running = 1;

    while(1)
      {
        // For each motor
        for(active_motor = 0; active_motor <= 3; active_motor++)
          {
            // If velocity has changed
            // Should it also check for a goal change?  Yes!  Also a has_goal change!
            if(*sim_motor_velocity_pointer(active_motor) != old_velocity[active_motor] || *sim_motor_goal_pointer(active_motor) != old_goal[active_motor] || *sim_motor_has_goal_pointer(active_motor) != old_has_goal[active_motor] )
              {
                init_mseconds[active_motor] = mseconds();
                //init_position[active_motor] = get_motor_position_counter(active_motor);
                init_position[active_motor] = (long) get_servo_position(active_motor);
            }

            // Set old velocity and has_goal
            old_velocity[active_motor] = *sim_motor_velocity_pointer(active_motor);
            old_has_goal[active_motor] = *sim_motor_has_goal_pointer(active_motor);
            old_goal[active_motor] = *sim_motor_goal_pointer(active_motor);

            // If the motor has a goal position
            if(*sim_motor_has_goal_pointer(active_motor))
              {
                // If the motor's goal is to increase.
                if(*sim_motor_goal_pointer(active_motor) > init_position[active_motor] && *sim_motor_velocity_pointer(active_motor) > 0)
                  {

                    // Set the motor position
                    rcservotraj_set_motor_position_counter(active_motor, (long) ((float)(*sim_motor_velocity_pointer(active_motor)) * ((float)(mseconds() - init_mseconds[active_motor]) / 1000.0) + (float)init_position[active_motor]));
                    //motor(active_motor, /*(int)*/ (/*(float)*/(*sim_motor_velocity_pointer(active_motor)) * 100 / /*(float)*/MAX_VEL));

                    // Check if motor has reached goal
                    if(rcservotraj_get_motor_position_counter(active_motor) > *sim_motor_goal_pointer(active_motor))
                      {
                        printf("sim_backemf: m%d reached goal", active_motor); // for debug
                        *sim_motor_velocity_pointer(active_motor) = 0; // Velocity = 0
                        *sim_motor_has_goal_pointer(active_motor) = 0; // No longer a goal
                        *sim_motor_position_pointer(active_motor) = *sim_motor_goal_pointer(active_motor); // Position = goal
                    }

                }
                // If the motor's goal is to decrease.
                else if(*sim_motor_goal_pointer(active_motor) < init_position[active_motor] && *sim_motor_velocity_pointer(active_motor) > 0)
                  {
                    // Set the motor position
                    rcservotraj_set_motor_position_counter(active_motor, (long)(-1.0 * (float)(*sim_motor_velocity_pointer(active_motor)) * ((float)(mseconds() - init_mseconds[active_motor]) / 1000.0) + (float)init_position[active_motor]));
                    //motor(active_motor, -1 * /*(int)*/ (/*(float)*/(*sim_motor_velocity_pointer(active_motor)) * 100 / /*(float)*/MAX_VEL));

                    // Check if motor has reached goal
                    if(rcservotraj_get_motor_position_counter(active_motor) < *sim_motor_goal_pointer(active_motor))
                      {
                        printf("sim_backemf: m%d reached goal", active_motor); // for debug
                        *sim_motor_velocity_pointer(active_motor) = 0;
                        *sim_motor_has_goal_pointer(active_motor) = 0;
                        *sim_motor_position_pointer(active_motor) = *sim_motor_goal_pointer(active_motor);
                    }
                }
            }
            // If the motor has no goal
            else
              {
                // Set the motor position
                rcservotraj_set_motor_position_counter(active_motor, (long)( (float)(*sim_motor_velocity_pointer(active_motor)) * ((float)(mseconds() - init_mseconds[active_motor]) / 1000.0) + (float)init_position[active_motor]));
                //motor(active_motor, /*(int)*/ (/*(float)*/(*sim_motor_velocity_pointer(active_motor)) * 100 / /*(float)*/MAX_VEL));
            }

            if(rcservotraj_get_motor_position_counter(active_motor) < 0) rcservotraj_set_motor_position_counter(active_motor, 0);
            if(rcservotraj_get_motor_position_counter(active_motor) > 2047) rcservotraj_set_motor_position_counter(active_motor, 2047);
            set_servo_position(active_motor,(int) (rcservotraj_get_motor_position_counter(active_motor)));

        }

        //defer();
        msleep(10);
    }
}

long *sim_motor_position_pointer(int motor_number)
{
    // Added by Jeremy Rand / Biolizard89

    if(motor_number == 0)
      return(&sim_motor_position0);
    else if(motor_number == 1)
      return(&sim_motor_position1);
    else if(motor_number == 2)
      return(&sim_motor_position2);
    else if(motor_number == 3)
      return(&sim_motor_position3);
}

long *sim_motor_goal_pointer(int motor_number)
{
    // Added by Jeremy Rand / Biolizard89

    if(motor_number == 0)
      return(&sim_motor_goal0);
    else if(motor_number == 1)
      return(&sim_motor_goal1);
    else if(motor_number == 2)
      return(&sim_motor_goal2);
    else if(motor_number == 3)
      return(&sim_motor_goal3);
}

int *sim_motor_has_goal_pointer(int motor_number)
{
    // Added by Jeremy Rand / Biolizard89

    if(motor_number == 0)
      return(&sim_motor_has_goal0);
    else if(motor_number == 1)
      return(&sim_motor_has_goal1);
    else if(motor_number == 2)
      return(&sim_motor_has_goal2);
    else if(motor_number == 3)
      return(&sim_motor_has_goal3);
}

long *sim_motor_velocity_pointer(int motor_number)
{
    // Added by Jeremy Rand / Biolizard89

    if(motor_number == 0)
      return(&sim_motor_velocity0);
    else if(motor_number == 1)
      return(&sim_motor_velocity1);
    else if(motor_number == 2)
      return(&sim_motor_velocity2);
    else if(motor_number == 3)
      return(&sim_motor_velocity3);
}

long rcservotraj_get_motor_position_counter(int motor)
{
    // Replaced by Jeremy Rand / Biolizard89

    return(*sim_motor_position_pointer(motor));
}

void rcservotraj_set_motor_position_counter(int motor, long value)
{
    // Replaced by Jeremy Rand / Biolizard89

    *sim_motor_position_pointer(motor) = value;

    // Show position in simulator motor field; currently disabled due to 100 max value there.
    //setpwm(motor+4, (int)value);
}

void rcservotraj_clear_motor_position_counter(int motor)
{
    // Replaced by Jeremy Rand / Biolizard89

    *sim_motor_position_pointer(motor) = 0L;
}

void rcservotraj_move_at_velocity(int motor_number, long velocity)
{
    // Replaced by Jeremy Rand / Biolizard89

    *sim_motor_velocity_pointer(motor_number) = velocity;
    *sim_motor_has_goal_pointer(motor_number) = 0;
    //motor(motor_number, (int) ((float)velocity * 100.0 / (float)MAX_VEL)); // Should change this to just change the value shown in simulator

    if(! sim_backemf_running)
      {
        sim_backemf_process = start_process(sim_backemf);
        sim_backemf_running = 1;
    }
}


// Move motor to goal_pos at given velocity.  The amount actually
// moved will be goal_pos - get_motor_position().
void rcservotraj_move_to_position(int motor_number, long velocity, long goal_pos)
{
    // Replaced by Jeremy Rand / Biolizard89

    *sim_motor_velocity_pointer(motor_number) = velocity;
    *sim_motor_has_goal_pointer(motor_number) = 1;
    *sim_motor_goal_pointer(motor_number) = goal_pos;
    //motor(motor_number, (int) ((float)velocity * 100.0 / (float)MAX_VEL)); // Should change this to just change the value shown in simulator

    if(! sim_backemf_running)
      {
        sim_backemf_process = start_process(sim_backemf);
        sim_backemf_running = 1;
    }

    //defer(); // Attempts to fix problem with multiple moves in quick succession; didn't seem to be necessary
}

// Move delta_pos relative to the current position of the motor.  The
// final motor tick will be get_motor_position() at the time of the
// call + delta_pos.
void rcservotraj_move_relative_position(int motor, long velocity, long delta_pos)
{
    // Replaced by Jeremy Rand / Biolizard89

    rcservotraj_move_to_position(motor, velocity, get_motor_position_counter(motor) + delta_pos);
}

void rcservotraj_mrp(int motor, long velocity, long delta_pos)
{
    // Replaced by Jeremy Rand / Biolizard89

    rcservotraj_move_relative_position(motor, velocity, delta_pos);
}

void rcservotraj_block_motor_done(int motor)
{
    // Replaced by Jeremy Rand / Biolizard89

    while(! rcservotraj_get_motor_done(motor))
      msleep(10);
}


void rcservotraj_bmd(int motor)
{
    rcservotraj_block_motor_done(motor);
}

void rcservotraj_mav(int motor, long velocity)
{
    rcservotraj_move_at_velocity(motor, velocity);
}

void rcservotraj_mtp(int motor, long velocity, long position)
{
    // Replaced by Jeremy Rand / Biolizard89

    rcservotraj_move_to_position(motor, velocity, position);
}

int rcservotraj_get_motor_done(int motor)
{
    // Replaced by Jeremy Rand / Biolizard89

    if(*sim_motor_position_pointer(motor) == *sim_motor_goal_pointer(motor) /*&& ! *sim_motor_has_goal_pointer(motor)*/)
      {
        return(1);
    }
    else
      {
        return(0);
    }
}

void rcservotraj_mtp_instant(int motor, long position)
{
	set_servo_position(motor, position-1);
	rcservotraj_mtp(motor, 2000000000, position);
}
/*
long mseconds()
{
	return(seconds() * 1000);
}
*/
