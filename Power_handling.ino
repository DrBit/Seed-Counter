/////////////////////////
// Main functions
/////////////////////////

void init_all_motors () {
	block_loop = true;
	// Prepare to init motors
	PSupply_ON ();		// Switch Power supply ON
	motors_enable ();	// Enable motors
	motors_awake ();	// Awake motors

	// INIT SYSTEM, and CHECK for ERRORS
	int temp_err = 0;   // flag for found errors
	if (!init_blocks(ALL)) temp_err = 1;
	
	// In case of error  
	while (temp_err > 0) { // We found an error, we chek ALL errors and try to initiate correctly
		// Serial.println("\nErrors found, press 1 when ready to check again, 2 to bypas the errors");
		// We can add also buttons to answer manually to the errors
		// Errors have already been sended to the server so we don have to take care of that
		// Now we just respond to the (custom) answers of the server, general answers will be taken inside check_stop function 
		check_server ();		// Here we can not do a check stop cause we would go into an endless loop.
		if (!skip_function()) {			// In case we pressed restart or another high we will skip everything and continue to a safe position.
			switch (server_answer) {
				//Init XY 
				case button_continue:
					temp_err = 0;
					if (error_XY) {
						if (!init_blocks(2)) temp_err++;
					}
					if (error_counter) {
						if (!init_blocks(3)) temp_err++;
					}
					if (error_blister) {
						if (!init_blocks(1)) temp_err++;
					}
					server_answer = 0;
				break;
				
				case button_ignore:
					// do nothing so we wond detect any error and we will continue
					server_answer = 0;
					temp_err = 0;
					send_error_to_server (no_error);		// Reset error on the server
				break;

				default:
					// Any other answer or 0
					server_answer = 0;
				break;
			}
		}else{
			// We enter here in case we trigger a main function of a parent loop
			temp_err = 0;
			send_error_to_server (no_error);		// Reset error on the server
		}
	}
	// Serial.print (" -OFF the error loop- ");
	block_loop = false;
}

void switch_off_machine () {
	send_action_to_server(power_off);
	delay (2000);

	pump_disable();
	motors_sleep ();	// Sleep motors
	motors_disable ();	// Enable motors
	PSupply_OFF ();		// Switch Power supply ON
	Serial.println ("Switching OFF!");
	send_status_to_server (S_switch_off);
}

void reset_machine () {
	block_loop = true;
	//send_status_to_server (S_stopped);		// First send status as stop to refresh // TESTING
	send_status_to_server (S_setting_up);	// here we comunicate the server that we begin the set-up process	
	send_error_to_server (no_error);		// NO ERROR
		// Get all configuration from the server
	get_config_from_server (C_All);	// gets default IDLE time
	// INIT SYSTEM, and CHECK for ERRORS
	init_all_motors ();
	// Updating Database from info staroed in the server
	get_positions_from_server (P0);					// receives all positions from server
	if (!do_a_restart) send_status_to_server (S_stopped);	// here we wait for the server to send orders if we don´t have to issue a restart..
	MySW.reset();
	MySW.start();
	blister_mode = 0;		// Reset blister mode in case we are reestarting
	block_loop = false;
}

void motors_enable () {
	if (get_motor_enable_state () == false) {
		Serial.println ("Enable Motors");
		send_action_to_server(enable_motors);
		set_motor_enable_state (true);
	}
}

void motors_disable () {
	if (get_motor_enable_state () == true) {
		Serial.println ("Disable Motors");
		send_action_to_server(disable_motors);
		set_motor_enable_state (false);
	}
}

void motors_sleep () {
	if (get_motor_sleep_state () == false) {
		Serial.println ("Sleep Motors");
		send_action_to_server(sleep_motors);
		set_motor_sleep_state (true);
	}
}

void motors_awake () {
	if (get_motor_sleep_state () == true) {
		Serial.println ("Awake Motors");
		send_action_to_server(awake_motors);
		set_motor_sleep_state (false);
	}
}

void PSupply_ON () {
	if (get_power_state () == false) { 
		Serial.println ("Power Supply ON");
		send_action_to_server(power_on);
		set_power_state (true);
	}
}

