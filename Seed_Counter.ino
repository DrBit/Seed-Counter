#include <Stepper_ac.h>
#include <avr/pgmspace.h>
#include <StopWatch.h>
#include <SoftwareServo.h> 
//#include <Servo.h> 


#include "list_commands_ethernet.h"		// Check in the same directory

#define version_prog "Testing on Proto4 V5.3.14"
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
// #define Cmotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
// #define Xmotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
// #define Ymotor_debug		// Eneable start of the motors without sensors conected for testing pourpuses only!!!!
#define Sensor_printer		// Disable sensor printer
#define Sensor_blister		// Disable sensor blisters
#define Server_com_debug		// Debug communications with the server
#define Server_com_error_debug  // Debug errors of communication with the server
// #define DEBUG_counter		// Debug counter.. print positions
// #define bypass_server		// Bypass_orders from the server and stat process straight away
#define serial_answers		// Enables serial answers for debuggin or errors with comunication
// Stop Watch Timer
StopWatch MySW;


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
#define sensD A8
#define sensE A9
#define sensF A10		
#define sensG A11
#define sensH A12
#define sensI A13
// NEW SENSOR FOR THE BLISTER 
/* This sensor line is comming directly from the extra I/O from the shield and not from
the control board. 2 Lines ara available
pin Analog 1 and pin 11 (Wich is also a PWM)
Both lines arrive at the board breakout fot he conveier. 1rs is analog , 2nd is pin 11
*/
#define sensJ A2  // 1rst in the connector
#define sensK 11  // Last in the connector


// Servos
#define blister_pinL 7
#define blister_pinR 8
#define ejection_servo_pin 9

// Inputs
#define emergency sensC  // Change in case connected at another input
#define SensBlister sensG
#define SensLabel sensH
#define SensOutBlisters sensI
#define Counter_wheel_sensor sensA
#define Counter_seed_sensor sensD
#define Pneumatics_sensor sensE
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
Stepper_ac Xaxis(stepB,dirB,sensB,ms1B,ms2B,200,8);
// Setting up motor B, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac counter(stepC,dirC,Counter_seed_sensor,ms1C,ms2C,200,2);
// Setting up motor C, step pin, direction pin, NO sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac blisters(stepA,dirA,0,ms1A,ms2A,200,4);
// Setting up motor D, step pin, direction pin, sensor pin, ms1 pin, ms2 pin, 200 steps, 8 for wighth step(mode of the stepper driver)
Stepper_ac Yaxis(stepD,dirD,sensC,ms1D,ms2D,200,8);

// Servo instances
SoftwareServo blister_servoL;  // create servo object to control a servo
SoftwareServo blister_servoR;  // create servo object to control a servo
SoftwareServo ejection_servo;  // create servo object to control a servo

// Servo blisterL;  // create servo object to control a servo 
// Servo blisterR;  // create servo object to control a servo
// Servo ejectblister;	// create servo object to control a servo


// ***********************
// ** DEFINES variables
// ***********************
int a = 0;
long old_xpos=0;
long old_ypos=0;
int motor_select=0;
int situation=0;
const int motor_speed_counter=1000;
const int motor_speed_XY=1200;
const int motor_speed_blisters=1500;
//ethernet flag
boolean connected_to_server = false;
// Blister mode
#define seeds10 1
#define seeds5 2
int blister_mode = 0;
boolean IDLE_mode = false;
boolean endingBatch = false;
boolean do_a_restart = false;
boolean block_loop = false;
boolean autoreset = false;
unsigned int blisters_for_autoreset = 30;
boolean debug_mode_enabled = false;


// ***********************
// ** Error FLAGS
// ***********************
boolean error_XY = true;
boolean error_counter = true;
boolean error_blister = true;
#define ALL 0
#define BLISTERS 1
#define XY 2
#define COUNTER 3

// ***********************
// ** Physical limits of the blister motors
// ***********************
byte M_ID=1;		// Machine ID
byte local_ip[] = { 10,250,1,199 + M_ID };
byte server[] = { 10,250,1,3 }; 
int port = 8888;


// ***********************
// ** Physical limits of the blister motors
// ***********************
unsigned int servoR_open = 153;
unsigned int servoL_open = 30;
unsigned int servoR_close = 182;
unsigned int servoL_close = 4;
unsigned int servoEjection_open = 0;
unsigned int servoEjection_close = 177;

// ***********************
// ** PAUSE & STATISTICS
// ***********************
unsigned long count_total_turns = 0;
unsigned long counter_s = 0;
boolean pause = false;
boolean manual_enabled = false;				// Flag to overwrite the pause flag
// Set the default times (they might be overwrited in the code)
int default_idle_time = 5;				// Defaul idle time to go to sleep on user input 120 = 2 minutes.
int default_off_time = 35;					// Defaul off time to go to sleep on user input 120 = 2 minutes.
// Used forinternal pourpouses
unsigned long  idle_counter_start_time = 0;
unsigned long  desired_idle_time = 0;		// Time in seconds 120s = 2 minutes
unsigned long last_time_server_checked = 0;		// For polling the server at a good rate, not to fast
unsigned long polling_server_rate = 1000;		// Polling rate in ms that we should check for incoming data.

// ***********************
// ** STATUS + ACTIONS
// ***********************
byte global_status = 0;
byte previous_status = 0;
byte server_answer = 0;

// ***********************
// ** Default Direcctions MOTORS
// ***********************
// Defining default directions of motors (in case we change the wiring or the position of motors)
#define default_directionX true
#define default_directionY true
#define default_directionB true
#define default_directionC true

// Defines in which direction of the motor we will find the sensors
#define default_sensor_directionX true
#define default_sensor_directionY true

