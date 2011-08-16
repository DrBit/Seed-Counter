
#ifndef Stepper_ac_h
#define stepper_ac_h

#include "WProgram.h"


								   
class Stepper_ac
{
  public:
    // functions
    Stepper_ac(int step_pin, int direction_pin, int sensor_pin, unsigned long limit_steps);
    void set_init_position();
    void do_step();
	int get_position();
	bool get_direction();
	void set_direction(bool direction);
	void move_step();
	void initiate();
	bool sensor_check();
	void wait_till_reach_position(long position);
	// VARs
	long stepPosition;
	unsigned char Easy_steptab[];

  private:
    int _step_pin;
    int _direction_pin;
	int _sensor_pin;
	unsigned long _limit_steps;		//! Limit of the steps counter (When reaching this limit the counter will reset
	long _stepPosition;				//! Position of stepper motor (relative to starting position as zero)
	bool _direction;			
};

#endif


