
#ifndef Stepper_ac_h
#define stepper_ac_h

#include "WProgram.h"


								   
class Stepper_ac
{
  public:
    // functions
    Stepper_ac(int step_pin, int direction_pin);
    void set_init_position();
    void do_step();
	int get_position();
	boolean get_direction();
	void set_direction(boolean direction);
	void move_step(unsigned char pos);
	// VARs
	int stepPosition;
	unsigned char Easy_steptab[];

  private:
    int _step_pin;
    int _direction_pin;
	//! Position of stepper motor (relative to starting position as zero)
	int _stepPosition;
	boolean _direction;
};

#endif


