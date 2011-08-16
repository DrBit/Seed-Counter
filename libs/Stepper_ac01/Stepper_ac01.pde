/*
  Stepper_ac.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.

There are many right ways to do it, but here is the way that I found to be the simplest. To turn a port high, use this command:
PORT{letter} |= _BV(P{letter}{number});
 
To turn a port low, use this command:
PORT{letter} &= ~_BV(P{letter}{number});
 
Replace {letter} and {number} with the corresponding pin letter and number. For example, for pin B5 with the LED, turning it high and then low would be:
PORTB |= _BV(PB5);
PORTB &= ~_BV(PB5);
*/




#include "WProgram.h"
#include "Stepper_ac01.h"

Stepper_ac::Stepper_setup(int step_pin, int direction_pin)
{
  pinMode(step_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);
  _step_pin = step_pin;
  _direction_pin = direction_pin;
}

void Stepper_ac::direction(boolean dir)
{
  digitalWrite(_direction_pin, dir);
}

void Stepper_ac::step()
{
  digitalWrite(_step_pin, !digitaRead(_step_pin));
}
