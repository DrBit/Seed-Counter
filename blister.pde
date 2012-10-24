// ***********************
// ** Physical limits of the blister motors
// ***********************
#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000
#define steps_to_move_when_blister_falls 800


// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************

boolean blisters_init () {
	int steps_to_do = (blisters_steps_absoulut_limit+100) / blisters.get_step_accuracy();		// The absolut limit would be 1000, but we add an extra 100 to be sure that we hit the maximum point so we init correctly
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 
	blisters.set_init_position();
	return true;
}

// ************************************************************
// ** USEFUL FUNCTIONS
// ************************************************************

void release_blister () {

	Serial.println("go to Blister Position");
	go_to_memory_position (2);			// blister
	
	
	int steps_to_do = blisters_steps_limit / blisters.get_step_accuracy();
	blisters.set_direction (!default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 
	
	/*

	// SHAKE to fit the blister (2 is blister position)
	int tempYcycles = get_cycle_Ypos_from_index(2);
	int tempYsteps = get_step_Ypos_from_index(2); 
	
	Yaxis.got_to_position (tempYcycles, tempYsteps+steps_to_move_when_blister_falls);
	delay (100);
	Yaxis.got_to_position (tempYcycles, tempYsteps-steps_to_move_when_blister_falls);
	delay (100);
	Yaxis.got_to_position (tempYcycles, tempYsteps);
	*/
	
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do + 50; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	}
	
	
	// Check if we are out of blisters
	check_out_of_blisters ();

}


boolean check_blister_realeased () {
	
	boolean p21_correct = false;
	boolean p22_correct = false;
	
	Serial.print("Blister released: ");
	
	
	// First we go right after the blister
	// In this position we should detect the blister, so if we do, something whent wrong
	go_to_memory_position (21);			// check blister (after begining, we shoud read OFF)
	
	boolean sensor_state = digitalRead (sensF); 
	if (!sensor_state) {
		p21_correct = true;
	}else{
		Serial.print(" - OFF state incorrect  ");
	}
		
	// Now we go right ON the blister at the begining.
	// Here we should detect it, if we dont something whent wrong
	go_to_memory_position (22);			// check blister (begining, we should read ON)

	sensor_state = digitalRead (sensF); 
	if (sensor_state) {
		p22_correct = true;
	}else{
		Serial.print(" - ON state incorrect ");
	}
	
	if (p21_correct && p22_correct) {
		print_ok();
		return true;
	}
	
	print_fail ();
	return false;
}


void check_out_of_blisters () {
	// Check if we are out of blisters
	boolean out_of_blsiters = false;
	
	byte sensorC_state = digitalRead (sensC); 
	if (sensorC_state) out_of_blsiters = true;
	while (out_of_blsiters) {
		// We got emty blisters, stop process
		Serial.println("OUT OF Blisters, please refill.");
		
		pump_disable ();
		
		// Check whether the sensor changes state
		while (sensorC_state) {
			sensorC_state = digitalRead (sensC);
			delay (2000);
		}
		
		// Comunicate the user to continue
		Serial.println("Sensor has changed, autostarting in 10 seconds.");
		delay (10000);
		
		// Recheck the sensor if not enabled proceed to continue
		sensorC_state = digitalRead (sensC); 
		if (!sensorC_state) {
			out_of_blsiters = false;
			pump_enable ();
		}
	}
}


void pick_blister_mode() {
	boolean correct_mode = false;
	while (!correct_mode) {
		Serial.println ("* Select blister mode:");
		Serial.println ("[1] - 10 seeds mode");
		Serial.println ("[2] - 5  seeds mode");
		
		int button_pressed = return_pressed_button ();
		if (button_pressed == 1) {
			blister_mode = seeds10;
			correct_mode = true;
			Serial.println ("10 seeds mode delected.");
		} else if (button_pressed == 2) {
			blister_mode = seeds5;
			correct_mode = true;
			Serial.println ("5 seeds mode delected.");
		}else{
			Serial.print (" Mode ");
			Serial.print (button_pressed);
			Serial.println (" not avaialble, try again");
		}
	}	
}

void get_and_release_blister () {

	// If we got already 500 seeds we reset the Y axis as its getting flufy (it shouldn't be like that)
	if (((counter_s%100) == 0) && (counter_s!=0)) {
		// Reset axis

		int temp_err = 0;   // flag for found errors
		if (!init_blocks(2)) temp_err = 1;
		
		while (temp_err > 0) { // We found an error, we chek ALL errors and try to initiate correctly
			temp_err = 0;
			Serial.println("\nErrors found, press 1 when ready to check again, 2 to bypas the errors");
			switch (return_pressed_button ()) {
				//Init XY 
				case 1:
					if (error_XY) {
						if (!init_blocks(2)) temp_err++;
					}
				break;
				
				case 2:
					// do nothing so we wond detect any error and we will continue
				break;
			}
		}
	}
	
	Serial.println("Get blister");
	release_blister ();
	
	boolean released = check_blister_realeased ();
	
	// ONE TIME ONLY
	if (!released) {
		Serial.println("Go to exit");
		go_to_memory_position (4);			// Exit
		
		Serial.println("Get blister");
		release_blister ();
		released = check_blister_realeased ();
	}
	
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
	
}