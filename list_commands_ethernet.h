
//////////////////////////
// LIST NEEDED CONFIG
//////////////////////////
// Basic
////////////////////////
// get_info_from_server (info); (I)
#define get_seeds_mode 1			// 5 or 10 seeds per blister


//////////////////////////
// LIST OF STATUS
//////////////////////////
// send_status_to_server (status); (S)
#define S_stopped 1 		// waiting action
#define S_running 2 		// Running
#define S_pause 3 			// Pause
#define S_setting_up 4 		// Setting up machine  or resetting the machine
#define S_finishing_batch 5 // Finishing batch
#define S_switch_off 6		// Machine is OFF
#define S_test 7			// Machine is in test mode (mainly testing positions)

// The list of status should be as short as possible to make it easyer to handler
// Statuses are contineous events that when enabled will stay enabled till another change on status
// The differences with actions are that actions are single events that happen in one moment.


//////////////////////////
// LIST OF ACTIONS
//////////////////////////
// send_action_to_server(action); (A)
#define blister_release 1			// Release one blister
#define blister_refilled 2			// Blisters filled, starting again...
#define counter_init 3				// Init Counter Motor
#define new_batch 4		 			// new batch selected (not used in arduino, needed???)
#define batch_end 5 				// finished batch, going IDLE
#define NOTHING 6 					// Nothing going on
#define starting_machine 7 			// Starting machine
#define try_counter_autofix	8		// Try counter autofix, re-init counter wheel.
#define needed_blisters_refill 9	// Required user to refill blisters
#define seed_counter_turn	10		// Wheel turned 1 time.
#define seed_released	11			// Seed has been picked up and released in the blister
#define seed_missed	12				// Seed has been missed
#define enable_pump  13				// Enable pump
#define disable_pump  14			// Disable pump
#define enter_idle	15				// Entering IDLE, awaiting user response or another input
#define resume_from_idle  16		// Resuming action from IDLE
#define ask_for_label  17			// Ask server to print a label
#define label_ok   18				// Label detected correctly
#define XY_init 19					// Init XY axis
#define blisters_disp_init 20		// Init blisters

#define awake_motors  21    // Awake motor drivers, sleep off
#define sleep_motors 22     // Sleep motor drivers, sleep on
#define enable_motors 23    // Enable motor drivers
#define disable_motors 24   // Disable motor drivers
#define power_on 25  		// Power on internal circuits (Motors, solenoids, pump_switch)
#define power_off 26 		// Power off internal circuits

// Specific answers for error solving
#define button_continue 27 			// Power off internal circuits
#define button_ignore 28 			// Power off internal circuits
#define button_finish 29 			// For finishing batch when we are out of seeds (sended by server)
#define button_print_label 30 		// Print one label

//////////////////////////
// LIST OF ERRORS
//////////////////////////
// send_error_to_server (error); (E)
#define no_error 1 
#define library_error 11 	// Error at compiling time. Library out of date. Update library and compile again.
#define init_eth_fail 12 	// Error reseting the ethernet module. Check power, connection mega-uno correct?
#define init_Y1_fail  13	// Error initializing Y axis. Could not find the sensor after doing the hole phisical path
#define init_X1_fail  14	// Error initializing X axis. Could not find the sensor after doing the hole phisical path
#define init_Y2_fail  15	// Error initializing Y axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.
#define init_X2_fail  16	// Error initializing X axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.

#define counter_init_fail  17    	// // Error in the detection of the wheel. Switch sensor might be disconnected
#define counter_max_turns_normal 18 // Counters has done too much turns. Check if there are seeds, bottle neck or sensor error.
#define counter_max_turns_end 19 	// Counters has done too much turns and already counted all seeds. Check if we finished the batch, bottle neck or sensor error.
#define counter_sensor_failed 20 	// Counter sensor has detected a seed and the wheel was not in the sensor position. This could mean that wheel has skiped steps, sensor malcfunction or dirty sensor.
#define label_timeout  21			// Label not printed or timeout
#define blister_release_fail 22 	// Blister not released correctly, remove any blister on the belt and press number 1 to try again. Check the sensor if error persist.
#define missing_blister_info 23		// The blister information I1 has not been supplied by the server (5 or 10 seeds per blister). Machine can not continue whitout this information



//////////////////////////
// LIST OF POSITIONS
//////////////////////////
// get_positions_from_server (position); (P)		// Receive position information stored in the server
// send_position_to_server (position); (G)			// Inform server that we are going to a position

