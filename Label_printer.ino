void print_one_label () {
	// Print one label 
	send_action_to_server(ask_for_label);
}

void print_and_release_label () {
	if (!skip_function() && (global_status != S_finishing_batch)) {
		check_status(false);		// Check for any pause status
		print_one_label ();
		/*
		// Wait for the printer to print a label
		boolean released = check_label_realeased (true);
		go_to_memory_position (3);			// Print position
		boolean done = false;
		unsigned long label_error_delay = 3000;
		unsigned long sensor_detect_timecode = 0;
		while (!released) {
			if (!done) send_error_to_server (label_timeout);
			if (!done) start_idle_timer (default_idle_time);		// start timer to calcule when do we have to go IDLE
			if (!done) Serial.println("Label error, remove any label that might be left and press number 1 to try again or 2 to continue.");
			done = true;

			if (skip_function()) {						// here code to skip function
				send_error_to_server (no_error);		// Reset error on the server
				released = true;
			}

			if (Lab_server_answer ()) {
				if (button_continue) {
					server_answer = 0;
					released = true;		// Detected not detected but we continue...
					end_idle_timer ();
				}

				if (button_print_label) {
					server_answer = 0;
					print_one_label ();
				}
			}

			if (digitalRead (SensLabel)) {		// Detected label. we have to wait 2 seconds to discard false positives
				if ((millis() - label_error_delay) > sensor_detect_timecode) {			// int time_to_wait
					released = true;		// Detected label
				}
			}else{
				sensor_detect_timecode = millis();
			}
			check_idle_timer (true);
		}

		// We have now a released label
		send_error_to_server(no_error);
		end_idle_timer ();*/
	}
}

void trigger_pneumatic_mechanism () {
	if (!skip_function()) {
		// Trigger pneumatics
		check_status(false);		// Check for any pause status
		PSupply_ON ();
		send_action_to_server(trigger_pneumatics);
		Serial.println(F("Trigger Pneumatics"));
		digitalWrite (extraoutput, HIGH);
		delay (400);
		digitalWrite (extraoutput, LOW);
		Serial.print(F("In progress"));
	}
}

void check_penumatics_are_done () {
	if (!skip_function()) {
		// When pin goes low indicates that is in progress
		while (!(digitalRead (Pneumatics_sensor))) {
			// wait until the pneumatic has finished
			Serial.print(F("."));
			delay(500);
		}
	    delay (1000);
		// After goes high again..
		// Serial.println(F("\nDone!"));
	}
}

boolean Lab_server_answer () {
	check_server ();		// Here we can not do a check stop cause we would go into an endless loop.	
	switch (server_answer) {

		case button_continue:
			return true;
		break;

		case button_print_label:
			return true;
		break;

		default:
			// Any other answer or 0
			server_answer = 0;
			return false;
		break;
	}
}

boolean check_label_realeased (boolean print) {
	boolean skip_print_sens = false;
#if defined Sensor_printer
	skip_print_sens = true;
#endif
	boolean label = false;
	boolean timeout_label = false;
	int count = 0;
	if (print) Serial.print("Label released: ");
	
	// Check if we got a label, or we timeout
	while (!label && !timeout_label) {
		label = digitalRead (SensLabel); 
		count ++;
		if (count == 200) timeout_label = true;
		delay (10);
		if (skip_print_sens) count == 199;
	}
	
	
	if (label || skip_print_sens) {
		if (print) print_ok();
		send_action_to_server(label_ok);
		delay (500);		// Just give sometime to the printer to finsh the job before we move
		Serial.println("Go to brush position");
		go_to_memory_position (20);
		label = digitalRead (SensLabel);		// After moving we check the label again, could be that wasn't completely stiked and moved on the way.
		if (!label && !skip_print_sens) return false;
		return true;
	}

	if (print) print_fail ();
	send_error_to_server (label_timeout);
	return false;
}


