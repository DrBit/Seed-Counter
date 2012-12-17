// ***********************
// ** Physical limits of the blister motors
// ***********************

#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000

// ************************************************************
// ** UTILS  FUNCTIONS
// ************************************************************

/***** Checks free ram and prints it serial *****/
void mem_check () {
	//checking memory:
	Serial.print("Memory available: [");
	Serial.print(freeRam());
	Serial.println(" bytes]");
}

/***** Returns free ram *****/
int freeRam () {
	extern int __heap_start, *__brkval; 
	int v; 
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}



/***** Pause and wait till a button is pressed  *****/
void press_button_to_continue (int button_number) {
	//Serial.flush();
	boolean _pause = true;
	start_idle_timer (default_idle_time);		// start timer to calcule when do we have to go IDLE
	switch (button_number) {
		case 1:
			while (_pause) {
                                /*
				//Chek if we press the start button
				if (digitalRead(button1) == HIGH) {
					pause = false;   // If we do, unpause
				}*/
				if (Serial.available()) {
					if (Serial.read() == '1') {
						_pause = false;
					}
				} 
				check_idle_timer (true);
			}
		break;
		case 2:
			while (_pause) {
                                /*
				//Chek if we press the start button
				if (digitalRead(button2) == HIGH) {
					pause = false;   // If we do, unpause
				}*/
				if (Serial.available()) {
					 if (Serial.read() == '2') {
						_pause = false;
					 }
				}
				check_idle_timer (true);
			}
		break;
		case 3:
			while (_pause) {
				//Chek if we press the start button
				/*if (digitalRead(button3) == HIGH) {
					pause = false;   // If we do, unpause
				}*/
				if (Serial.available()) {
					if (Serial.read() == '2') {
						_pause = false;
					}
				}
				check_idle_timer (true);
			}
		break;
		default: 
			// if nothing else matches, do the default
			// default is any key
			while (_pause) {
				if (Serial.available() > 0) {
					Serial.read();
					_pause = false;
				}

			}
			check_idle_timer (true);
		break;
	}
	end_idle_timer ();
}



/***** Pause and return the number of any button pressed  *****/
int return_pressed_button () {
	//Serial.flush();
	boolean _pause = true;
	int pressed_button = 0;
	start_idle_timer (default_idle_time);
	while(_pause) {
		// Serial interface
		if (Serial.available()) {
			char received_char = Serial.read();
			if (received_char == '1') {
				pressed_button = 1;
				_pause = false;
			}
			if (received_char == '2') {
				pressed_button = 2;
				_pause = false;
			}
			if (received_char == '3') {
				pressed_button = 3;
				_pause = false;
			}
			if (received_char == '4') {
				pressed_button = 4;
				_pause = false;
			}
			if (received_char == '5') {
				pressed_button = 5;
				_pause = false;
			}
			if (received_char == '6') {
				pressed_button = 6;
				_pause = false;
			}
			if (received_char == '7') {
				pressed_button = 7;
				_pause = false;
			}
			if (received_char == '8') {
				pressed_button = 8;
				_pause = false;
			}
			if (received_char == '9') {
				pressed_button = 9;
				_pause = false;
			}
			if (received_char == '0') {
				pressed_button = 0;
				_pause = false;
			}
		}  
		check_idle_timer (true);
	}
	
	end_idle_timer ();
	
	//Serial.flush();
	return pressed_button;
}




// Simple YES/NO Question
boolean YN_question () {
	start_idle_timer (default_idle_time);
	while (true) {
		if (Serial.available () > 0) {
			char C = Serial.read ();
			if (C == 'y' || C == 'Y') {
				end_idle_timer ();
				return true;
			}else if (C == 'n' || C == 'N') {
				end_idle_timer ();
				return false;
			}
		}
		check_idle_timer (true);
	}
}

// Simple YES/NO Question
boolean YN_question (int timeout) {
	while (timeout > 0) {
		if (Serial.available () > 0) {
			char C = Serial.read ();
			if (C == 'y' || C == 'Y') {
				return true;
			}else if (C == 'n' || C == 'N') {
				return false;
			}
		}
		delay (1000);
		timeout --;
	}
}


// Funtion that returns a number typed in the serial interface
int get_number(int buffer) {
	buffer = buffer +1;
	char PositionN[buffer];
	int length = 0;
	while (!Serial.available()) {}
	while (Serial.available()) {
		PositionN[length] = Serial.read();
		length = (length+1) % buffer;
		delay(30);
	}
	PositionN[length] = '\0';
	
	// Staring of script
	// String SpositionN = PositionN;
	int num = 0;
	for (int i = (buffer-2); i>=0 ; i--) {
		num = atoi(&PositionN[i]);
	}
	//Serial.println (num);
	return (int) num;
}



  



