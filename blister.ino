

// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************

boolean blisters_init () {
	read_database_DB_servo ();
	if (init_hooks_servos() && init_ejection_servos()) {
		return true;
	}
	return false;
}

boolean init_hooks_servos () {
	// Init blister hooks
	for (int l = 0; l<100; l++) {
		blister_servoL.write(servoL_close);                  // sets the servo position according to the scaled value 
		blister_servoR.write(servoR_close);
		delay(5);                           // waits for the servo to get there 
		SoftwareServo::refresh();
	}
	return true;
}


boolean init_ejection_servos () {
	// Init blister ejection
	for (int l = 0; l<100; l++) {
		ejection_servo.write(servoEjection_close);                  // sets the servo position according to the scaled value 
		delay(5);                           // waits for the servo to get there 
		SoftwareServo::refresh();
	}
	return true;
}



// ************************************************************
// ** USEFUL FUNCTIONS
// ************************************************************

void release_blister_servo () {

	go_to_memory_position (2);			// blister

	blister_servoL.write(servoL_open);                  // sets the servo position according to the scaled value 
	blister_servoR.write(servoR_open);
	
	for (int l = 0; l<90; l++) {                   				
		SoftwareServo::refresh();
		delay(6); 
	}

	//delay (300);

	blister_servoL.write(servoL_close);                  // sets the servo position according to the scaled value 
	blister_servoR.write(servoR_close);
	delay(5);                           // waits for the servo to get there 
	for (int l = 0; l<90; l++) {
		SoftwareServo::refresh();
		delay(6); 
	}

	send_action_to_server (blister_release);
}

void eject_blister () {
	if (!skip_function()) {
		PSupply_ON ();

		// Shold we check if we are at the right position?
		for (int l = 0; l<70; l++) {
			ejection_servo.write(servoEjection_open);           // sets the servo position according to the scaled value 
			delay(5);                           				// waits for the servo to get there 
			SoftwareServo::refresh();
		}

		for (int l = 0; l<70; l++) {
			ejection_servo.write(servoEjection_close);          // sets the servo position according to the scaled value 
			delay(5);                           				// waits for the servo to get there 
			SoftwareServo::refresh();
		}

		send_action_to_server (blister_ejected);
	}
}


