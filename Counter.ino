// ************************************************************
// ** INIT SEED COUNTER
// ************************************************************
// Inits the seedcounter using the sensor as a starting point

// Define positions of the wheel
// This positions will be in future storen in the UI
unsigned int pick_seed_position = 190;			// Position in which seeds are picked up	
unsigned int detect_seed_position = 598;		// Position in which seeds are detected
unsigned int margin_steps_to_detect_seed = 40;	// Margin in which the seed detector could detect a seed
unsigned int drop_position = 1176;				// Position in which seeds are droped 
unsigned int seed_sensor_sensivity = 900; 		// From 1023 Completely open to 0 completely closed sensor
		
// Defines max counts that will triger certain errors
unsigned int fails_max_normal = 40;				// Max number of tries to pick a seed before software will create an error
unsigned int fails_max_end = 20;				// Max number of fails before 100 seeds to reach the complet batch to create an error (since we are close to the end we dont need to go to 1000)
unsigned int fails_max_init_tries = 10;   		// Number of times the counter will try to get a seed at INITIATION before giving an error
unsigned int max_batch_count = 1100;			// Tipical number of seeds in a batch
unsigned int count_error_turns = 0;				// Used to count the amount of consecutive fails when picking up a seed (initially 0)

boolean first_time_drop = true;		// Used only to acomodate position after INIT. Once hase been used we won't used anymore.

unsigned int positive_acumulate_counter = 0;
unsigned int negative_acumulate_counter = 0;
unsigned int limit_consecutive_seeds_detected = 4;



boolean Seedcounter_init() {		// Init seed counter motors and sensors
	read_database_DB_counter ();	// Read values from DB
	Update_positions_accuracy ();	// this is done once per reset to update accuray from values of the database
	boolean wheel_sensor = false; 
	unsigned int count = 0;
	#if defined Cmotor_debug		// In case of debug do nothing so we skip the init 
	wheel_sensor = true;
	#endif
	counter.set_direction (!default_directionC);   	// Set direction
    send_action_to_server(counter_init);			// Send info to server
	
	while (!wheel_sensor) {			// Detect init position of the wheel
		if (count == (1600 / counter.get_step_accuracy())) {	// If after one full turn we haven´t detected the sensor something is wrong
			send_error_to_server(counter_init_fail);	// Error in the detection of the wheel. Switch might be disconnected
			return false;			// Failed to initiate seed counter, return false
		}  
		if (digitalRead(Counter_wheel_sensor)) {	// If the sensor is true means we found the position of the sensor
			wheel_sensor = true;
		}else{
			counter.do_step();		
			delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
		}
		count++;
	}
	counter.set_init_position();		// Set init position

	// Once we set the init position we can go to pick_up_seed position to be ready to start
	int steps_to_do = (pick_seed_position);
	counter.set_direction (default_directionC);   // Set direction
	#if defined DEBUG_counter
	Serial.print (F("Steps to do to go to seed point: "));
	Serial.println (steps_to_do);
	#endif
	for (int a = 1; a <= steps_to_do; a++){
		counter.do_step();	
		delayMicroseconds(motor_speed_counter*5);		// we do it 5 times slower to avoid breaking anything
	}
	#if defined DEBUG_counter
	Serial.print (F("Init position: "));
	Serial.print (counter.get_steps_cycles());
	Serial.print (F(" - "));
	Serial.println (counter.get_steps());
	#endif
	return true;
}

