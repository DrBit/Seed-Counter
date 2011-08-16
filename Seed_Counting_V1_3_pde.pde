#include <Stepper_ac05.h>
#include <avr/pgmspace.h>

/********************************************
**  Name: Seed Counter 
**  Version: V1.3
**  Last Updated: 13/05/2011
*********************************************
**  REVISIONS
**  V1.3 - 13/05/2011
**  * Included version 0.5 of library
**  * Improved steps counting (library)
**  * Code readability improved
**  * Implemented PROGMEM to store data of the matrix into flash memroy (cancelled)
**  * Added serial debugging
*********************************************
*/

// ***********************
// ** DEFINE ARRAY DIMENSION FOR BLISTERS
// ***********************
// code description:
// PROGMEM  prog_uchar name_of_array[]
// prog_uchar is an unsigned char (1 byte) 0 to 255
PROGMEM  prog_uint16_t charSet[]  = { 65000, 32796, 16843, 10, 11234};


// ***********************
// ** CONFIG MOTOR PINS
// ***********************
// Setting up motor 1, step pin = 9, direction pin = 8 , sensor pin 6, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac Seedcounter1(9,8,6,200,8);   
// Setting up motor 2, step pin = 11, direction pin = 10 , sensor pin 4, 200 steps, 4 for Quarter step(mode of the stepper driver)
Stepper_ac Xaxis(11,10,4,200,4);   
// Setting up motor 3, step pin = 13, direction pin = 12 , sensor pin 5, 200 steps, 4 for Quarter step(mode of the stepper driver)
Stepper_ac Yaxis(13,12,5,200,4);

// ***********************
// ** DEFINES PINS EXTRA BUTTONS
// ***********************
#define button1 2
#define button2 3

// ***********************
// ** DEFINES variables
// ***********************
int a = 0;
long old_xpos=0;
long old_ypos=0;
int motor_select=0;
int situation=0;


/////////////////////////////////////////////////
void setup() {
  init_serial();

  //Configure 2 Input Buttons
  pinMode (button1, INPUT);
  pinMode (button2, INPUT);
  
  // Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
  speed_cntr_Init_Timer1();
  delay (10);  // Delay to be safe
  
  Serial.println("SETTING UP");
  // Reset all motors and find point 0
  Serial.println("XY Axes Initializing:");
  XYaxes_init();       // Initiates X and Y axes
  Serial.println("XY Axes OK");
  Serial.println("Seed counter roll Initializing:");
  Seedcounter_init();  // Initiates seed counters
  Serial.println("Seed counter roll OK");
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************

void loop() {
 
  // Press button 2 to start
  boolean start = false;
  while (!start)
  {
    if (digitalRead(button2) == HIGH) {
      start = true;
      // We chek if we got a seed, if we dont we init again
      if (!Seedcounter1.sensor_check()) {
        Seedcounter_init();
      }
    }
  }  
  
  // Start whith the process
  first_time_drop ();

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////// 1e blister //////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for (int x=0;x<34; x++) {  
      go_position1 (); //arne comment: go to  next x axle position
      pickup_seed_extended (); 
   } 
  // 2nd row
  go_position2 (); //arne comment: go to  next y axle position
  pickup_seed_extended ();  
  //////////////////////////

  for (int x=0;x<34; x++) { 
    go_position3 (); //arne comment: go to  next x axle position
    pickup_seed_extended ();  
  } 
  
  // 3nd row
  go_positionRow (); //arne comment: go to  next y axle position
  pickup_seed_extended ();  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
   
  XYaxes_init();       // Initiates X and Y axes
  delay (500);  
}


////////////////////////////////////////////////////////////
// This function comes back from the acceleration function with a proper timing to 
// do steps on the motors
// As we dont have any way of knowing which motor we are moving back from this function
// we preselect the motor (0,1,2) before doing the movement
/////////////////////////////////////////////////////////////
void sm_driver_StepCounter(unsigned char Mdirection) {
  if (motor_select == 0) {
    Seedcounter1.set_direction (Mdirection);
    Seedcounter1.do_step();
  }else if (motor_select == 1) {
    Xaxis.set_direction (!Mdirection);
    Xaxis.do_step();
  }else if (motor_select == 2) {
    Yaxis.set_direction (!Mdirection);
    Yaxis.do_step();
  }
}

