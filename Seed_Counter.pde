#include <Stepper_ac.h>
#include <avr/pgmspace.h>
#include <StopWatch.h>

#include <network_config.h>

#define version_prog "V3.9"
#define lib_version 13


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
#define sensC 48
#define sensD 7
#define sensE 46
#define sensF 47

#define ms1A 43
#define ms2A 42
#define ms1B 41
#define ms2B 40
#define ms1C 44
#define ms2C 45
#define ms1D 51
#define ms2D 50

#define ethReset 39
#define emergency 13
#define pump 49

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
const int motor_speed_counter=1000;
const int motor_speed_XY=900;
const int motor_speed_blisters=1500;

// Blister mode
#define seeds5 1
#define seeds10 2
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



void setup() {
	
	// INIT Serial
	init_serial();	
	//Configure 3 Input Buttons
	pinMode (button1, INPUT);
	pinMode (button2, INPUT);
	pinMode (button3, INPUT);
	
	pinMode (sensF, INPUT); 
	pinMode (sensE, INPUT);
	pinMode (sensC, INPUT); 
	
	// Setup emergency pin 
	pinMode (emergency, INPUT);           // set pin to input

	// Controls ethernet reset
	pinMode (ethReset, OUTPUT);
	digitalWrite (ethReset, HIGH);
	
	// Controls Pump relay
	pinMode (pump, OUTPUT);
	
	// Check library Version
	check_library_version ();		//If different STOP
	
	// Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
	speed_cntr_Init_Timer1();
	delay (10);  // Delay to be safe

	init_DB ();				// Init database
	// Show_all_records();
	// manual_data_write();		// UPDATE manually all EEPROOM MEMORY (positions)
	
	print_set_up ();		// Begin SET-UP process
	
	// Init network module
	init_printer ();		// Init printer
	prepare_printer();		// Prepares the printer to be ready for blisters 
	
	// Defining default directions of motors (in case we change the wiring or the position of motors)
	#define default_directionX true
	#define default_directionY false
	#define default_directionB true
	#define default_directionC false

	// Set default direction
	Xaxis.set_default_direcction (default_directionX);
	Yaxis.set_default_direcction (default_directionY);
	blisters.set_default_direcction (default_directionB);
	counter.set_default_direcction (default_directionC);
	
	// INIT SYSTEM, and CHECK for ERRORS
	init_all_motors ();
		
	// set_accel_profile(init_timing, int ramp_inclination, n_slopes_per_mode, n_steps_per_slope)
	// MAX speed!
	Xaxis.set_speed_in_slow_mode (400);
	Xaxis.set_accel_profile(900, 17, 9, 20);
	Yaxis.set_speed_in_slow_mode (350);
	Yaxis.set_accel_profile(950, 13, 7, 15);
	
	// Pick bl;ister mode to start filling
	pick_blister_mode();
	
	// chec_sensorF();
	MySW.start();			// Start timer for statistics
	// END of setup
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************


// Blister positions
// * 10 * 07 * 06 * 03 * 02 * Y2
// * 09 * 08 * 05 * 04 * 01 * Y1
// * X5 * X4 * X3 * X2 * X1 *
void loop() {

	Serial.println("\n ************ ");
	Serial.println("Get blister");
	release_blister ();
	
	boolean released = check_blister_realeased ();
	while (!released) {
		Serial.println("Blister malfunction, blister will be removed and we will try again. Check for any potencial problem before continuing.");
		Serial.println("Press 1 to continue");
		press_button_to_continue (1);
		
		Serial.println("Go to exit");
		go_to_memory_position (4);			// Exit
		
		Serial.println("Get blister");
		release_blister ();
		released = check_blister_realeased ();
	}
	// Check if blister has been released correctly
	
	// 10 Seeds mode
	if (blister_mode == seeds10) {
	
		// START FILLING BLISTER
		Serial.print("1rst hole");
		go_to_memory_position (5);			// first hole
		pickup_seed ();
		
		Serial.print(" - 2nd hole");
		go_to_memory_position (6);			// 2nd hole
		pickup_seed ();
		
		Serial.print(" - 3th hole");
		go_to_memory_position (7);			//3th hole
		pickup_seed ();
		
		Serial.print(" - 4rd hole");
		go_to_memory_position (8);			// 4d hole
		pickup_seed ();

		Serial.println(" - 5th hole");
		go_to_memory_position (9);			// 5th hole
		pickup_seed ();
		
		Serial.print("6th hole");
		go_to_memory_position (10);			// 6th hole
		pickup_seed ();
		
		Serial.print(" - 7th hole");
		go_to_memory_position (11);			// 7th hole
		pickup_seed ();
		
		Serial.print(" - 8th hole");
		go_to_memory_position (12);			// 8th hole
		pickup_seed ();
		
		Serial.print(" - 9th hole");
		go_to_memory_position (13);			// 9th hole
		pickup_seed ();
		
		Serial.println(" - 10th hole");
		go_to_memory_position (14);			// 10th hole
		pickup_seed ();
	
	} else if (blister_mode == seeds5) {		// 5 Seeds mode

		// START FILLING BLISTER
		Serial.print("1rst hole");
		go_to_memory_position (5);			// first hole
		pickup_seed ();
		
		Serial.print(" - 2nd hole");
		go_to_memory_position (8);			//4th hole
		pickup_seed ();

		Serial.println(" - 3th hole");
		go_to_memory_position (9);			// 5th hole
		pickup_seed ();
		
		Serial.print(" - 4th hole");
		go_to_memory_position (12);			// 8th hole
		pickup_seed ();
		
		Serial.print(" - 5th hole");
		go_to_memory_position (13);			// 9th hole
		pickup_seed ();
		
	}

	Serial.println("Goto print position");
	go_to_memory_position (3);			// Print position
	
	print_one_label ();
	
	// Wait for the printer to print a label
	released = check_label_realeased (true);
	while (!released) {
		Serial.println("Lable error, remove any label that might be left and press number 1 to try again or 2 to continue.");
		int button_pressed = return_pressed_button ();
		if (button_pressed == 2) break;
		
		Serial.println("Goto print position");
		go_to_memory_position (3);			// Print position
		print_one_label ();
		released = check_label_realeased (true);
	}
	
	Serial.println("Go to exit");
	go_to_memory_position (4);			// Exit
	
	
	Serial.print ("Counted seeds: ");
	Serial.println (counter_s);
	
	Serial.print ("Elapsed time: ");
	unsigned long total_ms = MySW.value();
	print_time(total_ms);
	
	check_pause ();
	

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


void chec_sensorE () {
	while (true) {
		int sensor_state = digitalRead (sensE); 
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