// ************************************************************
// ** INIT  FUNCTIONS
// ************************************************************

/***** Init indivudual or all blocks of the system *****/
int init_blocks(int block) {
	//Switch mode
	switch (block) {
		//Init XY 
		case 1:
			return init_blisters_menu ();
		break;
		//Init counter
		case 2:
			return init_XY_menu();
		break;
		//Init blisters
		case 3:
			return init_counter_menu ();
		break;  
		//Init ALL
		default:
			int error = 0;
			if (!init_blisters_menu ()) error++;
			if (!init_XY_menu()) error++;
			if (!init_counter_menu ()) error++;
			mem_check ();
			// If we found an error we will return 0 so it did not completed
			// We can chek which one failed trough the error sistem
			if (error == 0) {
				return 1; 
			}else{
				return 0;
			}
		break;
	}
}

/***** Init XY from menu *****/
int init_XY_menu() {
  // Init XY axis
    Serial.print("Init XY Axes: ");
    if (XYaxes_init()) {       // Initiates X and Y axes
	print_ok();
        error_XY = false;
        return 1;
    }else{
	print_fail();
        error_XY = true;
        return 0;
    }
}

/***** Init Counter from menu *****/
int init_counter_menu () {
	// Init Counter
	pump_enable ();
    Serial.print("Init Seed counter roll: ");
    if (Seedcounter_init()) {  // Initiates seed counters
		print_ok();
        error_counter = false;
        return 1;
    }else{
		print_fail();
        error_counter = true;
		pump_disable ();
        return 0;
    }
}

/***** Init Blisters from menu *****/
int init_blisters_menu () {
  //Init blister dispenser
    Serial.print("Init blister dispenser: ");
    if (blisters_init ()) {
	print_ok();
        error_blister = false;
        return 1;
    }else{
	print_fail();
        error_blister = true;
        return 0;
    }
}


void wait_for_blister_info () {
	// if we haven't received seeds mode ask for it:
	if (blister_mode == 0) {
			#if defined Server_com_debug
			Serial.println(F("Waiting for Blister mode (I1)..."));
			#endif
			unsigned int info_timeout = 0;
		while (blister_mode == 0) {
			// get_info_from_server (get_seeds_mode);			// Gets seed mode (5 or 10 seeds per blister)
			check_server();		// Should we?
			delay (500);
			info_timeout++;

			#if defined bypass_server
				if (info_timeout == 100) {
					#if defined send_error_to_server
					Serial.println(F("Blister mode (I1) not defined. Impossible to continue. Please ask administrator..."));
					#endif
					send_error_to_server(missing_blister_info);
					pump_disable();
				}
			#endif
		}
		pump_enable();
		send_error_to_server(no_error);
	}
}

void check_stop () {

	if (endingBatch) {
		// Restore the flag endingBatch cause we now are at a save place and we can restart
		endingBatch = false;
		// endingBatch is enabled in "counter" inside function "pick a seed" and disables the basic functions of the loop
	}
	check_server();		// Should we?
	
	#if defined bypass_server 
	global_status = S_running;
	#endif
	
	start_idle_timer (default_idle_time);

	while (global_status != S_running && global_status != S_pause) {
		switch (global_status) {

			case S_finishing_batch: {
				// Since we go in here only when we are at the beginning of the process
				// We know now that we shouldn continue and change status to stopped
				send_status_to_server (S_stopped);
			break;}

			case S_stopped: {
				go_to_memory_position (2);		// We go to a safe place so we can disconect
				while (global_status == S_stopped) {
					#if defined Server_com_debug
					Serial.print(F("\n **STOP - Checking server global status... "));
					#endif
					delay(1000);
					check_server();
					check_idle_timer (true);
				}
			break;}

			case S_switch_off: {
				go_to_memory_position (2);		// We go to a safe place so we can disconect
				switch_off_machine ();		// Switch off machine
				while (global_status == S_switch_off) {
					// Do nothing while everithing is off
					#if defined Server_com_debug
					Serial.print(F("\n **OFF - Checking server global status... "));
					#endif
					check_server();
					delay (5000);
				}
				send_action_to_server(power_on); 
				reset_machine ();		// When changes we will switch back ON
				start_idle_timer (default_idle_time);		// Reset the timings for IDLE as we are not leaving this hole while
			break;}

			case S_test: {
				#if defined Server_com_debug
				Serial.print(F("\n ** Enterning TEST MODE"));
				#endif	
				end_idle_timer();

				while (global_status == S_test) {
					delay(150);
					check_server();
					// check_idle_timer (true);
				}
			break;}

			case S_setting_up: {
				#if defined Server_com_debug
				Serial.print(F("\n ** Setting UP"));
				#endif
				reset_machine ();
				start_idle_timer (default_idle_time);		// We restart the timer as there was a call inside reset_machine
			break;}

			default: {
				#if defined Server_com_error_debug
				Serial.print(F("\n ** Status not recognized: "));
				Serial.println(global_status);
				#endif
			break;}
		}
	}
	// global_status MUST be ready to get here
	// If we got here means we are ready to start. But before that we will check if we got all needed info from the server
	wait_for_blister_info ();		// Checks the status, waits until we receive info to proceed
	end_idle_timer();
}