#define P0 0 // List of all positions (server gets P*)
#define P1 1 // Go to position 1
#define P2 2 // Go to position 2
#define P3 3 // Go to position 3
#define P4 4 // Go to position 4
#define P5 5 // Go to position 5
#define P6 6 // Go to position 6
#define P7 7 // Go to position 7
#define P8 8 // Go to position 8
#define P9 9 // Go to position 9
#define P10 10 // Go to position 10
#define P11 11 // Go to position 11
#define P12 12 // Go to position 12
#define P13 13 // Go to position 13
#define P14 14 // Go to position 14
#define P15 15 // Go to position 15
#define P16 16 // Go to position 16
#define P17 17 // Go to position 17
#define P18 18 // Go to position 18
#define P19 19 // Go to position 19
#define P20 20 // Go to position 20
#define P21 21 // Go to position 21
#define P22 22 // Go to position 22
#define P23 23 // Go to position 23
#define P24 24 // Go to position 24
#define P25 25 // Go to position 25
#define P26 26 // Go to position 26
#define P27 27 // Go to position 27
#define P28 28 // Go to position 28
#define P29 29 // Go to position 29
#define P30 30 // Go to position 30
#define P31 31 // Go to position 31
#define P32 32 // Go to position 32
#define P33 33 // Go to position 33
#define P34 34 // Go to position 34
#define P35 35 // Go to position 35
#define P36 36 // Go to position 36
#define P37 37 // Go to position 37
#define P38 38 // Go to position 38
#define P39 39 // Go to position 39
#define P40 40 // Go to position 40
#define P41 41 // Go to position 41
#define P42 42 // Go to position 42
#define P43 43 // Go to position 43
#define P44 44 // Go to position 44
#define P45 45 // Go to position 45
#define P46 46 // Go to position 46
#define P47 47 // Go to position 47
#define P48 48 // Go to position 48
#define P49 49 // Go to position 49
#define P50 50 // Go to position 50


//////////////////////////
// LIST OF CONFIGURATIONS
//////////////////////////
// get_config_from_server (configuration); (C)

#define C_All 0                             // Asks for all the configurations
#define Cget_default_idle_time 1			// Defaul idle time to go to sleep on user input 120 = 2 minutes.
#define Cget_default_off_time 2	            // Defaul off time to shutdown after idle is activated 120 = 2 minutes.
#define Cget_autoreset_state 3 				// Receives autoreset state 0 disabled - 1 enabled
#define Cget_autoreset_value 4      		// Receives the number of blisters after it will be reset automatically.
#define Cget_server_polling_time 5 			// Receives the min amout of time that we will fetch server for information


// not defined iet in server side
#define Cblisters_steps_limit 	20	        // Max steps to release one blister (default 300)
#define Cblisters_steps_absoulut_limit 21   	// Max of steps needed to get from min to max (default 1000)
#define Csteps_to_move_when_blister_falls 22     // Steps that Y axis will move to fit the blister in the conveier (default 800)
#define Csteps_from_sensor_to_init_clockwise 23  			// Number of steps (based in mode 8) to go forward from the sensor to the init position
#define Csteps_from_sensor_to_start_moving_when_seed 24		// Number of steps (based in mode 8) away form the pick a seed point to start moving the axis when we got a seed.
#define Cmargin_steps_to_detect_seed 25		// Its the steps margin in wich the sensor will check if we have a seed
#define Cfails_max_normal 26					// Max number of tries to pick a seed before software will create an error
#define Cfails_max_end 27				// Max number of fails before 100 seeds to reach the complet batch to create an error (since we are close to the end we dont need to go to 1000)
#define Cinit_turns_till_error 28   		// Number of times the counter will try to get a seed at INITIATION before giving an error
#define Cmax_batch_count 29	                // Tipical number of seeds in a batch
#define CXaxis_cycles_limit 30
#define CYaxis_cycles_limit 31
#define Cmax_insensor_stepsError 32		// When init
#define Cdefault_directionX 33
#define Cdefault_directionY 34
#define Cdefault_directionB 35
#define Cdefault_directionC 36


/* To implement in the future
// List of all parameters of the code, for now due to the complexity of it
// we leave them as they are

// Speeds
// MAX speed!
Xaxis.set_speed_in_slow_mode (400);
Xaxis.set_accel_profile(900, 17, 9, 20);
Yaxis.set_speed_in_slow_mode (350);
Yaxis.set_accel_profile(950, 13, 7, 15);
*/