///////////////////////////////////////////////////////////////////
// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
boolean pickup_seed() {
	pump_enable ();
	motor_select = 0;
	count_error_turns = 0;  // restore missed seeds counter
	boolean seed_detected = false;	
	boolean temp_detection = false;		//Var for mark detection of a seed inside the switch
	#if defined DEBUG_counter
	Serial.print (F("We are starting pickup_seed function at position: "));
	Serial.print (counter.get_steps_cycles());
	Serial.print (F(" - "));
	Serial.println (counter.get_steps());
	#endif
	int accel = 4500;			// Values of the motor acceleration
	int speed = 9500;			// Values of the motor max speed
	#define case_error_checking 1 	// Values for handling the cases
	#define case_vibrate 2
	#define case_pick_seed_position 3
	#define case_detect_seed_position 4
	#define case_drop_position 5
	byte counting_case = 1;
	int upper_limit = (detect_seed_position + margin_steps_to_detect_seed)*counter.get_step_accuracy();	// Precalculate some variable to avoid loosing time insde the functions
	int lower_limit = (detect_seed_position - margin_steps_to_detect_seed)*counter.get_step_accuracy();
	boolean sensor_skip = false;		//Var for debugging
	#if defined Cmotor_debug
	sensor_skip = true; 
	#endif

	while (!seed_detected && !skip_function()) {
		// Skip function only reacts at ending batch or do a restart.. try more in deep
		switch (counting_case) {
			case case_error_checking:		// First some error checking.
				#if defined DEBUG_counter
				Serial.print (F("Error Checking: "));
				Serial.print (counter.get_steps_cycles());
				Serial.print (F(" - "));
				Serial.println (counter.get_steps());
				Serial.print ("Missed Seeds: ");
				Serial.print (count_error_turns);
				#endif
				if (count_error_turns > fails_max_normal) {				// Checks if we did too many turns. Could mean a bottle neck or just out of seeds
					send_error_to_server(counter_max_turns_normal);
					count_error_turns = 0;  // restore missed seeds counter
					if (check_if_end_of_batch ()) {
						return true;	// Should it say return?
					}
				}else if ((count_error_turns > fails_max_end) && (counter_s > (max_batch_count - 200))) {
					send_error_to_server(counter_max_turns_end);
					count_error_turns = 0;  // restore missed seeds counter
					if (check_if_end_of_batch ()) {
						return true;	// Should it say return?
					}
				}
				// Check if we have received a button like restart or pause or stop batch? isnt it?
				check_status (false);
				// Here we can not do a check stop cause we would go into an endless loop??
				///////////////////////////
				counting_case = case_pick_seed_position;	// Pick up a seed
			break;

			case case_vibrate:
				// here we can also implement de laser sensor so we have complete control over the seeds position
				if (false) {
					// If laser atttached

				}else{
					// If laser not atached
					if (temp_detection) {
						positive_acumulate_counter ++;
						negative_acumulate_counter  = 0;
						seed_detected = true;
					}else{
						positive_acumulate_counter = 0;
						negative_acumulate_counter ++;
					}

					if (positive_acumulate_counter < limit_consecutive_seeds_detected) {
						// RETHINK!!!!!
						if (negative_acumulate_counter >= 1) {
							// increase the power of vibration
							// vibrate_solenoid(poin_number, power, duration
							int power = 4 + (negative_acumulate_counter);
							if (power >= 6) power = 6;
							int duration = 100 -(negative_acumulate_counter*10);
							if (duration <= 40) duration = 40;
							vibrate_solenoid(solenoid1,power,duration);		// We vibrate for a fixed amount of time each time
							positive_acumulate_counter = limit_consecutive_seeds_detected - 1;	// Next detected seed will stop vibrating
						}else{
							// vibrate_solenoid(poin_number, power, duration)
							vibrate_solenoid(solenoid1,5,100);		// We vibrate for a fixed amount of time each time
							// was 100 instead of 80
						}
					} else {
						// Fail to vibrate and reset negative counter
						vibrate_solenoid(solenoid1,4,50);
						negative_acumulate_counter = 0;
					}
				}

				#if defined DEBUG_counter
				Serial.print (F("Vibrating: "));
				Serial.print (counter.get_steps_cycles());
				Serial.print (F(" - "));
				Serial.println (counter.get_steps());
				#endif
				
				counting_case = case_error_checking;
			break;

			case case_pick_seed_position:
				if (counter.get_steps() == (pick_seed_position*counter.get_step_accuracy())) {
					#if defined DEBUG_counter
					Serial.print (F("We are now at pick up position: "));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
					send_action_to_server(seed_counter_turn);	// Send info to the server
					unsigned int steps_count = (drop_position - pick_seed_position);
					if (default_directionC) steps_count = -steps_count;
					speed_cntr_Move(steps_count,accel,speed,accel);	// We go to release position. NOTICE that the acceleration in this case is lower
					counting_case = case_detect_seed_position;
				}else{
					#if defined DEBUG_counter
					Serial.print (F("Waiting to arrive at pick up seed position"));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
				}
			break;

			case case_detect_seed_position:
				if ((counter.get_steps() >= lower_limit) && (counter.get_steps() <= upper_limit)) {	// We check the sensor only when we are in the range of the sensor
					if (check_seed_sensor() || sensor_skip){		// We got a seed!!!
						counter_s ++;			// For statistics pourpouse
						count_error_turns = 0;  // restore missed seeds counter
						counting_case = case_drop_position; // Go to release seed
						temp_detection = true;
					}
				}else{
					if (check_seed_sensor () && !sensor_skip){	// We detected a seed We are not in sensor position
						check_faulty_sensor ();	// Reset wheel
						counting_case = case_error_checking;	// Restarts counting cycle
					}
				}
				if (counter.get_steps() == (drop_position*counter.get_step_accuracy())) {
					counting_case = case_drop_position; // If we are here we did not detect a seed and we already reached drop zone
					count_error_turns ++;
				}
			break;

			case case_drop_position:
				if (counter.get_steps() == (drop_position*counter.get_step_accuracy())) {
					#if defined DEBUG_counter
					Serial.print (F("_We are at drop position: "));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
					if (temp_detection) {
						send_action_to_server(seed_released);
						#if defined DEBUG_counter
						Serial.println(F("Seed done"));
						#endif
					}else{
						send_action_to_server(seed_missed);
						#if defined DEBUG_counter
						Serial.println(F("Seed missed"));
						#endif
					}
					count_total_turns++;
					unsigned int steps_count = (drop_position - pick_seed_position);
					if (!default_directionC) steps_count = -steps_count;
					speed_cntr_Move(steps_count,accel,speed,accel);	// We go to release position. NOTICE that the acceleration in this case is lower
					counting_case = case_vibrate;
				}else{
					#if defined DEBUG_counter
					Serial.print (F("Waiting to arrive at drop position: "));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
				}
			break;
		}
	}
}

