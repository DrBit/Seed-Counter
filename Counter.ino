// ************************************************************
// ** INIT SEED COUNTER
// ************************************************************
// Inits the seedcounter using the sensor as a starting point

// #define steps_from_sensor_to_init 1200  	// Number of steps (based in mode 8) to go backward from the sensor to the init position (not used)
#define steps_from_sensor_to_init_clockwise 1150  			// Number of steps (based in mode 8) to go forward from the sensor to the init position
#define steps_from_sensor_to_start_moving_when_seed 0		// Number of steps (based in mode 8) away fro 	m the pick a seed point to start moving the axis when we got a seed.
#define margin_steps_to_detect_seed 80		// Its the steps margin in wich the sensor will check if we have a seed

#define fails_max_normal 40				// Max number of tries to pick a seed before software will create an error
#define fails_max_end 20				// Max number of fails before 100 seeds to reach the complet batch to create an error (since we are close to the end we dont need to go to 1000)
#define init_turns_till_error 40   		// Number of times the counter will try to get a seed at INITIATION before giving an error
unsigned int max_batch_count = 1100;	// Tipical number of seeds in a batch

boolean first_time_drop = true;		// Used only to acomodate positionafter INIT. Once hase been used we won't used anymore.

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
			//Serial.println("NOT in sensor pos");
			#if defined Cmotor_debug
				sensor_skip = true; 
			#endif
			if (counter.sensor_check() && !sensor_skip){			// We got a seed and we are not supose to have one here!!
				send_error_to_server(counter_sensor_failed);
				// Something whent wrong!!!!
				Serial.println (F("\n\n **** Something whent wrong. Detected seed where it shouldn't"));
				Serial.println (F(" Probably counter motor missed some steps"));
				Serial.println (F("\n ***Press 1 to try auto-fix or reset the machine."));
				Serial.println (F("\n ***CHECK CURRENT BLISTER FOR DOUBLE SEEDS OR EMPTY SEEDS!"));
				int button_pressed = return_pressed_button ();
				if (button_pressed == 1) {
					counter_autofix ();
					send_error_to_server(no_error);
					previous_counted_turns = count_total_turns;			// Avoid giving more than 1 order to turn at the same time
					first_time_drop = true;
				}
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

boolean end_of_batch () {
	block_loop = true;			// This variable prevents from going into a endles loop.
	boolean release = false;
	while (!release) {
		
		check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
		if (!skip_function()) {			// In case we pressed restart or another high we will skip everything and continue to a safe position.
			switch (server_answer) {

				case button_continue:
					server_answer = 0;
					block_loop = false;
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
		}else{
			// We enter here in case we trigger a main function of a parent loop (skip function)
			send_error_to_server (no_error);		// Reset error on the server
			release = true;
			block_loop = false;
			return false;
		}
	}
	block_loop = false;

	// Serial.println (F("**** BATCH FINISHED! Close this windows and open again to restart *****"));
	
	// start_idle_timer (1500);	// 1500 = 20minutes
	// while (true) {
		//boring_messages ();
	// }
	// end_idle_timer ();		// Will never happens, but just in case...
}

