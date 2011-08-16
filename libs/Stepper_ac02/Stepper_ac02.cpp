#include "WProgram.h"
#include "Stepper_ac02.h"


//! Table with control signals for stepper motor (in case of 4 pins control)
//__flash  (nivestigate how to record this on the memory not in the program
/*
unsigned char steptab[] = {(1<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (0<<BIT_B2),
						   (1<<BIT_A1) | (0<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2),
					       (0<<BIT_A1) | (0<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2),
					       (0<<BIT_A1) | (1<<BIT_A2) | (1<<BIT_B1) | (0<<BIT_B2),
					       (0<<BIT_A1) | (1<<BIT_A2) | (0<<BIT_B1) | (0<<BIT_B2),
					       (0<<BIT_A1) | (1<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2),
					       (0<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2),
					       (1<<BIT_A1) | (0<<BIT_A2) | (0<<BIT_B1) | (1<<BIT_B2)}; */
// for easy driver
unsigned char Easy_steptab[] = {(1),(0)};						   
								   

// SETUP								  
Stepper_ac::Stepper_ac(int step_pin, int direction_pin)
{
  pinMode(step_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);
  _step_pin = step_pin;
  _direction_pin = direction_pin;
  // Set position of the motor to 0;
  stepPosition = 0;
  _stepPosition = 0;

}

void Stepper_ac::set_init_position()
{
  // Set position of the motor to 0;
  _stepPosition = 0;
}

int Stepper_ac::get_position()
{
	stepPosition = _stepPosition;
	return stepPosition;
}

boolean Stepper_ac::get_direction()
{
	return digitalRead(_direction_pin);
}

void Stepper_ac::set_direction(boolean direction)
{
  _direction=direction;
  if(_direction){
	digitalWrite (_direction_pin, HIGH);
  }
  else{
    digitalWrite (_direction_pin, LOW);
  }	
}

void Stepper_ac::do_step()
{
  // Just as information, stepPosition doesnt do anything in the code
  // It is just a mere information of position (in steps) to know where the motor is
  static unsigned char counter = 0;
  // Update position
  if(_direction){
    _stepPosition--;
  }
  else{
    _stepPosition++;
  }

  counter++;
  // Stay within the steptab
  counter &= B00000001; //Force count 0 to 1
  move_step(counter);
  //return(counter);
}

void Stepper_ac::move_step(unsigned char pos)
{
  //Using arduino code might take too long to put the pin on and off
	digitalWrite (_step_pin, Easy_steptab[pos]);
	
  // Output the fast way
  //SM_PORT |= ((temp<<4)&0xF0);
  //SM_PORT &= ((temp<<4)|0x0F);
}