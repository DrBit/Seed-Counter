#include <Stepper_ac.h>
#include <avr/pgmspace.h>
#include <StopWatch.h>

#define version_prog "TEST V2.1.9 Plain Counter"
#define lib_version 11

/********************************************
**  Name: Seed Counter 
**  Created: 27/06/2011
*********************************************
**  Quim Llums
**
**  http://www.skmcreatiu.com
**  http://blog.drbit.nl
**  http://drbit.nl
**
*/

// #define DEBUG		// Remove / Add"//" to enable / disbale DEBUG mode

// example debug:
// #if defined DEBUG
// Serial.println(val);
// #endif

// ***********************
// ** DEFINES PIN MAP
// ***********************
#define button1 23
#define button2 25
#define button3 27
#define Vibration 13

#define stepA 8
#define stepB 10
#define stepC 6
#define stepD 52
#define dirA 9
#define dirB 11
#define dirC 5
#define dirD 53
#define sensA 12
#define sensB 4
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

// Stop Watch Timer
StopWatch MySW;


// ***********************
// ** DEFINES variables
// ***********************
int a = 0;
long old_xpos=0;
long old_ypos=0;
int motor_select=0;
int situation=0;
const int motor_speed_counter=900;
const int motor_speed_XY=450;
const int motor_speed_blisters=2500;

// ***********************
// ** Error FLAGS
// ***********************
boolean error_XY = true;
boolean error_counter = true;
boolean error_blister = true;
#define ALL 0

int count_limit = 200;
int updated_count_limit = count_limit;
int count_total_turns = 0;

/////////////////////////////////////////////////
void setup() {

	//*** BASIC SETUP
	
	// INIT Serial
	init_serial();	
	//Configure 3 Input Buttons
	pinMode (button1, INPUT);
	pinMode (button2, INPUT);
	pinMode (button3, INPUT);
	pinMode (Vibration, OUTPUT);
	
	analogWrite(Vibration, 224);
	delay (1000);
	analogWrite(Vibration, 0);
	
	// Check Version
	if ((Xaxis.get_version()) != lib_version) {
		Serial.println("Library version mismatch");
		Serial.print(" This code is designed to work with library V");
		Serial.println(lib_version);
		Serial.print(" And the library installed is version V");
		Serial.println(Xaxis.get_version());
		Serial.println (" Program stoped!");
		while (true) {}
	}
	
	// Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
	speed_cntr_Init_Timer1();
	delay (10);  // Delay to be safe

	//**** ENTER MAIN MENU
	enter_main_menu();
	
	//**** CONTINUE with normal process
	Serial.println("SETTING UP");
	// INIT SISTEM, and CHECK for ERRORS
	int temp_err = 0;   // flag for found errors
	//if (!init_blocks(ALL)) temp_err = 1;
	if (!init_blocks(3)) temp_err = 1;  // We only init the counter

	
	while (temp_err > 0) { // We found an error, we chek ALL errors and try to initiate correctly
		temp_err = 0;
		Serial.println("\nErrors found, press 1 when ready to check again, 2 to bypas the errors");
		switch (return_pressed_button ()) {
			//Init XY 
			case 1:
				/*if (error_XY) {
					if (!init_blocks(2)) temp_err++;
				}*/
				if (error_counter) {
					if (!init_blocks(3)) temp_err++;
				}
				/*if (error_blister) {
					if (!init_blocks(1)) temp_err++;
				}*/
			break;
			
			case 2:
				// do nothing so we wond detect any error and we will continue
			break;
		}
	}
	// Serial.println("\nReady! Press button 1 to start");
	// Press button 1 to start
	// press_button_to_continue (1);
	// END of setup
	
	
	Serial.println ("Starting simple counter, press any key to pause ");
	Serial.print ("Counting till reach ");
	Serial.print (count_limit);
	Serial.println (" seeds");
	
	MySW.start();
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************
unsigned long counter_s = 0;
boolean pause = true;	

void loop() {

	pickup_seed();
	counter_s ++;
	Serial.print (" Counted seeds: ");
	Serial.println (counter_s);
	
	if (counter_s == updated_count_limit) {
		MySW.stop();
		pause = true;
		Serial.print ("Reached count value of:  ");
		Serial.println (updated_count_limit);
		Serial.print ("Press 1 to count ");
		Serial.print (count_limit);
		Serial.println(" seeds extra");
		Serial.println ("Press 2 to start from 0");
		Serial.println ("Press 3 to print statistics");
		
		while(pause) {
			switch (return_pressed_button ()) {			
				case 1:
					pause = false;
					updated_count_limit =+ count_limit;
					MySW.start();
				break;
				
				case 2:
					pause = false;
					counter_s = 0;
					updated_count_limit = count_limit;
					count_total_turns = 0;
					MySW.reset();
					MySW.start();
				break;
				
				case 3:
					statistics();
				break;
			}
		}
	}
	
	check_pause ();
	
}

void check_pause () {

	if (Serial.available()) {
		pause = true;
		MySW.stop();
		Serial.flush();
		Serial.println ("Pause activated ");
		Serial.println ("Press 1 to resume");
		Serial.println ("Press 2 to reset counter to default");
		Serial.println ("Press 3 to print statistics");
		
		while(pause) {
			switch (return_pressed_button ()) {			
				case 1:
					pause = false;
					MySW.start();
				break;
				
				case 2:
					pause = false;
					counter_s = 0;
					updated_count_limit = count_limit;
					count_total_turns = 0;
					MySW.reset();
					MySW.start();
				break;
				
				case 3:
					statistics();
				break;
			}
		}
	}
}

void statistics () {
	Serial.print ("\nCounter picked ");
	Serial.print (counter_s);
	Serial.print (" seeds from a total of ");
	Serial.println (updated_count_limit);

	Serial.print ("Counter roll made a total of ");
	Serial.print (count_total_turns);
	Serial.println (" turns");
	
	Serial.print ("Turns rate: ");
	int turns_per_seed = count_total_turns / counter_s;
	Serial.print (turns_per_seed);
	Serial.println (" turns / seed");

	Serial.print ("\ntranscurred time from start: ");
	int minutes = MySW.value() / 60000;
	Serial.print(minutes);
	Serial.print (" : ");
	int seconds = (MySW.value() % 60000) / 1000;
	Serial.println(seconds);
	
	float seeds_per_minute = 0;
	Serial.print ("Seed rate: ");
	if (minutes == 0) {
		seeds_per_minute = counter_s;
	}else{
		float temp_minutes = (seconds/60) + minutes ;
		seeds_per_minute = counter_s / temp_minutes;
	}
	Serial.print(seeds_per_minute);
	Serial.println (" seeds per minute");
}