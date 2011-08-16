#include "WProgram.h"
#include "Stepper_ac04.h"				   		   

// SETUP	
//int step_pin -- Pin where the step control is connected
//int direction_pin -- Pin where the direction control is connected
//int sensor_pin -- Pin where the sensor output is connected (if any)
//unsigned long limit_steps -- Total of steps per cycle, when the limit is reached the 
//                          -- counter resets itself (if 0, no limit will be aplied)							  
Stepper_ac::Stepper_ac(int step_pin, int direction_pin, int sensor_pin, unsigned long limit_steps )
{
  pinMode(step_pin, OUTPUT);
  pinMode(direction_pin, OUTPUT);
  pinMode(sensor_pin, INPUT);
  _step_pin = step_pin;
  _direction_pin = direction_pin;
  _sensor_pin = sensor_pin;
  _limit_steps = limit_steps;
  // Set position of the motor to 0;
  stepPosition = 0;
  _stepPosition = 0;

}

void Stepper_ac::set_init_position()
{
  // Set position of the motor to 0;
  _stepPosition = 0;
}

// This order returns the postion of the motor
int Stepper_ac::get_position()
{
	stepPosition = _stepPosition;
	return stepPosition;
}

// Sets direction of the motor movement
void Stepper_ac::set_direction(bool direction)
{
  _direction=direction;
  if(_direction){
	digitalWrite (_direction_pin, HIGH);
  }
  else{
    digitalWrite (_direction_pin, LOW);
  }	
}

// Reads the actual direction of the motor
bool Stepper_ac::get_direction()
{
	return digitalRead(_direction_pin);
}

// Order to make the motor move one step (with coun ting steps enabled)
void Stepper_ac::do_step()
{
  // Just as information, variable _stepPosition doesn't do anything in the code
  // It is just a mere information of position (in steps) to know where the motor is
  // And because its a private variable, each instance of motor has its own position
  static unsigned char counter = 0;
  // Update position
  if(_direction){
    _stepPosition++;
  }
  else{
    _stepPosition--;
  }
  
  // If we reach the limit of steps we reset the counter
  if (_stepPosition == _limit_steps)  // we chek the upper limit (if there was no limit (0) nothing will happen
  {
	_stepPosition = 0;
  }else if ((_stepPosition < 0) && (_limit_steps != 0)) {  // we chek the lower limit, -1 (
	_stepPosition = _limit_steps - 1;
  }

  // Change the counter in order to add steps at it
  // like using a full 4 pins driver it would need 
  // at least 4 steps, or 8 for doubled steps
  // Since we are using easy driver we need to count 0 to 1 only (true false, just a normal pulse)
  //counter++;
  // Stay within the steptab
  //counter &= B00000001; //Force count 0 to 1 because we have only 2 steps
  move_step();
}

int theDelay = 1;

// This order will move the motor one step in the designed direction but without updating the counter
void Stepper_ac::move_step()
{
  //Using arduino code might take too long to put the pin on and off  (OLD approach)
  //digitalWrite (_step_pin, Easy_steptab[pos]);
  
  // This is where we send to the easy driver a pulse with the right Width of Minimum 1.0 µs
  digitalWrite (_step_pin, HIGH);
  delayMicroseconds(theDelay);   // This is a trick to generate exactly 1uS delay in Arduino
  digitalWrite (_step_pin, LOW);
   
  // Output the fast way
  //SM_PORT |= ((temp<<4)&0xF0);
  //SM_PORT &= ((temp<<4)|0x0F);
}

// function to find the initial position of a motor thru the sensor
void Stepper_ac::initiate()
{
	set_direction (false);   // Goes back till we find the sensor
	while (sensor_check())
	{
		do_step();
	}
	set_init_position();
}

// Checks whether the sensor atached to the motor detects something
bool Stepper_ac::sensor_check()
{
	//bool a = false;
	//a = digitalRead(_sensor_pin);
	// Because its a barrier sensor the pin its inverted, so we have 0 when we have a sensor
	// hence the "!" in front of the digitalRead, so we invert the signal ** RECHEK!!!
	return (!digitalRead(_sensor_pin));
}

void Stepper_ac::wait_till_reach_position(long position) 
{
	while (get_position() != position) {
        // wait doing nothing until we arrive to the deposit
    } 
}
