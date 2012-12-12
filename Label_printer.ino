void print_one_label () {
	// Print one label 
	send_action_to_server(ask_for_label);
}

void print_and_release_label () {
	if (!endingBatch) {
		print_one_label ();
		// Wait for the printer to print a label
		boolean released = check_label_realeased (true);
		int button_pressed = 0;
		Serial.println("Goto print position");
		go_to_memory_position (3);			// Print position
		while (!released) {
			Serial.println("Label error, remove any label that might be left and press number 1 to try again or 2 to continue.");
			
			if (Serial.available() > 0) {
				button_pressed = Serial.read();
			}

			if (digitalRead (SensLabel)) {
				send_error_to_server(no_error);
				break;
			}

			if (button_pressed == '1') {
				print_one_label ();
				released = check_label_realeased (true);
			}
			if (button_pressed == '2') {
				send_error_to_server(no_error);
				break;
			}
		}
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
		delay (80);
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