boolean check_faulty_sensor () {
	send_error_to_server(counter_sensor_failed);
	#if defined serial_answers
	Serial.println (F("\nError in wheel. Missed steps? Sensor detected assed where it should not be"));
	Serial.println (F("Check sensors and press one to reset wheel"));
	Serial.println (F("Press 1 to continue:"));
	clean_serial_buffer ();		// Cleans serial buffer from possible unwanted sended keys
	#endif
	boolean released = false;
	block_loop = true;
	while (!released && !skip_function()) {
		check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
		switch (server_answer) {
			case button_continue:
				Serial.println(F("Continue button pressed"));
				if (Seedcounter_init ()) {
					released = true;
					send_error_to_server (no_error);		// Reset error on the server
					return true;
				}else{
					// Init failed
					server_answer = button_continue;
				}
			break;

			default:
				// Any other answer or 0
				server_answer = 0;
			break;
		}
	}
	Serial.println(F("Out of reset funciton"));
	block_loop = false;
}

boolean check_if_end_of_batch () {
	#if defined serial_answers
	Serial.println (F("Check if we are at the end of the batch"));
	Serial.println (F("Press 1 to continue or 3 to finsh batch:"));
	clean_serial_buffer ();		// Cleans serial buffer from possible unwanted sended keys
	#endif
	block_loop = true;			// This variable prevents from going into a endles loop.
	boolean release = false;
	while (!release || !skip_function()) {
		check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
		switch (server_answer) {
			case button_continue:
				server_answer = 0;
				block_loop = false;
				send_error_to_server (no_error);		// Reset error on the server
				return false;
			break;
			case button_finish:
				// Get ready to finish batch
				release = true;
				server_answer = 0;
				send_error_to_server (no_error);		// Reset error on the server
				// pump_disable ();
				Serial.println(F("Goto print position"));
				go_to_print_position();

				send_status_to_server (S_finishing_batch);
				send_action_to_server (batch_end);
				endingBatch=true;		// This flag will disable all functions so if we need to do anything before ending do it before
				block_loop = false;
				return true;
			break;
			default:
				// Any other answer or 0
				server_answer = 0;
			break;
		}
	}
	// We enter here in case we trigger a main function of a parent loop (skip function)
	send_error_to_server (no_error);		// Reset error on the server
	block_loop = false;
	return false;

	// Serial.println (F("**** BATCH FINISHED! Close this windows and open again to restart *****"));
	
	// start_idle_timer (1500);	// 1500 = 20minutes
	// while (true) {
		//boring_messages ();
	// }
	// end_idle_timer ();		// Will never happens, but just in case...
}