void PSupply_OFF () {
	if (get_power_state () == true) {
		Serial.println ("Power Supply OFF");
		send_action_to_server(power_off);
		set_power_state (false);
	}
}

void pump_enable () {
	if (get_pump_state () == false) {
		Serial.println ("Enable Pump");
		send_action_to_server(enable_pump);
		set_pump_state (true);
		delay (500);		// Wait 0.5 second to build up some pressure
	}
}

void pump_disable () {
	if (get_pump_state () == true) {
		Serial.println ("Disable Pump");
		send_action_to_server(disable_pump);
		set_pump_state (false);
	}
}


/////////////////////////
// Low Level functions
/////////////////////////

// POWER CONTROL
void set_power_state (boolean power_state) {
	if (power_state) {
		digitalWrite(PSupply, LOW);    // Power ON
	}else{ 
		digitalWrite (PSupply, HIGH);    // Power OFF
	}
}

boolean get_power_state () {
	return !(digitalRead (PSupply));
}

// MOTORS CONTROL
void set_motor_enable_state (boolean motor_state) {
	if (motor_state) {
		digitalWrite (enable, LOW);     // Enable motors 
	}else{ 
		digitalWrite (enable, HIGH);    // Disable motors 
	}
}

boolean get_motor_enable_state () {
	return !(digitalRead (enable));
}

void set_motor_sleep_state (boolean motor_state) {

	if (motor_state) {
		digitalWrite(sleep, LOW);    // Put drivers in sleep mode
	}else{ 
		digitalWrite (sleep, HIGH);    // Awake motors 
	}
}

boolean get_motor_sleep_state () {
	return !(digitalRead (sleep));
}

// PUMP functions
void set_pump_state (boolean pump_state) {

	if (pump_state) {
		digitalWrite (pump, HIGH);
	}else{ 
		digitalWrite (pump, LOW);
	}
}

boolean get_pump_state () {
	return digitalRead (pump);
}


/////////////////////////
// Timer functions
/////////////////////////

boolean check_idle_timer (boolean message) {

	// Check time, if time passed go IDLE
	if ((millis() - idle_counter_start_time) >= desired_idle_time) {
		// We are above the time limit. lets go IDLE.
		if (!IDLE_mode){
			if (message) Serial.println ("Sleep Time!");		
			send_action_to_server(enter_idle);
			IDLE_mode = true;
			pump_disable ();
		}	
		
		// Check if we are long time in IDLE (default_off_time) and we should switch off completely
		unsigned long temp_default_off_time = (unsigned long)default_off_time * 1000;
		if ((millis() - idle_counter_start_time) >= (desired_idle_time + temp_default_off_time)){
			if (get_motor_enable_state() || !get_motor_sleep_state() || get_power_state()) {
				motors_sleep ();	// Sleep motors
				motors_disable ();	// Disable motors
				PSupply_OFF ();		// Switch Power supply ON
				send_status_to_server (S_switch_off);				// Send status 
				if (message) Serial.println ("Switching OFF!");		// Print if nedeed
			}
		}
		return true;
	}else{
		// We are not in IDLE time
		return false;
	}
}

void start_idle_timer (unsigned long  seconds) {
	idle_counter_start_time = millis();				// Reset main counter
	desired_idle_time = seconds*1000;				// Convert seconds into milliseconds
	Serial.print ("Set IDLE timer to: ");
	Serial.println (desired_idle_time);
	Serial.print ("Set OFF timer to: ");
	Serial.println (desired_idle_time + ((unsigned long)default_off_time * 1000));
	IDLE_mode = false;
}

void end_idle_timer () {
	// if motors are sleep or disbaled means we went IDLE so restart...
	if (IDLE_mode) {
		Serial.println ("Recovering from ILDE!");
		send_action_to_server(resume_from_idle);
		if (!get_power_state() || get_motor_sleep_state() || !get_motor_enable_state()) {
			init_all_motors ();		// Restart
		}
		pump_enable ();	// True that if we init all motors pump will be already enabled
		// but if we come from state IDLE but we didnt shut down power we need to enable only pumps here.
		IDLE_mode = false;
	}
}
