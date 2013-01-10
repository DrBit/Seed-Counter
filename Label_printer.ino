void print_one_label () {
	// Print one label 
	send_action_to_server(ask_for_label);
}

void print_and_release_label () {
	if (!skip_function() && (global_status != S_finishing_batch)) {
		check_status(false);
		print_one_label ();
		// Wait for the printer to print a label
		boolean released = check_label_realeased (true);
		int button_pressed = 0;
		//Serial.println("Goto print position");
		go_to_memory_position (3);			// Print position
		boolean done = false;
		unsigned long label_error_delay = 3000;
		unsigned long sensor_detect_timecode = 0;
		while (!released) {
			if (!done) send_error_to_server (label_timeout);
			if (!done) start_idle_timer (default_idle_time);		// start timer to calcule when do we have to go IDLE
			if (!done) Serial.println("Label error, remove any label that might be left and press number 1 to try again or 2 to continue.");
			done = true;

			if (Serial.available() > 0) {
				button_pressed = Serial.read();
			}

			if (digitalRead (SensLabel)) {
				// Detected label. we have to wait 2 seconds to discard false positives
				// int time_to_wait
				if ((millis() - label_error_delay) > sensor_detect_timecode) {
					send_error_to_server(no_error);
					end_idle_timer ();
					break;
				}
			}else{
				sensor_detect_timecode = millis();
			}

			if (button_pressed == '1') {
				print_one_label ();
				released = check_label_realeased (true);
				done = false;
				button_pressed = 0;
			}
			if ((button_pressed == '2') || continue_pressed ()) {
				send_error_to_server(no_error);
				end_idle_timer ();
				break;
			}

			check_idle_timer (true);
		}
		send_error_to_server(no_error);
		end_idle_timer ();
	}
}

boolean continue_pressed () {
	
	check_server ();		// Here we can not do a check stop cause we would go into an endless loop.	
	switch (server_answer) {

		case button_continue:
			server_answer = 0;
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
		delay (20);
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