void vibrate_solenoid (byte solenoid_number, byte power, byte duration) {
	// power 1 - 10 its the delay of the inner oscilation, the fastes (lowest number) less powerful
	// the slowest (higher number) stronger the vibration
	for (int c=0; c<duration; c++) {
		digitalWrite (solenoid_number, HIGH);
		delay (power);
		digitalWrite (solenoid_number, LOW);
		delay (power);
	}
}



void calibrate_counter () {
	show_DBservos_data ();
	// Find wheel sensor
	boolean wheel_sensor = false; 
	int count = 0;
	#if defined Cmotor_debug		// In case of debug do nothing so we skip the init 
	wheel_sensor = true;
	#endif
	counter.set_direction (!default_directionC);   	// Set direction

	unsigned int sensor_pickup = 0;
	unsigned int sensor_begin = 0;
	unsigned int sensor_end = 0;
	unsigned int sensor_margin = 0;
	unsigned int sensor_midpoint = 0;
	unsigned int sensor_release = 0;

	////////////////////// Enable power and motor here!!
	
	Serial.println (F("Detecting init position of the wheel..."));
	while (!wheel_sensor) {			// Detect init position of the wheel
		if (count == (1600 / counter.get_step_accuracy())) {	// If after one full turn we haven´t detected the sensor something is wrong
			Serial.println (F("Error detecting wheel sensor"));
			wheel_sensor = true;
		}  
		if (digitalRead(Counter_wheel_sensor)) {	// If the sensor is true means we found the position of the sensor
			wheel_sensor = true;
		}else{
			counter.do_step();		
			delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
		}
		count++;
	}
	counter.set_init_position();		// Set init position

	boolean done = false;

	Serial.println (F("Now go to Pick up position (slightly lower)"));
	Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready"));
	while (!done) {
		if (Serial.available() >0){
			char received = Serial.read ();
			if (received == '1') {
				for (int i =0; i <5; i++) {
					counter.set_direction (default_directionC);   	// Set direction
					counter.do_step();		
					delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
				}
			}
			if (received == '2') {
				for (int i =0; i <5; i++) {
					counter.set_direction (!default_directionC);   	// Set direction
					counter.do_step();		
					delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
				}

			}
			if (received == '3') {
				sensor_pickup = counter.get_steps();
				done = true;
			}
		}
	}

	Serial.println (F("Now we will try to detect the optic sensor of the seeds"));
	Serial.println (F("Be sure to have a seed in position and the pump connected to provided AC adaptor"));
	Serial.print(F("\r\nReady? [y/n]\r\n"));
	if (YN_question ()) {
		// Detect sensor
		Serial.println (F("Detecting sensor... "));
		pump_enable ();
		counter.set_direction (default_directionC);   	// Set direction
		unsigned int temp_max_steps_counter_init = 1300;
		while ((check_seed_sensor() == false) && (counter.get_steps() < temp_max_steps_counter_init)) {
			counter.do_step();		
			delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
			if (counter.get_steps() == temp_max_steps_counter_init) {
				Serial.println(F("Error, max step count reached"));
				break;
			}
		}
		// init sensor detected, record data
		sensor_begin = counter.get_steps();
		Serial.print (F("Sensor begins at position: "));
		Serial.println (sensor_begin);
		while (check_seed_sensor() == true) {
			counter.do_step();		
			delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step	
		}
		// end sensor detected, record data
		sensor_end = counter.get_steps();
		Serial.print (F("Sensor ends at position: "));
		Serial.println (sensor_end);
		sensor_margin = (sensor_end - sensor_begin) / 2;
		sensor_midpoint = sensor_margin + sensor_begin;

		done = false;
		Serial.println (F("Now move the wheel until the seeds is completely released"));
		Serial.println (F("Press 1 to go further, 2 backwards, 3 when ready"));
		while (!done) {
			if (Serial.available()){
				char received = Serial.read ();
				if (received == '1') {
					for (int i =0; i <5; i++) {
						counter.set_direction (default_directionC);   	// Set direction
						counter.do_step();		
						delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
					}
				}
				if (received == '2') {
					for (int i =0; i <5; i++) {
						counter.set_direction (!default_directionC);   	// Set direction
						counter.do_step();		
						delayMicroseconds(motor_speed_counter*5);		// This delay slows down the velocity so we won't miss any step
					}

				}
				if (received == '3') {
					sensor_release = counter.get_steps();
					done = true;
					pump_disable();
				}
			}
		}

		Serial.println (F("* Collected data *"));
		Serial.print (F("Pickup position: "));
		Serial.println (sensor_pickup);
		Serial.print (F("Sensor mid point position: "));
		Serial.println (sensor_midpoint);
		Serial.print (F("Sensor maring: "));
		Serial.println (sensor_margin+20);		// +20 as a safety for bigger seeds
		Serial.print (F("Release position: "));
		Serial.println (sensor_release);

		record_database_DB_counter (sensor_pickup,sensor_midpoint,sensor_margin+20, sensor_release);

		show_DBservos_data ();
		read_database_DB_counter ();	// Read values from DB
		Update_positions_accuracy();

		unsigned int steps_count = (drop_position - pick_seed_position);
		if (!default_directionC) steps_count = -steps_count;
		speed_cntr_Move(steps_count,500,1000,500);	// We go to release position. NOTICE that the acceleration in this case is lower
	}
}


