// ************************************************************
// ** INIT SEED COUNTER
// ************************************************************
// Inits the seedcounter using the sensor as a starting point

// #define steps_from_sensor_to_init 1200  	// Number of steps (based in mode 8) to go backward from the sensor to the init position (not used)
#define steps_from_sensor_to_init_clockwise 1220  	// Number of steps (based in mode 8) to go forward from the sensor to the init position
#define margin_steps_to_detect_seed 30		// Its the steps margin in wich the sensor will check if we have a seed

#define fails_max_normal 400			// Max number of tries to pick a seed before software will create an error
#define fails_max_end 100				// Max number of fails before 100 seeds to reach the complet batch to create an error (since we are close to the end we dont need to go to 1000)
#define init_turns_till_error 10   		// Number of times the counter will try to get a seed at INITIATION before giving an error

boolean first_time_drop = true;		// Used only to acomodate positionafter INIT. Once hase been used we won't used anymore.

boolean Seedcounter_init() {
        
	boolean seed_sensor = false; 
	int count = 0;

	/*
	// First time we init we just go back one complete cycle in case we have seeds atached and we bring them into the deposit
	counter.set_direction (true);   // Set direction
	for (count = (counter.get_steps_per_cycle()); count > 0; count--) {
		counter.do_step();
		delayMicroseconds(motor_speed_counter);
	} */

	count =0;
	counter.set_direction (false);   // Set direction
	while (!seed_sensor) {
		// If the vacuum is not on, this would be cheking for the 0 position foreve
		// so we count ten times and if there is no sense of a seed we pause
		count++;
		if (count == (counter.get_steps_per_cycle() * init_turns_till_error)) {  
			//send_error("c1");     still to implemetn an error message system
			// Counter error, pump might be off, seeds deposits might be empty, sensor might be disconnected or broken
			return false;  // Failed to initiate seed counter, retunr false
		}  
		if (!counter.sensor_check()) {
			counter.do_step();
		}
		if (counter.sensor_check()) {
			// If the sensor is true means we found the position of the sensor
			seed_sensor = true;
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
	/*
	counter.set_direction (true);   // Set direction
	for (int i=0;i<(steps_from_sensor_to_init/counter.get_step_accuracy()); i++) {   // we go back at the position we should be for starting point
		counter.do_step();
		delayMicroseconds(motor_speed_counter);
	}
	counter.set_direction (true);   // Set direction
	*/
	first_time_drop = true;		// State that the first time we drop a seed has to be different because we just INIT and the wheel is in a different posuition than by default
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
	unsigned int error_counter = 0;
#if defined DEBUG
	Serial.print ("Init position: ");
	Serial.print (counter.get_steps_cycles());
	Serial.print (" - ");
	Serial.println (counter.get_steps());
#endif
	while (!seed_detected) {
		
		if (error_counter > fails_max_normal) {
			Serial.println ("Error, no more seeds. Empty? Bottleneck?");
			pause = true;
			check_pause ();				// Enters menu if a button is pressed
		}else if ((error_counter > fails_max_end) && (counter_s > (max_batch_count - 100))) {
			Serial.println ("We might have reached the end");
			pause = true;
			check_pause ();				// Enters menu if a button is pressed
		}
		check_pause ();				// Enters menu if a button is pressed
		
		if (counter.get_steps() == steps_from_sensor_to_init_clockwise){			// If we are at the starting position means we are ready to continue
			delay (200);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
			speed_cntr_Move(1600/counter.get_step_accuracy(),5500,9000,5500);	// We do a full turn, NOTICE that the acceleration in this case is lower
			count_total_turns ++;		// for statistics pourpous
			error_counter++;			// for error pourpouses
			// Thats to avoid trowing seeds and to achieve a better grip on the seed
		}else if (first_time_drop) {				// If its the first time we wont be at the starting position so instead of a full turn we move less toa rrive at the default pos.
			first_time_drop = false;
			delay (200);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
			speed_cntr_Move(steps_from_sensor_to_init_clockwise/counter.get_step_accuracy(),5500,9000,5500);	// We do a full turn, NOTICE that the acceleration in this case is lower
			count_total_turns ++;		// for statistics pourpous
			error_counter++;			// for error pourpouses
		}
		if ((counter.get_steps() >= (1600-margin_steps_to_detect_seed)) || (counter.get_steps() <= margin_steps_to_detect_seed)) {				// We check the sensor only when we are in the range of the sensor
			if (counter.sensor_check()){			// We got a seed!!!
				seed_detected = true; 
				counter_s ++;						// For statistics pourpouse
				while (!(counter.get_steps() == steps_from_sensor_to_init_clockwise))	// If we are not finished moving...
				{
					// Do nothing and wait
				}
			}
		}
	}
}

