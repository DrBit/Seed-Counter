#include <Stepper_ac.h>
#include <avr/pgmspace.h>
#include <StopWatch.h>

//#include <network_config.h>		// NEDED?
#include "list_commands_ethernet.h"		// Check in the same directory

#define version_prog "V4.0.10"
#define lib_version 15

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
#define Cmotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
#define Xmotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
//#define Ymotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
#define Sensor_printer		// Disable sensor printer
#define Sensor_blister		// Disable sensor blisters
#define Server_com_debug	// Debug communications witht the server
#define Server_com_error_debug // Debug errors of communication with the server
//#define DEBUG_counter		// Debug counter.. print positions
#define bypass_server		// Bypass_orders from the servre and stat process stright away

// example debug:
// #if defined DEBUG
// Serial.println(val);
// #endif

// Stop Watch Timer
StopWatch MySW;

// Machine ID
byte M_ID=1;

// ***********************
// ** DEFINES PIN MAP
// ***********************
// Direction and steps
#define stepA 45
#define stepB 28
#define stepC 26
#define stepD 24
#define stepE 22
#define dirA 31
#define dirB 29
#define dirC 27
#define dirD 25
#define dirE 23
// Speed control
#define ms1A 36
#define ms2A 37
#define ms1B 39
#define ms2B 34
#define ms1C 38
#define ms2C 35
#define ms1D 41
#define ms2D 32
#define ms1E 40
#define ms2E 30
//Control
#define sleep 42
#define enable 33
#define motor_reset 43
// Digital sensors
#define sensA 49
#define sensB 48
#define sensC 46  // Emergency button?
// Analog sensors
#define sensD 8
#define sensE 9
#define sensF 10
#define sensG 11
#define sensH 12
#define sensI 13
#define emergency sensC  // Change in case connected at another input
// Outputs
#define PSupply 47
#define solenoid1 5
#define solenoid2 6
#define pump 2
#define extraoutput 3

// ***********************
// ** Physical limits of the motors
// ***********************
#define Xaxis_cycles_limit 280
#define Yaxis_cycles_limit 12

// ***********************
// ** CONFIG MOTOR PINS
// ***********************
// Setting up motor A, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for eighth step(mode of the stepper driver)
Stepper_ac Xaxis(stepA,dirA,sensA,ms1A,ms2A,200,8);
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
const int motor_speed_counter=1000;
const int motor_speed_XY=900;
const int motor_speed_blisters=1500;
//ethernet flag
boolean connected_to_server = false;
// Blister mode
#define seeds10 1
#define seeds5 2
int blister_mode = 0;

// ***********************
// ** Error FLAGS
// ***********************
boolean error_XY = true;
boolean error_counter = true;
boolean error_blister = true;
#define ALL 0

// ***********************
// ** PAUSE & STATISTICS
// ***********************
unsigned long count_total_turns = 0;
unsigned long counter_s = 0;
boolean pause = false;
boolean manual_enabled = false;				// Flag to overwrite the pause flag
// Set the default times (they might be overwrited in the code)
int default_idle_time = 120;				// Defaul idle time to go to sleep on user input 120 = 2 minutes.
int default_off_time = 120;					// Defaul off time to go to sleep on user input 120 = 2 minutes.
// Used forinternal pourpouses
unsigned long  idle_time_counter = 0;
unsigned long  desired_idle_time = 0;		// Time in seconds 120s = 2 minutes

// ***********************
// ** STATUS + ACTIONS
// ***********************
byte global_status = 0;
byte previous_status = 0;

// ***********************
// ** Default Direcctions MOTORS
// ***********************
// Defining default directions of motors (in case we change the wiring or the position of motors)
#define default_directionX true
#define default_directionY false
#define default_directionB true
#define default_directionC false

#define default_sensor_directionX true
#define default_sensor_directionY false

#define default_Ysensor_HIGH_state true
#define default_Xsensor_HIGH_state false


