#include <Stepper_ac.h>
#include <avr/pgmspace.h>

#define version_prog "TEST V2.1.2"

/********************************************
**  Name: Seed Counter 
**  Created: 27/06/2011
*********************************************
**  REVISIONS
**
**  V2.1.2 - 17/08/2011
**  * Added "press_button_to_continue(int button_number)" and "return_pressed_button ()" in extra fucntions
**  * Fixed bugs in XY axis init process.
**  * Added main menu with function show sensor status
**  * Implemented counter init procedure
**  * Added a full error control sistem in init procedures
**
**  V2.1.1 - 11/08/2011
**  * Added control of the axis limits in manual mode to avoid going to far and damaging the sensors 
**    or going off limits.
**  * Added Manual mode for the blisters dispenser
**  * Changed to the new library version 0.6 "Stepper_ac06.h"
**
**  V2.1.0 - 10/08/2011
**  * Merging code from previous motor testings
**  * Created new set of functions for controlling blister's motors "blisters.pde"
**  * Implemented error checking system in XY motors init algorithm. Depured code.
**  * Added velocity control of the motors. MS1,MS2 conencted from Arduino to the "easy driver" 
**    (functions included in library "Stepper_ac05.h")
**  *
**  V2.0 - 27/06/2011
**  * Changed hardware, new motors, new functionality, complete code reset
**  * Testing New motors
**
**  V1.4 - 31/05/2011
**  * Implement 2 holes pick-up seed
**  * Improved INIT sequence of the counter
**  * Check free memory during program
**  * removed first time drop function (instead included in motor_INIT function)
**
**  V1.3 - 13/05/2011
**  * Included version 0.5 of library
**  * Improved steps counting (in library "Stepper_ac05.h")
**  * Code readability improved
**  * Implemented PROGMEM to store data of the matrix into flash memroy (on hold waiting for 
**    new HW prototype version)
**  * Added serial debugging
**
*********************************************
*/

// ***********************
// ** DEFINES PIN MAP
// ***********************
#define button1 23
#define button2 25
#define button3 27

#define stepA 8
#define stepB 10
#define stepC 6
#define stepD 52
#define dirA 9
#define dirB 11
#define dirC 5
#define dirD 53
#define sensA 12
#define sensB 13
#define sensC "not_used"
#define sensD 7

#define ms1A 43
#define ms2A 42
#define ms1B 41
#define ms2B 40
#define ms1C 44
#define ms2C 45
#define ms1D 51
#define ms2D 50

// ***********************
// ** Physical limits of the motors
// ***********************

#define Xaxis_cycles_limit 280
#define Yaxis_cycles_limit 12
#define blisters_steps_limit 300    
#define blisters_steps_absoulut_limit 1000

// ***********************
// ** CONFIG MOTOR PINS
// ***********************
// Setting up motor A, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac Xaxis(stepA,dirA,sensA,ms1A,ms2A,200,2);   
// Setting up motor B, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac Yaxis(stepB,dirB,sensB,ms1B,ms2B,200,4);   
// Setting up motor C, step pin, direction pin, NO sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac blisters(stepC,dirC,0,ms1C,ms2C,200,4);
// Setting up motor D, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac counter(stepD,dirD,sensD,ms1D,ms2D,200,4);


// ***********************
// ** DEFINES variables
// ***********************
int a = 0;
long old_xpos=0;
long old_ypos=0;
int motor_select=0;
int situation=0;
int motor_speed=580;


// ***********************
// ** Error FLAGS
// ***********************
boolean error_XY = false;
boolean error_counter = false;
boolean error_blister = false;
#define ALL 0


/////////////////////////////////////////////////
void setup() {

  init_serial();
  enter_main_menu();
  
  Serial.println("SETTING UP");
  
  //Configure 3 Input Buttons
  pinMode (button1, INPUT);
  pinMode (button2, INPUT);
  pinMode (button3, INPUT);
  
  // Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
  speed_cntr_Init_Timer1();
  delay (10);  // Delay to be safe
  
  
  int temp_err = 0;   // flag for found errors 
  // INIT SISTEM, and CHECK for ERRORS
  if (!init_blocks(ALL)) temp_err = 1;  

  while (temp_err > 0) { // We found an error  
      temp_err = 0;
      Serial.println("\nErrors found, press 1 when ready to check again");
      // Press button 1 to start
      press_button_to_continue (1);
      
      if (error_XY) {
        if (!init_blocks(1)) temp_err++;
      }
      if (error_counter) {
        if (!init_blocks(2)) temp_err++;
      }
      if (error_blister) {
        if (!init_blocks(3)) temp_err++;
      }
  }
  Serial.println("\nReady! Press button 1 to start");
  // Press button 1 to start
  press_button_to_continue (1);
  // END of setup
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************

void loop() {
  Serial.println(" READY ************************************************* ");
  // Init Counter
  Serial.print("Initializing Seed counter roll: ");
  if (Seedcounter_init()) {  // Initiates seed counters
	print_ok();
  }else{
	print_fail();
        // error = 2;
  }
  
  press_button_to_continue (1);
  
  /*
  // Start program
  release_blister ();  //- motors C half turn and back
  
  go_to_position (80,30,10,30)  // example position (xcycles, xsteps, ycycles, ysteps)

  get 
- seedcounter motor D turn
- motor B turn (+8 turns)
- seedcounter motor D turn
- motor B turn
- seedcounter motor D turn
- motor B turn (-8 turns)
- seedcounter motor D turn
- motor B turn
- seedcounter motor D turn
- motor B turn (+8 turns)
- seedcounter motor D turn
- motor B turn
- seedcounter motor D turn
- motor B turn (-8 turns)
- seedcounter motor D turn
- motor A turn
- seedcounter motor D turn
- motor B turn (+8 turns)
- seedcounter motor D turn
- motor B turn (-8 turns)

- motor A turn until blister is under printer
- print lable on blister
- motor A turn unti blister drops
  
  
  pickup_seed_extended ();
  */
 
}