boolean check_seed_sensor () {
	if (analogRead(Counter_seed_sensor) < seed_sensor_sensivity) {
		return true;
	}
	return false;
}

void Update_positions_accuracy () {
	pick_seed_position = pick_seed_position / counter.get_step_accuracy();
	detect_seed_position = detect_seed_position / counter.get_step_accuracy();
	if ((margin_steps_to_detect_seed % counter.get_step_accuracy()) > 0) margin_steps_to_detect_seed = margin_steps_to_detect_seed + counter.get_step_accuracy();
	margin_steps_to_detect_seed = margin_steps_to_detect_seed / counter.get_step_accuracy();
	drop_position = drop_position / counter.get_step_accuracy();
	#if defined DEBUG_counter
	Serial.println (F("\n* Updated positions to match accuray of motor *"));
	Serial.print (F("Pickup position: "));
	Serial.println (pick_seed_position * counter.get_step_accuracy());
	Serial.print (F("Sensor mid point position: "));
	Serial.println (detect_seed_position * counter.get_step_accuracy());
	Serial.print (F("Sensor maring: "));
	Serial.println (margin_steps_to_detect_seed * counter.get_step_accuracy());
	Serial.print (F("Release position: "));
	Serial.println (drop_position * counter.get_step_accuracy());
	#endif
}