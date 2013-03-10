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


boolean get_blister_info () {

	if (blister_mode == 0) {
		#if defined bypass_server
		blister_mode = seeds10;		// We set blister mode to 1
		return true;
		#endif	

		#if defined Server_com_debug
		Serial.println(F("Waiting for Blister mode (I1)..."));
		#endif
		
		send_error_to_server(missing_blister_info);
		return false;
	}

	send_error_to_server(no_error);
	return true;
}

void check_for_autoreset () {
	if (autoreset) {
		int seedsXblister = 0;
		if (blister_mode == seeds10) seedsXblister = 10;
		if (blister_mode == seeds5) seedsXblister = 5;
		if ((((counter_s / seedsXblister) % blisters_for_autoreset) == 0) && (counter_s =! 1)) {
			Serial.print (F("Autoreset* Blisters counted: "));
			Serial.println (counter_s / seedsXblister);
			init_all_motors ();		// Cause our intention is just reset motor counters we don't have to do the hole reset procedure, just init motors
		}
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
	if (endingBatch || do_a_restart) {
		return true;
	}
	return false;
}