// Defines the default HIGH (or enabled) state of the sesor
#define default_Ysensor_HIGH_state true
#define default_Xsensor_HIGH_state true


void setup() {
	init_serial();			// INIT Serial
	setup_pins ();			// Setup IO pins

	check_library_version ();	// Check library Version. If different STOP
	speed_cntr_Init_Timer1();	// Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
	delay (10);  				// Delay to be safe	

	// servo_test ();
	init_DBs ();			// Init all databases tables (need to open table to work with them, only one table open).
	debug_mode();			// See if we should go in debug mode
	setup_network();		// First thing we do is set up the network
	server_connect();		// Now we try to stablish a connection
	reset_machine ();		// Reset Machine and be ready for operation
	mem_check();			// Check memory. If it is lower than 1000Kb we could have problems
	// check_blister_sens ();
	// calibrate_counter ();

	// Working setup
	//vibrate_solenoid(solenoid1,3,40);
	// servo_test ();
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************


// Blister positions
// * 10 * 07 * 06 * 03 * 02 * Y2
// * 09 * 08 * 05 * 04 * 01 * Y1
// * X5 * X4 * X3 * X2 * X1 *
void loop() {
	
	// INIT procedure
	check_status (true);			// Check if we need to stop, finish batch, restart or go into test mode.

	// If we got here means we are ready to start. But before that we will check if we got all needed info from the server
	if (get_blister_info ()) {
		check_for_autoreset ();	// If autoreset is enabled check if we have to autoreset
		do_one_blister ();		// Checks the status, waits until we receive info to proceed
	}
}


// Process one blister
void do_one_blister () {

	if (!skip_function()) Serial.println(F("\n ************ "));
	
	get_and_release_blister ();
	print_and_release_label ();
		
	// 10 Seeds mode
	if (blister_mode == seeds10) {
	
		// START FILLING BLISTER
		if (!skip_function()) Serial.print(F("1rst hole"));
		go_to_memory_position (5);			// first hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 2nd hole"));
		go_to_memory_position (6);			// 2nd hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 3th hole"));
		go_to_memory_position (7);			//3th hole
		pickup_seed ();

		if (!skip_function()) Serial.print(F(" - 4rd hole"));
		go_to_memory_position (8);			// 4d hole
		pickup_seed ();

		if (!skip_function()) Serial.println(F(" - 5th hole"));
		go_to_memory_position (9);			// 5th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F("6th hole"));
		go_to_memory_position (10);			// 6th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 7th hole"));
		go_to_memory_position (11);			// 7th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 8th hole"));
		go_to_memory_position (12);			// 8th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 9th hole"));
		go_to_memory_position (13);			// 9th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.println(F(" - 10th hole"));
		go_to_memory_position (14);			// 10th hole
		pickup_seed ();
		
	} else if (blister_mode == seeds5) {		// 5 Seeds mode

		// START FILLING BLISTER
		if (!skip_function()) Serial.print(F("1rst hole"));
		go_to_memory_position (6);			// 2nd hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F(" - 2nd hole"));
		go_to_memory_position (7);			//3th hole
		pickup_seed ();

		if (!skip_function()) Serial.println(F(" - 3th hole"));
		go_to_memory_position (10);			// 6th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.print(F("4th hole"));
		go_to_memory_position (11);			// 7th hole
		pickup_seed ();
		
		if (!skip_function()) Serial.println(F(" - 5th hole"));
		go_to_memory_position (14);			// 10th hole
		pickup_seed ();
		
	}

	// Starts before we go to the printing position because it takes time to start the mechanism
	
	trigger_pneumatic_mechanism ();

	if (!skip_function()) Serial.println(F("Goto print position"));
	go_to_memory_position (3);			// Print position
	
	check_penumatics_are_done (); 	// Check if this is really necessary
	//print_and_release_label ();
	
	//if (!skip_function()) Serial.println(F("Go to exit"));
	//if (global_status != S_finishing_batch)  go_to_memory_position (4);			// Exit

	//eject blister
	eject_blister();
	
	Serial.print (F("Counted seeds: "));
	Serial.println (counter_s);
	
	Serial.print (F("Elapsed time: "));
	unsigned long total_ms = MySW.value();
	print_time(total_ms);

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
	
	// Default sensor HIGH state
	counter.set_default_sensor_state (false);
    
	// set_accel_profile(init_timing, int ramp_inclination, n_slopes_per_mode, n_steps_per_slope)
	Xaxis.set_speed_in_slow_mode (380);
	Xaxis.set_accel_profile(950, 4, 20, 450);
	Yaxis.set_speed_in_slow_mode (1500);
	Yaxis.set_accel_profile(950, 13, 7, 15);
	//Yaxis.set_speed_in_slow_mode (350);
	//Yaxis.set_accel_profile(950, 14, 8, 15);

    // Servo configure
	pinMode (blister_pinL, OUTPUT);
	pinMode (blister_pinR, OUTPUT);
	pinMode (ejection_servo_pin, OUTPUT);

	blister_servoR.attach(blister_pinR);  // attaches the servo on pin 2 to the servo object
	blister_servoR.setMinimumPulse(500);
	blister_servoR.setMaximumPulse(2300);

	blister_servoL.attach(blister_pinL);  // attaches the servo on pin 2 to the servo object
	blister_servoL.setMinimumPulse(500);
	blister_servoL.setMaximumPulse(2300);

	ejection_servo.attach(ejection_servo_pin);  // attaches the servo on pin 2 to the servo object
	ejection_servo.setMinimumPulse(400);
	ejection_servo.setMaximumPulse(2500);

}
