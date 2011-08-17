// ************************************************************
// ** INIT SEED COUNTER
// ************************************************************
// Inits the seedcounter using the sensor as a starting point
// The sensor should be always at the same position in all the seedcunters and the init position hardcoded based on the position of the sensor.
boolean Seedcounter_init() {
	boolean seed_sensor = false; 
	int count = 0;

	// First time we init we just go back one complete cycle in case we have seeds atached and we bring them into the deposit
	counter.set_direction (true);   // Set direction
	for (int a=1600; a > 0; a--) {
		counter.do_step();
		delayMicroseconds(motor_speed);
	}

	counter.set_direction (false);   // Set direction
	while (!seed_sensor) {
		// If the vacuum is not on, this would be cheking for the 0 position foreve
		// so we count ten times and if there is no sense of a seed we pause
		count++;
		if (count == (800*10)) {  
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
		delayMicroseconds(motor_speed);
	}
	for (int i=0;i<10; i++) {   // Since the detection of the seed is just at the edge of the same seed we do	
								// 10 steps further to be in the middle of the sensor
		counter.do_step();
		delayMicroseconds(motor_speed);
	}
	counter.set_init_position();  

	counter.set_direction (true);   // Set direction
	for (int i=0;i<400; i++) {   // we go back at the position we should be for starting point
		counter.do_step();
		delayMicroseconds(motor_speed);
	}
	counter.set_direction (true);   // Set direction
	return true;
}

// ************************************************************
// ** Extra functions
// ************************************************************
// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
void pickup_seed_extended () {
  motor_select = 0;
  boolean seed_detected = false;
  // IMLEMENT BOTH HOLES of the vacuum hole ... see below
  Serial.print (counter.get_steps_cycles());
  Serial.print (" - ");
  Serial.println (counter.get_steps());
  while (!seed_detected) {
    if ((counter.get_steps() == 1200) || (counter.get_steps() == 400)  || (counter.get_steps() == -400)) {
      delay (150);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
      speed_cntr_Move(-800,5500,19000,19000);  // We do a half turn, NOTICE that the acceleration in this case is lower
                                                // Thats to avoid trowing seeds and to achieve a better grip on the seed
    }
    if ((counter.get_steps() == 1600) || (counter.get_steps() == 800) || (counter.get_steps() == 0)) {   // We chek the sensor only when we are in the range of the sensor
      if (counter.sensor_check()){                 // We got a seed!!!
          seed_detected = true; 
          while (!(counter.get_steps() == 1200) && !(counter.get_steps() == 400))
          {
            //delay (100); // Wait for the seed to fall
          }
      }
    }
  
  }
}


