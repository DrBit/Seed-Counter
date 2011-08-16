#include <Stepper_ac05.h>
#include <avr/pgmspace.h>

#define version_prog "TEST V1.4"

/********************************************
**  Name: Seed Counter 
**  Version: V1.3
**  Last Updated: 13/05/2011
*********************************************
**  REVISIONS
**  V1.4 - 31/05/2011
**  * Implement 2 holes pick-up seed
**  * Improved INIT seuquence of the counter
**  * Check free memory during program
**  * removed first time drop function (instead prepare motor in INIT function)
**
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
PROGMEM  prog_uint16_t x_axis_set[]  = { 2,14,43,800,85,153,125,181,165,717,206,67,247,150};
PROGMEM  prog_uint16_t y_axis_set[]  = { 1,1,38,465,57,567};
// read back a 2-byte int
// pgm_read_word_near(charSet + N)
// cycle of blister N = N + (N - 1)
// step position of blister N = N + N


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
int motor_speed=580;


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
  // Mem_check
  mem_check ();
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
    }
  }  
  
  
  pickup_seed_extended ();

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


// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
void pickup_seed_extended () {
  motor_select = 0;
  boolean seed_detected = false;
  // IMLEMENT BOTH HOLES of the vacuum hole ... see below
  Serial.print (Seedcounter1.get_steps_cycles());
  Serial.print (" - ");
  Serial.println (Seedcounter1.get_steps());
  while (!seed_detected) {
    if ((Seedcounter1.get_steps() == 1200) || (Seedcounter1.get_steps() == 400)  || (Seedcounter1.get_steps() == -400)) {
      delay (150);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
      speed_cntr_Move(-800,5500,19000,19000);  // We do a half turn, NOTICE that the acceleration in this case is lower
                                                // Thats to avoid trowing seeds and to achieve a better grip on the seed
    }
    if ((Seedcounter1.get_steps() == 1600) || (Seedcounter1.get_steps() == 800) || (Seedcounter1.get_steps() == 0)) {   // We chek the sensor only when we are in the range of the sensor
      if (Seedcounter1.sensor_check()){                 // We got a seed!!!
          seed_detected = true; 
          while (!(Seedcounter1.get_steps() == 1200) && !(Seedcounter1.get_steps() == 400))
          {
            //delay (100); // Wait for the seed to fall
          }
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
  delay (5550);
}


void go_position2 () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(6500,15000,16000,16000);
  delay (3550);
}

void go_positionRow () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(8800,15000,16000,16000);
  delay (3500);
}

void go_position3 () { //arne comment: go to next position on x axle backwards 
  motor_select = 1;
  speed_cntr_Move(-6530,19000,20000,20000);
  delay (2550); 
}




int get_cycle_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*N) - 2)
}

int get_step_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*N) - 1)
}

int get_cycle_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*N) - 2)
}

int get_step_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*N) - 1)
}

void go_to_position (int position_to_go) {
  int X_cycles_to_move = Xaxis.get_steps_cycles() - get_cycle_Xpos(position_to_go);
  int X_steps_to_move = Xaxis.get_steps() - get_step_Xpos(position_to_go);
  int Y_cycles_to_move = Yaxis.get_steps_cycles() - get_cycle_Ypos(position_to_go);
  int Y_steps_to_move = Yaxis.get_steps() - get_step_Ypos(position_to_go);
  
  

}


// cycle = 1600 steps
//  