bool check_blister_realeased () {
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
		p21_correct = false;
	}
		
	// Now we go right ON the blister at the begining.
	// Here we should detect it, if we dont something whent wrong
	go_to_memory_position (22);			// check blister (begining, we should read ON)

	sensor_state = digitalRead (SensBlister); 
	if (sensor_state || skip_sensor_blister) {
		p22_correct = true;
	}else{
		Serial.print(" - ON state incorrect ");
		p21_correct = false;
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
		release_blister_servo ();
		
		// boolean released = check_blister_realeased ();
		boolean released = true;

		block_loop = true;
		while (!released) {
			
			check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
			if (!skip_function()) {			// In case we pressed restart or another high we will skip everything and continue to a safe position.
				switch (server_answer) {

					case button_continue:

						Serial.println("Eject Blister");
						go_to_eject_blister ();
						eject_blister ();
						
						Serial.println("Get blister");
						release_blister_servo ();
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
		}
		block_loop = false;
	}
}


void calibrate_blister_hooks () {
	PSupply_ON ();
	
	show_DBservos_data ();
	Serial.println (F("Calibrating servos of blister"));

	// Starting with Closing positions
	boolean closing_done = false;
	while (!closing_done){
		//look for closed left position
		Serial.println (F("Move left servo until you reach closed position"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 reset"));
		boolean done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoL_close ++;
					blister_servoL.write(servoL_close);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoL_close --;
					blister_servoL.write(servoL_close);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4'){
					servoL_close = 0;
				}
			}
			SoftwareServo::refresh();
		}

		//look for closed right position
		Serial.println (F("Move right servo until you reach closed position"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 reset"));
		done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoR_close ++;
					blister_servoR.write(servoR_close);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoR_close --;
					blister_servoR.write(servoR_close);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4'){
					servoR_close = 0;
				}
			}
			SoftwareServo::refresh();
		}

		Serial.println (F("Is your closing position correct? [Y/N]"));
		if (YN_question ()) {
			closing_done =true;
		}
	}

	// Opening positions
	boolean opening_done = false;
	while (!opening_done){
		//look for open left position
		Serial.println (F("Move left servo until you reach OPEN position"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 reset"));
		boolean done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoL_open++;
					blister_servoL.write(servoL_open);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoL_open --;
					blister_servoL.write(servoL_open);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4'){
					servoL_open = 0;
				}
			}
			SoftwareServo::refresh();
		}

		//look for open right position
		Serial.println (F("Move right servo until you reach OPEN position"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 reset"));
		done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoR_open ++;
					blister_servoR.write(servoR_open);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoR_open --;
					blister_servoR.write(servoR_open);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4') {
					servoR_open = 0;
				}
			}
			SoftwareServo::refresh();
		}

		Serial.print (F("servoR_open: "));
		Serial.println (servoR_open);
		Serial.print (F("servoL_open: "));
		Serial.println (servoL_open);

		Serial.print (F("servoR_close: "));
		Serial.println (servoR_close);
		Serial.print (F("servoL_close: "));
		Serial.println (servoL_close);

		Serial.println (F("Is your OPEN position correct? [Y/N]"));
		if (YN_question ()) {
			Serial.println (F("Do you want to record it into the DB? [Y/N]"));
			if (YN_question ()) {
				record_blister_servo_DB(servoR_open,servoL_open,servoR_close,servoL_close);
				Serial.println (F("Recorded!"));
			}
			opening_done =true;
			show_DBservos_data ();
			init_hooks_servos ();
		}
	}
}

void calibrate_ejection_hooks () {
	PSupply_ON ();

	show_DBservos_data ();

	Serial.println (F("Calibrating ejection servo"));

	// Starting with Closing positions
	boolean closing_done = false;
	while (!closing_done){
		//look for closed
		Serial.println (F("Move ejection servo until you reach a normaly closed position"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 to reset"));
		boolean done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoEjection_close ++;
					ejection_servo.write(servoEjection_close);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoEjection_close --;
					ejection_servo.write(servoEjection_close);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4') {
					servoEjection_close = 0;
				}
			}
			SoftwareServo::refresh();
		}

		//look for open
		Serial.println (F("Move ejecton servo until you reach fully ejected blsiter"));
		Serial.println (F("Press 1 to go forward, 2 backwards, 3 when ready, 4 to reset"));
		done = false;
		while (!done) {
			if (Serial.available() >0){
				char received = Serial.read ();
				if (received == '1') {
					servoEjection_open ++;
					ejection_servo.write(servoEjection_open);            // sets the servo position according to the scaled value 
				}
				if (received == '2') {
					servoEjection_open --;
					ejection_servo.write(servoEjection_open);            // sets the servo position according to the scaled value 
				}
				if (received == '3') {
					done = true;
				}
				if (received == '4') {
					servoEjection_open = 0;
				}
			}
			SoftwareServo::refresh();
		}

		Serial.print (F("Open state: "));
		Serial.println (servoEjection_open);
		Serial.print (F("Close state: "));
		Serial.println (servoEjection_close);

		Serial.println (F("Are you happy with the positions? [Y/N]"));
		if (YN_question ()) {
			Serial.println (F("Do you want to record it into the DB? [Y/N]"));
			if (YN_question ()) {
				record_ejection_servo_DB(servoEjection_open,servoEjection_close);
				Serial.println (F("Recorded!"));
			}
			closing_done =true;
			show_DBservos_data ();
			init_ejection_servos ();
		}
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

//OLD not used anymore
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

*/