
//////////////////////////
// LIST OF STATUS
//////////////////////////
#define waiting 1 // waiting action
#define 2 // new batch selected
#define 3 // Running
#define 4 // Pause
#define 5 // finished batch IDLE
#define 6 // Error
#define starting__machine 7 // Starting machine

//////////////////////////
// LIST OF ACTIONS
//////////////////////////
1 - 
2
3
4
5
6
7

//////////////////////////
// LIST OF ERRORS
//////////////////////////
0 - Failed to open connection. Network down or website not available
1 - Time out receiving and aswer from the server 
2 - We didnt get any tag equal of what we where expecting
3 - Expected command (C03) to configure printer before anything else
4 - Configuration command not supported (when inside configuration)
5 - Can NOT connect to the User Interface Server. Check connectrions, Check server is alive

10 - Not expected command  	// When we sended a command that receiver wasn't expecting
							// Normalli means receiver expects a concrete command and opnly will react to that
	
#define library_error 11 	// Error at compiling time. Library out of date. Update library and compile again.
#define init_eth_fail 12 	// Error reseting the ethernet module. Check power, connection mega-uno correct?
#define init_Y1_fail  13	// Error initializing Y axis. Could not find the sensor after doing the hole phisical path
#define init_X1_fail  14	// Error initializing X axis. Could not find the sensor after doing the hole phisical path
#define init_Y2_fail  15	// Error initializing Y axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.
#define init_X2_fail  16	// Error initializing X axis. Could not go out of sensor. The axis might be stuck, or the sensor disconnected or broken. Keeps reading ON even if we move it.
#define counter_fail  17    // Counter error, pump might be off, seeds deposits might be empty, sensor might be disconnected or broken
#define blister_release_fail 18 // Blister not released correctly, remove any blister on the belt and press number 1 to try again. Check the sensor if error persist.

send_error_to_server
send_command_to_server
send_status_to_server



// INIT PROCESS
/*

User needs first to be sure machine is clean of blisters on the combeier and with enough 
blister in the deposit

Printer must have one label ready with the selected batch before start