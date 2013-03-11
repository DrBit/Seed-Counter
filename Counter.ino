// ************************************************************
// ** INIT SEED COUNTER
// ************************************************************
// Inits the seedcounter using the sensor as a starting point

// Define positions of the wheel
// This positions will be in future storen in the UI
unsigned int pick_seed_position = 200;			// Position in which seeds are picked up	
unsigned int detect_seed_position = 600;			// Position in which seeds are detected
unsigned int margin_steps_to_detect_seed = 80;	// Margin in which the seed detector could detect a seed
unsigned int drop_position = 1200;				// Position in which seeds are droped 
		
// Defines max counts that will triger certain errors
unsigned int fails_max_normal = 40;				// Max number of tries to pick a seed before software will create an error
unsigned int fails_max_end = 20;				// Max number of fails before 100 seeds to reach the complet batch to create an error (since we are close to the end we dont need to go to 1000)
unsigned int fails_max_init_tries = 10;   			// Number of times the counter will try to get a seed at INITIATION before giving an error
unsigned int max_batch_count = 1100;			// Tipical number of seeds in a batch

boolean first_time_drop = true;		// Used only to acomodate position after INIT. Once hase been used we won't used anymore.


boolean Seedcounter_init() {		// Init seed counter motors and sensors
	boolean seed_sensor = false; 
	boolean wheel_sensor = false; 
	int count = 0;
	#if defined Cmotor_debug		// In case of debug do nothing so we skip the init 
	seed_sensor = true;
	wheel_sensor = true;
	#endif
	counter.set_direction (!default_directionC);   	// Set direction
    send_action_to_server(counter_init);			// Send info to server
	
	while (!wheel_sensor) {			// Detect init position of the wheel
		if (count == counter.get_steps_per_cycle()) {	// If after one full turn we haven´t detected the sensor something is wrong
			send_error_to_server(counter_init_fail);	// Error in the detection of the wheel. Switch might be disconnected
			return false;			// Failed to initiate seed counter, return false
		}  
		if (digitalRead(sensA)) {	// If the sensor is true means we found the position of the sensor
			wheel_sensor = true;
		}else{
			counter.do_step();		
			delayMicroseconds(motor_speed_counter);		// This delay slows down the velocity so we won't miss any step
		}
		count++;
	}
	counter.set_init_position();		// Set init position

	// Once we set the init position we can go to pick_up_seed position to be ready to start
	int steps_to_do = (pick_seed_position / counter.get_step_accuracy()) +(pick_seed_position % counter.get_step_accuracy());
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
void pickup_seed() {
	motor_select = 0;
	boolean seed_detected = false;
	#if defined DEBUG_counter
	Serial.print (F("We are starting pickup_seed function at position: "));
	Serial.print (counter.get_steps_cycles());
	Serial.print (F(" - "));
	Serial.println (counter.get_steps());
	#endif
	unsigned int count_error_turns =0;
	int accel = 10000;			// Values of the motor acceleration and speed
	int speed = 5200;
	#define case_error_checking 1 	// Values for handling the cases
	#define case_vibrate 2
	#define case_pick_seed_position 3
	#define case_detect_seed_position 4
	#define case_drop_position 5
	byte counting_case = 1;
	int upper_limit = detect_seed_position + margin_steps_to_detect_seed;	// Precalculate some variable to avoid loosing time insde the functions
	int lower_limit = detect_seed_position - margin_steps_to_detect_seed;
	boolean sensor_skip = false;		//Var for debugging
	#if defined Cmotor_debug
	sensor_skip = true; 
	#endif

	while (!seed_detected || !skip_function()) {
		switch (counting_case) {
			case case_error_checking:		// First some error checking.
				if (count_error_turns > fails_max_normal) {				// Checks if we did too many turns. Could mean a bottle neck or just out of seeds
					send_error_to_server(counter_max_turns_normal);
					if (check_if_end_of_batch ()) {
						break;	// Should it say return?
					}
				}else if ((count_error_turns > fails_max_end) && (counter_s > (max_batch_count - 200))) {
					send_error_to_server(counter_max_turns_end);
					if (check_if_end_of_batch ()) {
						break;	// Should it say return?
					}
				}
				count_error_turns = 0;
				counting_case = case_pick_seed_position;	// Pick up a seed
			break;

			case case_vibrate:
				// Here could be implemented the laser function
				// if we havent reached the pick position we can still vibrate
				// here we can also implement de laser sensor so we have complete control over the seeds position
				if (counter.get_steps() != pick_seed_position) {
					// time function can be implemented to avoid shaking too much
					vibrate_solenoid(solenoid1,3,5);
					delay (300);
				}else{
					counting_case = case_error_checking;
				}
			break;

			case case_pick_seed_position:
				if (counter.get_steps() == pick_seed_position) {
					send_action_to_server(seed_counter_turn);	// Send info to the server
					unsigned int steps_count = ((drop_position - pick_seed_position) / counter.get_step_accuracy());
					if (default_directionC) steps_count = -steps_count;
					speed_cntr_Move(steps_count,accel,speed,accel);	// We go to release position. NOTICE that the acceleration in this case is lower
					counting_case = case_detect_seed_position;
				}else{
					#if defined DEBUG_counter
					Serial.print (F("We are at: "));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
				}
			break;

			case case_detect_seed_position:
				if ((counter.get_steps() >= lower_limit) && (counter.get_steps() <= upper_limit)) {	// We check the sensor only when we are in the range of the sensor
					if (counter.sensor_check() || sensor_skip){		// We got a seed!!!
						seed_detected = true; 	// Mark that we got a seed
						counter_s ++;			// For statistics pourpouse
						counting_case = case_drop_position; // Go to release seed
					}
				}else{
					if (counter.sensor_check() && !sensor_skip){	// We detected a seed We are not in sensor position
						check_faulty_sensor ();	// Reset wheel 
					}
				}
				if (counter.get_steps() == drop_position) counting_case = case_drop_position; // If we are here we did not detect a seed and we already reached drop zone
			break;

			case case_drop_position:
				if (counter.get_steps() == drop_position) {
					if (seed_detected) {
						send_action_to_server(seed_released);
					}else{
						send_action_to_server(seed_missed);
					}
					count_total_turns++;
					unsigned int steps_count = ((drop_position - pick_seed_position) / counter.get_step_accuracy());
					if (!default_directionC) steps_count = -steps_count;
					speed_cntr_Move(steps_count,accel,speed,accel);	// We go to release position. NOTICE that the acceleration in this case is lower
					counting_case == case_vibrate;	// We go to vibration case
				}else{
					#if defined DEBUG_counter
					Serial.print (F("We are at: "));
					Serial.print (counter.get_steps_cycles());
					Serial.print (F(" - "));
					Serial.println (counter.get_steps());
					#endif
				}
			break;
		}
	}
}

void check_faulty_sensor () {
	send_error_to_server(counter_sensor_failed);
	#if defined serial_answers
	Serial.println (F("Error in wheel. Missed steps? Sensor detected assed where it should not be"));
	Serial.println (F("Check sensors and press one to reset wheel"));
	Serial.println (F("Press 1 to continue:"));
	clean_serial_buffer ();		// Cleans serial buffer from possible unwanted sended keys
	#endif
	boolean released = false;
	block_loop = true;
	while (!released || !skip_function()) {
		check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
		switch (server_answer) {
			case button_continue:
				Serial.println(F("Continue button pressed"));
				if (Seedcounter_init ()) {
					released = true;
					send_error_to_server (no_error);		// Reset error on the server
				}
				server_answer = 0;
			break;

			default:
				// Any other answer or 0
				server_answer = 0;
			break;
		}
	}
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

/*
// OLD Seed counter init
boolean Seedcounter_init() {
    send_action_to_server(counter_init);
	boolean seed_sensor = false; 
	int count = 0;
#if defined Cmotor_debug
	// Do nothing so we skip the init 
	seed_sensor = true;
#endif
	counter.set_direction (default_directionC);   // Set direction
	while (!seed_sensor) {
		// If the vacuum is not on, this would be cheking for the 0 position foreve
		// so we count ten times and if there is no sense of a seed we pause
		count++;
		if (count == (counter.get_steps_per_cycle() * init_turns_till_error)) {  
			send_error_to_server(counter_init_fail);
			// Counter error, pump might be off, seeds deposits might be empty, sensor might be disconnected or broken
			return false;  // Failed to initiate seed counter, retunr false
		}  
		if (!counter.sensor_check()) {
			counter.do_step();
		}
		if (counter.sensor_check()) {
			// If the sensor is true means we found the position of the sensor
			seed_sensor = true;
			//Serial.println ("Counter sensor is HIGH");  // for debug
		}
		// This delay slows down the velocity so we won't miss any step
		// Thats because we are not using acceleration in this case
		delayMicroseconds(motor_speed_counter);
	}
	for (int i=0;i<9; i++) {	// Since the detection of the seed is just at the edge of the same seed we do	
								// 9 steps further to be in the middle of the sensor
		//delay (1000);			// Just for testing if its correct
		counter.do_step();
		delayMicroseconds(motor_speed_counter);
	}

	counter.set_init_position();  

	// CHOOSE BETWEEN STANDARD AND EXPERIMENTAL INIT
	if (false) {
		first_time_drop = true;		// State that the first time we drop a seed has to be different because we just INIT and the wheel is in a different posuition than by default
	}else{
		// go back to the first place
		int steps_to_do = (1600 - steps_from_sensor_to_init_clockwise) / counter.get_step_accuracy();
		counter.set_direction (!default_directionC);   // Set direction
		#if defined DEBUG_counter
		Serial.print (F("Steps to do to go to pick up seed point: "));
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
		first_time_drop = true;
	}

	return true;
}
*/


/*
// OLD Seed counter pickup seed 

// ************************************************************
// ** Extra functions
// ************************************************************
// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
void pickup_seed() {
	motor_select = 0;
	boolean seed_detected = false;
	#if defined DEBUG_counter
	Serial.print (F("Init position: "));
	Serial.print (counter.get_steps_cycles());
	Serial.print (F(" - "));
	Serial.println (counter.get_steps());
	#endif
	unsigned int previous_counted_turns = count_total_turns;				// Avoid giving more than 1 order to turn at the same time
	unsigned int count_error_turns =0;
	// Values of the motor acceleration and speed
	int accel = 10000;
	int speed = 5200;
	
	while (!seed_detected && !skip_function()) {
		
		// First some error checking.
		
		// Prepare code to do the last count if we are at 1 count to trigger error
		int last_turn = false;
		if (count_error_turns == fails_max_normal) {
			last_turn = true;
		}else if ((count_error_turns == fails_max_end) && (counter_s > (max_batch_count - 200))) {
			last_turn = true;
		}
		
		// Checks if we did too many turns. Could mean a bottle neck or just out of seeds
		if (count_error_turns > fails_max_normal) {
			send_error_to_server(counter_max_turns_normal);
			if (end_of_batch ()) {
				break;
			}else{
				count_error_turns = 0;
				previous_counted_turns = count_total_turns;
				send_error_to_server(no_error);
				// server_answer
			}
		}else if ((count_error_turns > fails_max_end) && (counter_s > (max_batch_count - 200))) {
			send_error_to_server(counter_max_turns_end);
			if (end_of_batch ()) {
				break;
			}else{
				count_error_turns = 0;
				previous_counted_turns = count_total_turns;
				send_error_to_server(no_error);
			}
		}

		// Vibrate acordingly depending on the amount of previous errors
		byte intensity = count_error_turns%10; 
		//duration is 10 - intensity
		byte _duration = 10 - intensity;
		//vibrate_solenoid (solenoid1, intensity, _duration);	// intensity will be a number of 0 to 10
		

		// We are at drop seed position ready to start turning.
		if ((counter.get_steps() == steps_from_sensor_to_init_clockwise) && (count_total_turns == previous_counted_turns)){			// If we are at the starting position means we are ready to continue
			check_status (false);				// Enters menu if a button is pressed
			// We check for pause in here because here we are time safe. Outside this is time criticall and any delay would crash de counter
			wait_time(50);
			if (!last_turn) {
				send_action_to_server(seed_counter_turn);
				if (default_directionC) {
					speed_cntr_Move(-(1600/counter.get_step_accuracy()),accel,speed,accel);	// We do a full turn, NOTICE that the acceleration in this case is lower
				}else{
					speed_cntr_Move(1600/counter.get_step_accuracy(),accel,speed,accel);	// We do a full turn, NOTICE that the acceleration in this case is lower

				}
				count_total_turns ++;		// for statistics pourpouses
				
			}
			count_error_turns ++;		// for errors pourpous
			
		}else if (first_time_drop && (count_total_turns == previous_counted_turns)) {				// If its the first time we wont be at the starting position so instead of a full turn we move less to arrive at the default pos.
			first_time_drop = false;
			wait_time(50);
			if (!last_turn) {
				send_action_to_server(seed_counter_turn);
				if (default_directionC) {
					speed_cntr_Move(-(1600/counter.get_step_accuracy()),accel,speed,accel);	// We do a full turn, NOTICE that the acceleration in this case is lower
				} else {
					// speed_cntr_Move(steps_from_sensor_to_init_clockwise/counter.get_step_accuracy(),accel,speed,accel);	// We do a full turn, NOTICE that the acceleration in this case is lower
					speed_cntr_Move(1600/counter.get_step_accuracy(),accel,speed,accel);	// We do a full turn, NOTICE that the acceleration in this case is lower
				}
				count_total_turns ++;		// for statistics pourpouses
			}
			count_error_turns ++;		// for errors pourpous
		}
			
		if ((counter.get_steps() >= (1600-margin_steps_to_detect_seed)) || (counter.get_steps() <= margin_steps_to_detect_seed)) {				// We check the sensor only when we are in the range of the sensor
			// Fake sensor in case of debug motors
			boolean sensor_skip = false;
			#if defined Cmotor_debug
				sensor_skip = true; 
			#endif
			// Check if we got seed
			if (counter.sensor_check() || sensor_skip){			// We got a seed!!!
				seed_detected = true; 
				counter_s ++;						// For statistics pourpouse
				while (!(counter.get_steps() == (steps_from_sensor_to_init_clockwise-steps_from_sensor_to_start_moving_when_seed)))	// If we are not finished moving...
				{
					// Do nothing and wait
					// We are waiting to reach a position where the seed is about to fall. At that speed the motor will start acelerating while the seeds
					// lower the steps_from_sensor_to_start_moving_when_seed in order to wait more to start moving the X or Y motors before the seed falls.
				}
				send_action_to_server(seed_released);
			}
			// Each time we are here is because we already started moving
			previous_counted_turns = count_total_turns;			// Avoid giving more than 1 order to turn at the same time
			
		} else {
			// Fake sensor in case of debug motors
			boolean sensor_skip = false;
			#if defined Cmotor_debug
				sensor_skip = true; 
			#endif
			if (counter.sensor_check() && !sensor_skip){			// We got a seed and we are not supose to have one here!!
				send_error_to_server(counter_sensor_failed);
				boolean released = false;
				block_loop = true;
				while (!released) {
					check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
					if (!skip_function()) {			// In case we pressed restart or another high we will skip everything and continue to a safe position.
						switch (server_answer) {
							case button_continue:
								Serial.println(F("Continue button pressed"));
								if (counter_autofix ()) {
									released = true;
									send_error_to_server (no_error);		// Reset error on the server
								}
								server_answer = 0;
							break;

							default:
								// Any other answer or 0
								server_answer = 0;
							break;
						}
					}else{
						// We enter here in case we trigger a main function of a parent loop
						send_error_to_server (no_error);		// Reset error on the server
						released = true;
					}
				}
				block_loop = false;
			}
			
		}
		// Check if we are at sensor position
		#if defined DEBUG_counter
			Serial.print (F("Init position: "));
			Serial.print (counter.get_steps_cycles());
			Serial.print (F(" - "));
			Serial.println (counter.get_steps());
		#endif
	}
}
*/


/*
// functions not needed anymore 

void wait_time (unsigned long milliseconds) {
	unsigned long now =0;
	now = millis();
	while (millis() < (now + milliseconds)) {
		// Just wait
	}
}

boolean counter_autofix() {
    send_action_to_server (try_counter_autofix);
	// Goes back till senses and then senses nothing again or just turns back little but more than half a turn
	counter.set_direction (!default_directionC);   // Set direction
	
	int steps_limit = ((counter.get_steps_per_cycle() / 4)*3) * counter.get_step_accuracy();
	
	boolean pre_init_position = false;
	int count = 0;
	boolean seed_sensor = false; 
	
	while (!pre_init_position) {
		
		counter.do_step();
		count++;
		delayMicroseconds(motor_speed_counter*5);		// we do it 5 times slower to avoid breaking anything
		
		if (counter.sensor_check()) {
			// We have detected the sensor, we go further till going out, and little further to be in a safe place
			seed_sensor = true;
			while (seed_sensor) {
				seed_sensor = counter.sensor_check();
				counter.do_step();
				delayMicroseconds(motor_speed_counter*5);		// we do it 5 times slower to avoid breaking anything
			}
			
			for (int a = 0; a < margin_steps_to_detect_seed; a++) {\
				counter.do_step();
				delayMicroseconds(motor_speed_counter*5);		// we do it 5 times slower to avoid breaking anything
			}
			
			pre_init_position = true;
		} else {
			if (count >= steps_limit) {
				pre_init_position = true;
			}
		}
	}

	Seedcounter_init();
}
*/