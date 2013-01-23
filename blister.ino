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
	send_action_to_server (blisters_disp_init);
	int steps_to_do = (blisters_steps_absoulut_limit+100) / blisters.get_step_accuracy();		// The absolut limit would be 1000, but we add an extra 100 to be sure that we hit the maximum point so we init correctly
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 
	blisters.set_init_position();

	// Servo init
	for (int l = 0; l<100; l++) {
		myservo_left.write(180);             // sets the servo position according to the scaled value 
		myservo_right.write(0);
		delay(15);                           // waits for the servo to get there 
		SoftwareServo::refresh();
	}
	return true;
}

// ************************************************************
// ** USEFUL FUNCTIONS
// ************************************************************

void release_blister () {
	send_action_to_server (blister_release);
	Serial.println("go to Blister Position");
	go_to_memory_position (2);			// blister
	
	
	int steps_to_do = blisters_steps_limit / blisters.get_step_accuracy();
	blisters.set_direction (!default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 
	
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do + 50; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	}

	// Check if we are out of blisters
	// check_out_of_blisters ();

}

void release_blister_servo () {
	//send_action_to_server (blister_release);
	Serial.println("TESTING SERVOOOOO");
	go_to_memory_position (2);			// blister
	
	for (int l = 0; l<100; l++) {
	myservo_left.write(0);                  // sets the servo position according to the scaled value 
	myservo_right.write(180);
	delay(15);                           // waits for the servo to get there 
	SoftwareServo::refresh();
	}

	for (int l = 0; l<100; l++) {
	myservo_left.write(180);                  // sets the servo position according to the scaled value 
	myservo_right.write(0);
	delay(15);                           // waits for the servo to get there 
	SoftwareServo::refresh();
	}
	// delay(15);
	// Check if we are out of blisters
	//check_out_of_blisters ();

}


boolean check_blister_realeased () {
	boolean skip_sensor_blister = false;
#if defined Sensor_blister
	skip_sensor_blister = true;
#endif
	
	
	boolean p21_correct = false;
	boolean p22_correct = false;
	
	Serial.print("Blister released: ");
	
	
	// First we go right after the blister
	// In this position we should detect the blister, so if we do, something whent wrong
	go_to_memory_position (21);			// check blister (after begining, we shoud read OFF)
	
	boolean sensor_state = digitalRead (SensBlister); 
	if (!sensor_state || skip_sensor_blister) {
		p21_correct = true;
	}else{
		Serial.print(" - OFF state incorrect  ");
	}
		
	// Now we go right ON the blister at the begining.
	// Here we should detect it, if we dont something whent wrong
	go_to_memory_position (22);			// check blister (begining, we should read ON)

	sensor_state = digitalRead (SensBlister); 
	if (sensor_state || skip_sensor_blister) {
		p22_correct = true;
	}else{
		Serial.print(" - ON state incorrect ");
	}
	
	if (p21_correct && p22_correct) {
		print_ok();
		return true;
	}
	
	print_fail ();
	send_error_to_server (blister_release_fail);
	return false;
}


void check_out_of_blisters () {
	boolean skip_sensor_blister = false;
#if defined Sensor_blister
	skip_sensor_blister = true;
#endif

	// Check if we are out of blisters
	boolean out_of_blsiters = false;
	
	byte sensorC_state = digitalRead (SensOutBlisters); 
	if (sensorC_state && !skip_sensor_blister) out_of_blsiters = true;
	while (out_of_blsiters) {
		// We got emty blisters, stop process
		Serial.println("OUT OF Blisters, please refill.");

		send_action_to_server (needed_blisters_refill);
		send_status_to_server (S_pause);
		pump_disable ();
		
		// Check whether the sensor changes state
		while (sensorC_state) {
			sensorC_state = digitalRead (SensOutBlisters);
			delay (2000);
		}
		
		// Comunicate the user to continue
		Serial.println("Sensor has changed, autostarting in 10 seconds.");
		delay (10000);
		
		// Recheck the sensor if not enabled proceed to continue
		sensorC_state = digitalRead (SensOutBlisters); 
		if (!sensorC_state) {
			out_of_blsiters = false;
			send_action_to_server (blister_refilled);
			send_status_to_server (S_running);
			pump_enable ();
		}
	}
}


void get_and_release_blister () {
	check_status(false);
	if (!skip_function()) {
		Serial.println("Get blister");
		release_blister ();
		
		boolean released = check_blister_realeased ();

		block_loop = true;
		while (!released) {
			
			check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
			if (!skip_function()) {			// In case we pressed restart or another high we will skip everything and continue to a safe position.
				switch (server_answer) {

					case button_continue:

						Serial.println("Eject Blister");
						eject_blister ();
						
						Serial.println("Get blister");
						release_blister ();
						released = check_blister_realeased ();
						if (released) send_error_to_server (no_error);		// Reset error on the server
						server_answer = 0;
					break;
					
					case button_ignore:
						// do nothing so we wond detect any error and we will continue
						released = false;
						server_answer = 0;
						send_error_to_server (no_error);		// Reset error on the server
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
			delay (1000);
		}
		block_loop = false;
	}
}



/*
void pick_blister_mode() {

	// This information will be provided by the server
	// send_config_from_server (get_seeds_mode);
	
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
*/