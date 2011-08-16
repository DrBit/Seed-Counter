/*
  Stepper_ac.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#ifndef Stepper_ac_h
#define stepper_ac_h

#include "WProgram.h"

class Stepper_ac
{
  public:
    Stepper_setup(int step_pin, int direction_pin);
    void direction();
    void step();
  private:
    int _step_pin;
    int _direction_pin;
};

#endif

