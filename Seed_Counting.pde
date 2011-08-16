#include <Stepper_ac.h>
#include <avr/pgmspace.h>

#define version_prog "TEST V2.1.1"

/********************************************
**  Name: Seed Counter 
**  Created: 27/06/2011
*********************************************
**  REVISIONS
**
**  V2.1.1 - 11/08/2011
**  * Added control of the axis limits in manual mode to avoid going to far and damaging the sensors or going off limits.
**  * Added Manual mode for the blisters dispenser
**  * Changed to the new library version 0.6 "Stepper_ac06.h"
**
**  V2.1.0 - 10/08/2011
**  * Merging code from previous motor testings
**  * Created new set of functions for controlling blister's motors "blisters.pde"
**  * Implemented error checking system in XY motors init algorithm. Depured code.
**  * Added velocity control of the motors. MS1,MS2 conencted from Arduino to the "easy driver" (functions included in library "Stepper_ac05.h")
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
**  * Implemented PROGMEM to store data of the matrix into flash memroy (on hold waiting for new HW prototype version)
**  * Added serial debugging
**
*********************************************


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
// ** Physical limits
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


/////////////////////////////////////////////////
void setup() {

  init_serial();
  Serial.println("Check the seed counter for any blister maygh be left in the conveier");
  Serial.println("When ready press button 1 to start set-up process");
 
 // Press button 1 to start
  press1_to_continue();
  Serial.println("SETTING UP");
  
  //Configure 3 Input Buttons
  pinMode (button1, INPUT);
  pinMode (button2, INPUT);
  pinMode (button3, INPUT);
  
  // Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
  speed_cntr_Init_Timer1();
  delay (10);  // Delay to be safe
  
  // Init XY axis
  Serial.print("Initializing XY Axes: ");
  if (XYaxes_init()) {       // Initiates X and Y axes
	print_ok();
  }else{
	print_fail();
  }
  
  // Init Counter
  Serial.print("Initializing Seed counter roll: ");
  if (Seedcounter_init()) {  // Initiates seed counters
	print_ok();
  }else{
	print_fail();
	
	//////////////// REWORK this piece of code
	//do a pause
	boolean pause = true;
	Serial.println("*** error in the seed counter roll, please check the vacuum engine, check the sensor cable is connected correctly");
	Serial.println("*** Press button 2 to try again");
	while (pause) {
	  //Chek if we press the start button
	  if (digitalRead(button2) == HIGH) {
		Serial.println("*** trying again...");
		pause = false;   // If we do, unpause
		count = 0;
	  }
	}	
	//////////////////////////////////
  }
  
  //Init blister dispenser
  Serial.println("Initializing blister dispenser: ");
  if (blisters_init ()) {
	print_ok();
  }else{
	print_fail();
  }
  
  // Mem_check
  mem_check ();
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************

void loop() {
  Serial.println("Ready! Press button 1 to start");
  // Press button 1 to start
  press1_to_continue();
  
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
 
}


void release_blister () {
	// motors to position up
	delay (1000);
	// motors to position hold
	motor_select = 2;
	speed_cntr_Move(-400,15000,16000,16000);
}