void check_pause () {
	
	check_server();

	// Emergency button handler
	int button_emergency = digitalRead (emergency); // Conected at sens C?????
	// if (button_emergency) {		// Bypas for now,, re-enable when connected
	if (false) {
		send_status_to_server (S_pause);
		Serial.println ("Emergency Enabled");
		MySW.stop();
		// Send error
		start_idle_timer (default_idle_time);
		while (button_emergency) {
			button_emergency = digitalRead (emergency); 
			delay (500);
			check_idle_timer (true);
		}
		end_idle_timer ();
		MySW.start();
		Serial.println ("Emergency disabled");
		send_status_to_server (previous_status);
	}
	
	// Checking pause in the software
	if ((pause || global_status == S_pause) && !manual_enabled) {
		pause = true;
		MySW.stop();
		// We don't have a timer here cause we can not reestart after unpause
		// Must be enabled and disable by a user.
		pump_disable ();		// We do only switch off the pump
		while(pause) {
			check_server();
			if (global_status != S_pause) {
				pause = false;
			}
			delay (1000);
		}
		pump_enable ();
		MySW.start();
		pause = false;
	}
}

/***** Pause and return the number of any button pressed  *****/
void pause_if_any_key_pressed () {
		// Serial interface
	if (Serial.available()) {
		Serial.flush();
		boolean _pause = true;
		Serial.println(" Press 1 to unpause... ");
		while(_pause) {
			char received_char = Serial.read();
			if (received_char == '1') {
				_pause = false;
			}
		}
	}  
	Serial.flush();
}

unsigned int hours;
unsigned int minutes;
unsigned int seconds;

void statistics () {
	
	Serial.print ("\nCounter picked ");
	Serial.print (counter_s);
	Serial.print (" seeds from a batch total of ");
	Serial.println (max_batch_count);

	Serial.print ("Counter roll made a total of ");
	Serial.print (count_total_turns);
	Serial.println (" turns");

	Serial.print ("Turn rate: ");
	float turns_per_seed = (float)count_total_turns / (float)counter_s;
	Serial.print (turns_per_seed);
	Serial.println (" turns/seed");

	Serial.print ("Transcurred time from start: ");
	
	unsigned long total_ms = MySW.value();
	print_time(total_ms);

	Serial.print ("Seed rate: ");
	
	float seeds_per_minute = 0;
	float temp_minutes = ((float)seconds/60.0) + (float)minutes + ((float)hours * 60);
	seeds_per_minute = (float)counter_s/temp_minutes;
	Serial.print(seeds_per_minute);
	Serial.print (" seeds/minute - ");
	
	if (hours > 0) {
		float seeds_per_hour = 0;
		float temp_hours = ((((float)seconds/60.0) + (float)minutes)/60.0) + (float)hours ;
		seeds_per_hour = (float)counter_s/temp_hours;
		Serial.print(seeds_per_hour);
		Serial.println (" seeds/hour");
	}else{
		// aproximation
		Serial.print (" aprox. ");
		float seeds_per_hour = 0;
		float temp_hours = (((float)seconds/60.0) + (float)minutes)/60.0;
		seeds_per_hour = (float)counter_s/temp_hours;
		Serial.print(seeds_per_hour);
		Serial.println (" seeds/hour.");
	}
}

void print_time (unsigned long total_milliseconds) {
	seconds = total_milliseconds / 1000;
	
	minutes = seconds / 60;
	seconds = seconds % 60;
	
	hours = minutes / 60;
	minutes =  minutes % 60;
	
	if (hours < 10) Serial.print('0');
	Serial.print(hours);
	Serial.print(":");
	if (minutes < 10) Serial.print('0');
	Serial.print(minutes);
	Serial.print(":");
	if (seconds < 10) Serial.print('0');
	Serial.println(seconds);
}


void check_library_version () {
	if ((Xaxis.get_version()) != lib_version) {
		Serial.println("Library version mismatch");
		Serial.print(" This code is designed to work with library V");
		Serial.println(lib_version);
		Serial.print(" And the library installed is version V");
		Serial.println(Xaxis.get_version());
		Serial.println (" Program stoped!");
		send_error_to_server (library_error);
		while (true) {}
	}
}

// This function is called inside every function of the loop to check if we should skip the executuin in order to go to a save point in the sketch.
boolean skip_function() {
	if (endingBatch) {
		return true;
	}
	return false;
}