void setup() {
	init_serial();			// INIT Serial
	setup_pins ();			// Setup IO pins

	check_library_version ();	// Check library Version. If different STOP
	speed_cntr_Init_Timer1();	// Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
	delay (10);  				// Delay to be safe
	init_DB ();				// Init database

	setup_network();		// First thing we do is set up the network
	server_connect();		// Now we try to stablish a connection
	reset_machine ();		// Reset machine (motors, data base fetch, ....)
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************


// Blister positions
// * 10 * 07 * 06 * 03 * 02 * Y2
// * 09 * 08 * 05 * 04 * 01 * Y1
// * X5 * X4 * X3 * X2 * X1 *
void loop() {

	Serial.println(F("\n ************ "));

	/*
	// vibrate_solenoid (byte solenoid_number, byte power, byte duration)
	for (int a = 1; a<=10;a++) {
		Serial.println(F("power "));
		Serial.println(a);
		vibrate_solenoid (solenoid1, a, 30);
		delay (5000);
	}*/

	get_and_release_blister ();
	
	// 10 Seeds mode
	if (blister_mode == seeds10) {
	
		// START FILLING BLISTER
		Serial.print(F("1rst hole"));
		go_to_memory_position (5);			// first hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 2nd hole"));
		go_to_memory_position (6);			// 2nd hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 3th hole"));
		go_to_memory_position (7);			//3th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 4rd hole"));
		go_to_memory_position (8);			// 4d hole
		pickup_seed ();
		//delay (5000);

		Serial.println(F(" - 5th hole"));
		go_to_memory_position (9);			// 5th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F("6th hole"));
		go_to_memory_position (10);			// 6th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 7th hole"));
		go_to_memory_position (11);			// 7th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 8th hole"));
		go_to_memory_position (12);			// 8th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.print(F(" - 9th hole"));
		go_to_memory_position (13);			// 9th hole
		pickup_seed ();
		//delay (5000);
		
		Serial.println(F(" - 10th hole"));
		go_to_memory_position (14);			// 10th hole
		pickup_seed ();
		//delay (5000);
	
	} else if (blister_mode == seeds5) {		// 5 Seeds mode

		// START FILLING BLISTER
		Serial.print(F("1rst hole"));
		go_to_memory_position (6);			// 2nd hole
		pickup_seed ();
		
		Serial.print(F(" - 2nd hole"));
		go_to_memory_position (7);			//3th hole
		pickup_seed ();

		Serial.println(F(" - 3th hole"));
		go_to_memory_position (10);			// 6th hole
		pickup_seed ();
		
		Serial.print(F(" - 4th hole"));
		go_to_memory_position (11);			// 7th hole
		pickup_seed ();
		
		Serial.print(F(" - 5th hole"));
		go_to_memory_position (14);			// 10th hole
		pickup_seed ();
		
	}

	Serial.println(F("Goto print position"));
	go_to_memory_position (3);			// Print position
	
	print_and_release_label ();

	
	Serial.println(F("Go to exit"));
	go_to_memory_position (4);			// Exit
	
	
	Serial.print (F("Counted seeds: "));
	Serial.println (counter_s);
	
	Serial.print (F("Elapsed time: "));
	unsigned long total_ms = MySW.value();
	print_time(total_ms);
	
	check_pause ();
	check_stop ();
	

	/******** USEFUL FUNCTIONS
	
	void mem_check ()
	void press_button_to_continue (int button_number)
	int return_pressed_button ()
	boolean YN_question ()
	int get_number(int buffer) 
	void enter_main_menu()
	void check_pause ()
	void statistics ()
	void print_time (unsigned long total_milliseconds, unsigned int hours, unsigned int minutes, unsigned int seconds)
	void test_functions ()
	*/
}


void testing_motors () {

	//go_to_posXY (int Xcy,int Xst,int Ycy,int Yst)
	go_to_posXY (0,0,2,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,4,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,6,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,8,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,10,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,12,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,14,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,16,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,18,0);

	Serial.println ("Testing finished, restarting...");
	delay (1000);
}


void setup_pins () {
	// Setp all pins
	pinMode (PSupply, OUTPUT);  
	digitalWrite(PSupply, HIGH);	// Disable power supply at de begining
	pinMode (enable, OUTPUT);  
	digitalWrite (enable, HIGH);	// Disable motors before start
	pinMode (sleep, OUTPUT);  
	digitalWrite(sleep, LOW);		// Put drivers in sleep mode
	// Define Outputs
	pinMode (solenoid1, OUTPUT); 
	pinMode (solenoid2, OUTPUT); 
	pinMode (pump, OUTPUT); 
	pinMode (extraoutput, OUTPUT); 
	// Define Inputs
	pinMode (sensE, INPUT);
	pinMode (sensF, INPUT); 
	pinMode (sensG, INPUT); 
	pinMode (sensH, INPUT);
	pinMode (sensI, INPUT);

	// Set default directions for motors
	Xaxis.set_default_direcction (default_directionX);
	Yaxis.set_default_direcction (default_directionY);
	Xaxis.set_default_sensor_state (default_Xsensor_HIGH_state);
	Yaxis.set_default_sensor_state (default_Ysensor_HIGH_state);
	blisters.set_default_direcction (default_directionB);
	counter.set_default_direcction (default_directionC);
    
	// set_accel_profile(init_timing, int ramp_inclination, n_slopes_per_mode, n_steps_per_slope)
	Xaxis.set_speed_in_slow_mode (400);
	Xaxis.set_accel_profile(900, 17, 9, 20);
	Yaxis.set_speed_in_slow_mode (350);
	Yaxis.set_accel_profile(950, 13, 7, 15);
	//Yaxis.set_speed_in_slow_mode (350);
	//Yaxis.set_accel_profile(950, 14, 8, 15);
}



void vibrate_solenoid (byte solenoid_number, byte power, byte duration) {

// power 1 - 10 its the delay of the inner oscilation, the fastes (lowest number) less powerful
// the slowest (higher number) stronger the vibration
	for (int c=0; c<duration; c++) {
		for (int a=0; a < 100; a++) {
			digitalWrite (solenoid_number, HIGH);
			delay (power*2);
			digitalWrite (solenoid_number, LOW);
			delay (power*2);
		}
	}
}


void chec_sensorF () {
	while (true) {
		int sensor_state = digitalRead (sensF); 
		if (sensor_state) {
			// We got the begining of the blister
			print_ok();
			// We got a blisters
			// Now we know that we have just a few left
			// We start counting
			// How can we reset this count when blisters are refilled? Database?
				// In database case. Check database, if refilled reset state.
		}else{
			print_fail ();
			// lister not detected, send error
			// press_button_to_continue (1);
		}
		delay (700);
	}
}

