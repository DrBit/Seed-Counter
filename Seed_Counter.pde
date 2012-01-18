#include <Stepper_ac.h>
#include <avr/pgmspace.h>
#include <StopWatch.h>

#define version_prog "V3.4.2"
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

#define ethReset 39

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
unsigned int max_batch_count = 1100;


void setup() {
	
	// INIT Serial
	init_serial();	
	//Configure 3 Input Buttons
	pinMode (button1, INPUT);
	pinMode (button2, INPUT);
	pinMode (button3, INPUT);
	// Controls ethernet reset
	pinMode (ethReset, OUTPUT);
	digitalWrite (ethReset, HIGH);
	
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

	init_DB ();				// Init database
	// Show_all_records();
	// manual_data_write();		// UPDATE manually all EEPROOM MEMORY (positions)
	// press_button_to_continue (1);
	
	Serial.println("\n*****************");
	Serial.println("** SETTING UP  **");
	Serial.println("*****************");
	
	
	init_network ();					// Init network
	configure_network ();				// Self configure network
	prepare_printer();					// Prepares the printer to be ready for blisters 
	// select_batch_number ();			// Ask for a batch number
	
	
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
	int temp_err = 0;   // flag for found errors
	if (!init_blocks(ALL)) temp_err = 1;
	
	while (temp_err > 0) { // We found an error, we chek ALL errors and try to initiate correctly
		temp_err = 0;
		Serial.println("\nErrors found, press 1 when ready to check again, 2 to bypas the errors");
		switch (return_pressed_button ()) {
			//Init XY 
			case 1:
				if (error_XY) {
					if (!init_blocks(2)) temp_err++;
				}
				if (error_counter) {
					if (!init_blocks(3)) temp_err++;
				}
				if (error_blister) {
					if (!init_blocks(1)) temp_err++;
				}
			break;
			
			case 2:
				// do nothing so we wond detect any error and we will continue
			break;
		}
	}
	
	// some motor adjustments ( This configurations have been proved to work well, but there is still room for adjustments )
	// Xaxis.set_speed_in_slow_mode (350);
	// Xaxis.set_accel_profile(900, 17, 9, 20);			// Proven to be working really good
	// Yaxis.set_speed_in_slow_mode (550);
	// Yaxis.set_accel_profile(1100, 14, 9, 20);		// Proven to be working really good
	
	// set_accel_profile(init_timing, int ramp_inclination, n_slopes_per_mode, n_steps_per_slope)
	// MAX speed!
	Xaxis.set_speed_in_slow_mode (400);
	Xaxis.set_accel_profile(900, 17, 9, 20);
	Yaxis.set_speed_in_slow_mode (350);
	Yaxis.set_accel_profile(950, 13, 7, 15);
	
	MySW.start();			// Start timer for statistics
	// END of setup
}


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************


// Blister positions
// * 10 * 08 * 06 * 04 * 02 * Y2
// * 09 * 07 * 05 * 03 * 01 * Y1
// * X5 * X4 * X3 * X2 * X1 *
void loop() {

	update_positions_information ();	// Sends petition to get postions data
	// receive position data from ethernet module
	// Check if its correct and update if necessary

	Serial.println("\n ************ ");
	
	Serial.println("go to Blister Position");
	go_to_memory_position (2);			// blister
	
	Serial.println("Get blister");
	release_blister ();

	Serial.print("1rst hole");
	go_to_memory_position (5);			// first hole
	pickup_seed ();
	
	Serial.print(" - 2nd hole");
	go_to_memory_position (6);			// 2nd hole
	pickup_seed ();
	
	Serial.print(" - 3th hole");
	go_to_memory_position (7);			//4th hole
	pickup_seed ();
	
	Serial.print(" - 4rd hole");
	go_to_memory_position (8);			// 3d hole
	pickup_seed ();

	Serial.println(" - 5th hole");
	go_to_memory_position (9);			// 5th hole
	pickup_seed ();
	
	Serial.print("6th hole");
	go_to_memory_position (10);			// 6th hole
	pickup_seed ();
	
	Serial.print(" - 7th hole");
	go_to_memory_position (11);			// 8th hole
	pickup_seed ();
	
	Serial.print(" - 8th hole");
	go_to_memory_position (12);			// 7th hole
	pickup_seed ();
	
	Serial.print(" - 9th hole");
	go_to_memory_position (13);			// 9th hole
	pickup_seed ();
	
	Serial.println(" - 10th hole");
	go_to_memory_position (14);			// 10th hole
	pickup_seed ();

	
	Serial.println("Goto print position");
	go_to_memory_position (3);			// Print position
	
	print_one_label ();
	
	// Wait for the printer to print a label
	delay (3800);
	
	Serial.println("Go to brush position");
	go_to_memory_position (20);
	
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

