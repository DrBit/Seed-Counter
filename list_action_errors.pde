
//////////////////////////
// LIST NEEDED CONFIG
//////////////////////////
// get_config_from_server (config);
// #define get_batch_ID 1			// batch ID number NOT NEEDED
#define get_seeds_mode 2		// 5 or 10 seeds per blister
// #define get_all_positions 3		// List of all positions

//////////////////////////
// LIST OF STATUS
//////////////////////////
// send_status_to_server (status);
#define stopped 1 	// waiting action
#define running 2 	// Running
#define pause 3 	// Pause   
// #define error 4 	// Error (defines an error state that needs user attention) NOT necessary

// The list of status should be as short as possible to make it easyer to handler
// Statuses are continueos events that when enabled will stay enabled till another change on status
// The differences with actions are that actions are single events that happen in one moment.


//////////////////////////
// LIST OF ACTIONS
//////////////////////////
// send_action_to_server(action);
#define blister_release 1				// Release one blister
#define blister_refilled 2				// Blisters filled, starting again...
#define counter_init 3					// Init Counter Motor
#define 4 			// new batch selected (not used in arduino, needed???)
#define batch_end 5 				// finished batch, going IDLE
#define starting_machine 7 			// Starting machine
#define try_counter_autofix	8		// Try counter autofix, re-init counter wheel.
#define needed_blisters_refill 9	// Required user to refill blisters
#define
#define
#define
#define
#define
#define
#define
#define
#define
#define
#define

//////////////////////////
// LIST OF ERRORS
//////////////////////////
// send_error_to_server (error);
/*
// this ones might be changed....
0 - Failed to open connection. Network down or website not available
1 - Time out receiving and aswer from the server 
2 - We didnt get any tag equal of what we where expecting
3 - Expected command (C03) to configure printer before anything else
4 - Configuration command not supported (when inside configuration)
5 - Can NOT connect to the User Interface Server. Check connectrions, Check server is alive

10 - Not expected command  	// When we sended a command that receiver wasn't expecting
							// Normalli means receiver expects a concrete command and only will react to that
*/
#define library_error 11 	// Error at compiling time. Library out of date. Update library and compile again.
#define init_eth_fail 12 	// Error reseting the ethernet module. Check power, connection mega-uno correct?
#define init_Y1_fail  13	// Error initializing Y axis. Could not find the sensor after doing the hole phisical path
#define init_X1_fail  14	// Error initializing X axis. Could not find the sensor after doing the hole phisical path
#define init_Y2_fail  15	// Error initializing Y axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.
#define init_X2_fail  16	// Error initializing X axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.
#define counter_init_fail  17    // Counter error, pump might be off, seeds deposits might be empty, sensor might be disconnected or broken

#define counter_max_turns_normal 19 // Counters has done too much turns. Check if there are seeds, bottle neck or sensor error.
#define counter_max_turns_end 20 // Counters has done too much turns and already counted all seeds. Check if we finished the batch, bottle neck or sensor error.
#define counter_sensor_failed 21 // Counter sensor has detected a seed and the wheel was not in the sensor position. This could mean that wheel has skiped steps, sensor malcfunction or dirty sensor.

#define blister_release_fail 18 // Blister not released correctly, remove any blister on the belt and press number 1 to try again. Check the sensor if error persist.
#define blister_empty // Blisters are empty, refill needed

//////////////////////////
// LIST OF POSITIONS
//////////////////////////
// send_position_to_server (position);
// P* means all positions.
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




// INIT PROCESS
/*

User needs first to be sure machine is clean of blisters on the combeier and with enough 
blister in the deposit

Printer must have one label ready with the selected batch before start
*/