// Especial function for the first time drop (as we used one seed to detect the 0 position and is now at the sensor position
void first_time_drop () {
  motor_select = 0;
  speed_cntr_Move(-375,20000,20000,20000);
  while (Seedcounter1.get_steps() != 375) {
    // wait doing nothing until we drop the seed
    delay (100); // This delay also waits for the seed to fall
  }
}

// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
void pickup_seed_extended () {
  motor_select = 0;
  boolean seed_detected = false;
  // IMLEMENT BOTH HOLES of the vacuum hole ... see below
  /*
  while (!seed_detected) {
    if (Seedcounter1.get_steps() == 375) {
      delay (100);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
      speed_cntr_Move(-1800,5500,19000,19000);  // We do a half turn, NOTICE that the acceleration in this case is lower
                                                // Thats to avoid trowing seeds and to achieve a better grip on the seed
    }
    if ((Seedcounter1.get_steps() >= 375) || (Seedcounter1.get_steps() <= 20)) {   // We chek the sensor only when we are in the range of the sensor
      if (Seedcounter1.sensor_check()){                 // We got a seed!!!
          seed_detected = true; 
          Seedcounter1.wait_till_reach_position(375);   // We wait until we are bak at the deposit position (400)
          delay (100); // Wait for the seed to fall
      }
    }
  }*/
  
  while (!seed_detected) {
    if (Seedcounter1.get_steps() == 375) {
      delay (100);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
      speed_cntr_Move(-800,5500,19000,19000);  // We do a half turn, NOTICE that the acceleration in this case is lower
                                                // Thats to avoid trowing seeds and to achieve a better grip on the seed
    }
    if ((Seedcounter1.get_steps() >= 375) || (Seedcounter1.get_steps() <= 20)) {   // We chek the sensor only when we are in the range of the sensor
      if (Seedcounter1.sensor_check()){                 // We got a seed!!!
          seed_detected = true; 
          Seedcounter1.wait_till_reach_position(375);   // We wait until we are bak at the deposit position (400)
          delay (100); // Wait for the seed to fall
      }
    }
  
  }
}

// ************************************************************
// ** POSITION FUNCTIONS
// ************************************************************
void go_position1 () { //arne comment: go to next position on x axle 
  motor_select = 1;
  speed_cntr_Move(6530,19000,20000,20000);
  delay (2150);
}


void go_position2 () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(6500,15000,16000,16000);
  delay (2150);
}

void go_positionRow () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(8800,15000,16000,16000);
  delay (3300);
}

void go_position3 () { //arne comment: go to next position on x axle backwards 
  motor_select = 1;
  speed_cntr_Move(-6530,19000,20000,20000);
  delay (2150); 
}




// ************************************************************
// ** EXTRA FUNCTIONS  (NOT USED)
// ************************************************************

void manual_modeXY() {
  
  Xaxis.set_direction (true);   // Goes back till we find the sensor
  Yaxis.set_direction (true);   // Goes back till we find the sensor

  if (digitalRead(button1) == HIGH) {
    Xaxis.do_step();
  }else{
     delayMicroseconds(18);    
  }
  
  if (digitalRead(button2) == HIGH) {
    Yaxis.do_step();
  }else{
     delayMicroseconds(18);
  }
  
  delayMicroseconds(390);
}


void manual_modeCounter() {
  //
  if (digitalRead(button1) == HIGH) {
    Seedcounter1.set_direction (true);   // Goes back till we find the sensor
    Seedcounter1.do_step();
  }
  if (digitalRead(button2) == HIGH) {
    Seedcounter1.set_direction (false);   // Goes back till we find the sensor
    Seedcounter1.do_step();
  }
  //delayMicroseconds(390);
}

