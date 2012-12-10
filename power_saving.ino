// Main functions
/////////////////////////

void motors_enable () {
	Serial.println ("Enable Motors");
	send_action_to_server(enable_motors);
	set_motor_enable_state (true);
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
	Serial.println ("Awake Motors");
	send_action_to_server(awake_motors);
	set_motor_sleep_state (false);
}

void PSupply_ON () {
	Serial.println ("Power Supply ON");
	send_action_to_server(power_on);
	set_power_state (true);
}

void PSupply_OFF () {
	if (get_power_state () == true) {
		Serial.println ("Power Supply OFF");
		send_action_to_server(power_off);
		set_power_state (false);
	}
}

void pump_enable () {
	Serial.println ("Enable Pump");
	send_action_to_server(enable_pump);
	set_pump_state (true);
	//delay (1000);		// Wait 1 second to build up some pressure
}

void pump_disable () {
	Serial.println ("Disable Pump");
	send_action_to_server(disable_pump);
	set_pump_state (false);
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


void boring_messages () {
	if (check_idle_timer(false)) {
		int number = random(4);
		switch (number) {
			case 0: {
				Serial.println ("**** Hello? anybody here..? I said RESTART *****");
			break; }
			
			case 1: {
				Serial.println ("**** I see... this will take time... I can not just leave, can I? *****");
			break; }
			
			case 2: {
				Serial.println ("**** booooooring... *****");
			break; }
			
			case 3: {
				Serial.println ("**** Please switch me down *****");
			break; }
			
			case 4: {
				Serial.println ("**** You could at least put some music... *****");
			break; }
		}
	}
}


/////////////////////////
// Timer functions
/////////////////////////

boolean check_idle_timer (boolean message) {

	// Check time
	//Serial.print ("Passed ms: ");
	//Serial.println (millis() - idle_counter_start_time);
	if ((millis() - idle_counter_start_time) >= desired_idle_time) {
		// We are above the time limit. lets go IDLE.

		if (get_pump_state () == true) {		// Disable pump only if it was previously enabled
			if (message) Serial.println ("Sleep Time!");
			send_action_to_server(enter_idle);
			pump_disable ();
		}
		
		// Check if we are long time in IDLE (default_off_time) and we should switch off completely
		unsigned long temp_default_off_time = (unsigned long)default_off_time * 1000;
		if ((millis() - idle_counter_start_time) >= (desired_idle_time + temp_default_off_time)){
			motors_sleep ();	// Sleep motors
			motors_disable ();	// Disable motors
			PSupply_OFF ();		// Switch Power supply ON
			send_status_to_server (S_switch_off);				// Send status 
			if (message) Serial.println ("Switching OFF!");		// Print if nedeed
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
}

void end_idle_timer () {
	// if motors are sleep or disbaled means we went IDLE so restart...
	if (get_motor_sleep_state() || !get_motor_enable_state()) {
		Serial.println ("Switching ON!");
		init_all_motors ();		// Restart
	}

	if ((millis() - idle_counter_start_time) >= desired_idle_time) {
		Serial.println ("Wake UP!");
		send_action_to_server(resume_from_idle);
		// Enable pump in case is off
		if (!get_pump_state ()) {
			pump_enable ();
		}
	}
}
