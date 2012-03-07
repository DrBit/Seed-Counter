// ***********************
// ** Physical limits of the blister motors
// ***********************

#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000


// ************************************************************
// ** MANUAL FUNCTIONS  (NOT USED)
// ************************************************************


/***** MANUAL MODE XY AXIS  *****/
boolean holdX = false;
boolean holdY = false; 
boolean holdX1 = false;
boolean holdY1 = false; 
boolean holdX2 = false;
boolean holdY2 = false;
boolean Xaxis_enabled = true;

void manual_modeXY() {
  if (digitalRead(button1) == HIGH) {
    Xaxis_enabled =true;
  }else{
    Xaxis_enabled =false;
  }
  // Botton 2 moves motor BACKWARDS
  if (digitalRead(button2) == HIGH) {
    if (Xaxis_enabled) {
      if ((Xaxis.get_steps_cycles() >= 0) && (Xaxis.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
        Xaxis.set_direction (!default_directionX);   // Goes backward towards the sensor
        Xaxis.do_step();
      }
      holdX1 = true;
    }else{
      if ((Yaxis.get_steps_cycles() >= 0) && (Yaxis.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
        Yaxis.set_direction (!default_directionY);   // Goes backward towards the sensor
        Yaxis.do_step();
      }
      holdY1 = true;
    } 
    delayMicroseconds(18);
  }else{
    if (holdX1) {
      holdX1 = false;
      print_x_pos();
    }else if (holdY1) {    
      holdY1 = false;
      print_y_pos();
    }
  }
  // Botton 3 moves motor FORWARD
  if (digitalRead(button3) == HIGH) {
    if (Xaxis_enabled) {
      if (Xaxis.get_steps_cycles() < Xaxis_cycles_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
        Xaxis.set_direction (default_directionX);   // Goes forward
        Xaxis.do_step();
      }
      holdX2 = true;
    }else{
      if (Yaxis.get_steps_cycles() < Yaxis_cycles_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
        Yaxis.set_direction (default_directionY);   // Goes forward 
        Yaxis.do_step();
      }
      holdY2 = true;
    }
    delayMicroseconds(18);
  }else{
    if (holdX2) {
      holdX2 = false;
      print_x_pos();
    }else if (holdY2) {    
      holdY2 = false;
      print_y_pos();
    }
  }
  delayMicroseconds(motor_speed_XY);
}


/***** MANUAL MODE BLISTERS  *****/
boolean holdC1 = false;
boolean holdC2 = false;

void manual_mode_blisters() {
  // Botton 2 moves motor BACKWARDS
  if (digitalRead(button2) == HIGH) {
    if ((blisters.get_steps_cycles() >= 0) && (blisters.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
      blisters.set_direction (true);   // Goes backward towards the sensor
      blisters.do_step();
    }
    holdC1 = true;
    delayMicroseconds(18);
  }else{
    if (holdC1) {
      holdC1 = false;
      print_blisters_pos();
    }
  }
  // Botton 3 moves motor FORWARD
  if (digitalRead(button3) == HIGH) {
    if (blisters.get_steps() < blisters_steps_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
      blisters.set_direction (false);   // Goes forward
      blisters.do_step();
    }
    holdC2 = true;
    delayMicroseconds(18);
  }else{
    if (holdC2) {
      holdC2 = false;
      print_blisters_pos();
    }
  }
  delayMicroseconds(motor_speed_blisters);
}


/***** MANUAL MODE Counter  *****/
boolean holdD1 = false;
boolean holdD2 = false;

void manual_modeCounter() {
	// Button 2 moves motor BACKWARDS
	if (digitalRead(button2) == HIGH) {
		counter.set_direction (true);   // Goes backward towards the sensor
		counter.do_step();
		holdC1 = true;
		delayMicroseconds(18);
	}else{
		if (holdC1) {
			holdC1 = false;
			print_counter_pos ();
		}
	}
	// Botton 3 moves motor FORWARD
	if (digitalRead(button3) == HIGH) {
		counter.set_direction (false);   // Goes forward
		counter.do_step();
		holdC2 = true;
		delayMicroseconds(18);
	}else{
		if (holdC2) {
			holdC2 = false;
			print_counter_pos();
		}
	}
  //delayMicroseconds(motor_speed_counter*10);
  delay(100);
}


/***** Print Position X axis  *****/
void print_x_pos () {
	Serial.print ("* X_cycles: ");
	Serial.print (Xaxis.get_steps_cycles());
	Serial.print (", X_steps: ");
	Serial.println (Xaxis.get_steps());
}


/***** Print Position Y axis  *****/
void print_y_pos () {
	Serial.print ("* Y_cycles: ");
	Serial.print (Yaxis.get_steps_cycles());
	Serial.print (", Y_steps: ");
	Serial.println (Yaxis.get_steps());
}


/***** Print Position Blisters axis  *****/
void print_blisters_pos () {
	Serial.print ("* Blister_cycles: ");
	Serial.print (blisters.get_steps_cycles());
	Serial.print (", Blister_steps: ");
	Serial.println (blisters.get_steps());
}


/***** Print Position Counter axis  *****/
void print_counter_pos () {
	Serial.print ("* Couter_cycles: ");
	Serial.print (counter.get_steps_cycles());
	Serial.print (", Counter_steps: ");
	Serial.println (counter.get_steps());
}


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
	Serial.flush();
	boolean pause = true;
	start_idle_timer (default_idle_time);		// start timer to calcule when do we have to go IDLE
	switch (button_number) {
		case 1:
			while (pause) {
				//Chek if we press the start button
				if (digitalRead(button1) == HIGH) {
					pause = false;   // If we do, unpause
				}
				if (Serial.available()) {
					if (Serial.read() == '1') {
						pause = false;
					}
				} 
				check_idle_timer (true);
			}
		break;
		case 2:
			while (pause) {
				//Chek if we press the start button
				if (digitalRead(button2) == HIGH) {
					pause = false;   // If we do, unpause
				}
				if (Serial.available()) {
					 if (Serial.read() == '2') {
						pause = false;
					 }
				}
				check_idle_timer (true);
			}
		break;
		case 3:
			while (pause) {
				//Chek if we press the start button
				if (digitalRead(button3) == HIGH) {
					pause = false;   // If we do, unpause
				}
				if (Serial.available()) {
					if (Serial.read() == '2') {
						pause = false;
					}
				}
				check_idle_timer (true);
			}
		break;
		default: 
			// if nothing else matches, do the default
			// default is optional
			start_idle_timer (true);
		break;
	}
	end_idle_timer ();
}



/***** Pause and return the number of any button pressed  *****/
int return_pressed_button () {
	Serial.flush();
	boolean pause = true;
	int pressed_button = 0;
	start_idle_timer (default_idle_time);
	while(pause) {
		if (digitalRead(button1) == HIGH) {
			pressed_button = 1;
			pause = false;   // If we do, unpause
		}
		if (digitalRead(button2) == HIGH) {
			pressed_button = 2;
			pause = false;   // If we do, unpause
		}
		if (digitalRead(button3) == HIGH) {
			pressed_button = 3;
			pause = false;   // If we do, unpause
		}

		// Serial interface
		if (Serial.available()) {
			char received_char = Serial.read();
			if (received_char == '1') {
				pressed_button = 1;
				pause = false;
			}
			if (received_char == '2') {
				pressed_button = 2;
				pause = false;
			}
			if (received_char == '3') {
				pressed_button = 3;
				pause = false;
			}
			if (received_char == '4') {
				pressed_button = 4;
				pause = false;
			}
			if (received_char == '5') {
				pressed_button = 5;
				pause = false;
			}
			if (received_char == '6') {
				pressed_button = 6;
				pause = false;
			}
			if (received_char == '7') {
				pressed_button = 7;
				pause = false;
			}
			if (received_char == '8') {
				pressed_button = 8;
				pause = false;
			}
			if (received_char == '9') {
				pressed_button = 9;
				pause = false;
			}
			if (received_char == '0') {
				pressed_button = 0;
				pause = false;
			}
		}  
		check_idle_timer (true);
	}
	
	end_idle_timer ();
	
	Serial.flush();
	return pressed_button;
}


/***** Test Menu, all functions included  *****/
void test_functions () {
boolean inTestMenu = true;
	while (inTestMenu) {
		Serial.println("\n	Select Action, to do :");
		Serial.println("		=====================================");
		Serial.println("		1 to init all motors");
		Serial.println("		2 to adjust defined positions");
		Serial.println("		3 to show statistics");
		Serial.println("		=====================================");
		Serial.println("		4 to move XY motors manually");
		Serial.println("		5 to move counter motor manually");
		Serial.println("		6 to move blisters motors manually");
		Serial.println("		7 to pick up a seed and release");
		Serial.println("		8 to release blister");
		Serial.println("		9 to check sensors status");
		Serial.println("		=====================================");
		Serial.println("		0 go to Main Menu");
		boolean InMenuTemp = true;				// Init temp value for menu
		switch (return_pressed_button ()) { 
		
			case 1:
				Serial.println("\n	Init all motors");
				init_blocks(ALL);
			break;
			
			case 2:
				if (error_XY) {
						Serial.println("\n	Error ** XY motors not initialized correctly, first INIT");
				}else{
					Serial.println("\n Adjust positions");
					Serial.println("Type in the position number you want to adjust and press enter (2cyfers max)");
					int position_n = 0;
					position_n = get_number(2);		//2 is the number of digits we need the number
					Serial.print("Selected position: ");
					Serial.print(position_n);
					
					Serial.println(" - Go to position before adjust? Y/N");
					
					if (YN_question()) {
						// Go to selected position
						Serial.println ("Going to position: ");
						Serial.print ("Xc: "); Serial.print (get_cycle_Xpos_from_index(position_n));
						Serial.print (" Xf: "); Serial.println (get_step_Xpos_from_index(position_n));
						Serial.print ("Yc: "); Serial.print (get_cycle_Ypos_from_index(position_n));
						Serial.print (" Yf: "); Serial.println (get_step_Ypos_from_index(position_n));
						Serial.print ("moving...   ");
						manual_enabled = true;				// overwrite flag pause so we dont enter pause menu again
						go_to_memory_position (position_n);		
						manual_enabled = false;				// restore flag pause so we dont enter pause menu again
						Serial.println ("Done!");
					}
					
					Serial.println("Adjust position and press a key when ready.");
					while (InMenuTemp) {
						manual_modeXY();					// Manual mode, adjust position
						if (Serial.available()) {			// If a key is pressed
							Serial.flush();					// Remove all data from serial
							Serial.print("Save changes into postion: ");
							Serial.print(position_n);
							Serial.println(" ? Y/N");
							if (YN_question()) {
								// record the position in memory
								// WRITE
								mposition.Xc = Xaxis.get_steps_cycles();
								mposition.Xf = Xaxis.get_steps();
								mposition.Yc = Yaxis.get_steps_cycles();
								mposition.Yf = Yaxis.get_steps();
								db.write(position_n, DB_REC mposition);
								Serial.println("Position recorded!");
							}else{
								Serial.println("Position NOT recorded!");
							}
							InMenuTemp = false;			// Exit menu
						}
					}
				}
			break;
			
			case 3:
				statistics();
			break;
			
			case 4:
				Serial.println("\n	Move XY motors, buttons to move motors and press keyboard key 4 to quit");
				if (error_XY) {
						Serial.println("\n	Error ** XY motors not initialized correctly, first INIT");
				}else{
					while (InMenuTemp) {
						manual_modeXY();
						if (Serial.read() == '4')  InMenuTemp = false;
					}
				}
			break;

			case 5:
				Serial.println("\n	Move Counter motors, buttons to move motors and press keyboard key 4 to quit");
				while (InMenuTemp) {
					manual_modeCounter();
					if (Serial.read() == '4')  InMenuTemp = false;
				}
			break;
			
			case 6:
				Serial.println("\n	Move Blisters motors, buttons to move motors and press keyboard key 4 to quit");
				if (error_blister) {
						Serial.println("\n	Error ** Blisters motors not initialized correctly, first INIT");
				}else{
					while (InMenuTemp) {
						manual_mode_blisters();
						if (Serial.read() == '4')  InMenuTemp = false;
					}
				}
			break;
			
			case 7:
				Serial.println("\n	Pickup one seed");
				if (error_counter) {
						Serial.println("\n	Error ** Counter not initialized correctly, first INIT");
				}else pickup_seed ();
			break;
			
			case 8:
				Serial.println("\n	Release one blister");
				if (error_blister) {
						Serial.println("\n	Error ** Blister not initialized correctly, first INIT");
				}else release_blister ();
			break;
			
			case 9:
				Serial.println("\n	Sensor Stats. Press keyboard key 4 to quit");
				while (InMenuTemp) {
					print_sensor_stats();
					for (int i =0; i<=10; i++) {
						if (Serial.read() == '4')  {
							InMenuTemp = false;
							i=10;
						}
						delay (100);
					}
				}
			break;
			
			case 0:
				inTestMenu = false;
			break;
		}
	}
}

// Simple YES/NO Question
boolean YN_question () {
	while (true) {
		if (Serial.available ()) {
			char C = Serial.read ();
			if (C == 'y' || C == 'Y') {
				return true;
			}else if (C == 'n' || C == 'N') {
				return false;
			}
		}
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
// ** MANIN MENU FUNCTIONS
// ************************************************************

/***** Enters into main menu  *****/
void enter_main_menu() {
	boolean inMainMenu = true;
	while (inMainMenu) {
		//Serial.println("Main Menu:");
		Serial.println("1 to start the seed counter");
		Serial.println("2 go to MAIN menu ");
		//Serial.println("3 to *****");
    
		switch (return_pressed_button ()) {
			case 1:   //Button 1 - to start the seed counter
				// START INIT
				inMainMenu = false;   // Quit main menu and start
			break;
			
			case 2:   //Button 1
				test_functions ();
			break;
		}
		//delay (150); // wait 200ms so we unpress any pressed buttons
	}
}
  
/***** Prints the sensor status *****/
void print_sensor_stats() {
	// Print X
	Serial.print ("\nX axis sensor [");
	if (Xaxis.sensor_check()) {
		Serial.print ("TRUE");
	}else{
		Serial.print ("FALSE");
	}
	Serial.println ("]");
	// Print Y
	Serial.print ("Y axis sensor [");
	if (Yaxis.sensor_check()) {
		Serial.print ("TRUE");
	}else{
		Serial.print ("FALSE");
	}
	Serial.println ("]");
	// Print Counter
	Serial.print ("Counter sensor [");
	if (counter.sensor_check()) {
		Serial.print ("TRUE");
	}else{
		Serial.print ("FALSE");
	}
	Serial.println ("]");
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


void check_pause () {
	
	// Emergency button handler
	int button_emergency = digitalRead (emergency); 
	// if (button_emergency) {		// Bypas for now,, re-enable when connected
	if (false) {
		byte previous_status = global_status;
		send_status_to_server (pause);
		Serial.println ("Emergency Enabled");
		MySW.stop();
		// Send error
		start_idle_timer (60);
		while (button_emergency) {
			button_emergency = digitalRead (emergency); 
			delay (1000);
			
			check_idle_timer (true);
		}
		end_idle_timer ();
		MySW.start();
		Serial.println ("Emergency disabled");
		send_status_to_server (previous_status);
	}
	
	
	if ((Serial.available() || pause) && !manual_enabled) {
		pause = true;
		MySW.stop();
		Serial.flush();
		Serial.println ("Pause activated ");
		Serial.println ("Press 1 to resume");
		Serial.println ("Press 2 Change seed batch code");
		Serial.println ("Press 3 to print statistics");
		Serial.println ("Press 4 to reset statistics");
		Serial.println ("Press 5 to go to main menu");
		
		while(pause) {
			switch (return_pressed_button ()) {			
				case 1:
					pause = false;
					MySW.start();
				break;
				
				case 2:
					select_batch_number ();
					send_petition_to_configure_network ();
					update_network_configuration ();
				break;
				
				case 3:
					statistics();
				break;
				
				case 4:
					pause = false;
					counter_s = 0;
					count_total_turns = 0;
					MySW.reset();
					MySW.start();
				break;
				
				case 5:
					test_functions();
				break;
				
				
			}
		}
	}
}

/***** Pause and return the number of any button pressed  *****/
void pause_if_any_key_pressed () {
		// Serial interface
	if (Serial.available()) {
		Serial.flush();
		boolean pause = true;
		Serial.println(" Press 1 to unpause... ");
		while(pause) {
			char received_char = Serial.read();
			if (received_char == '1') {
				pause = false;
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
		send_error_to_server (library_error)
		while (true) {}
	}
}

void init_all_motors () {
	// INIT SYSTEM, and CHECK for ERRORS
	int temp_err = 0;   // flag for found errors
	if (!init_blocks(ALL)) temp_err = 1;
	
	while (temp_err > 0) { // We found an error, we chek ALL errors and try to initiate correctly
		temp_err = 0;
		Serial.println("\nErrors found, press 1 when ready to check again, 2 to bypas the errors");
		switch (return_pressed_button ()) {
			//Init XY 
			case 1:
				if (error_XY) {
					if (!init_blocks(2)) temp_err++;
				}
				if (error_counter) {
					if (!init_blocks(3)) temp_err++;
				}
				if (error_blister) {
					if (!init_blocks(1)) temp_err++;
				}
			break;
			
			case 2:
				// do nothing so we wond detect any error and we will continue
			break;
		}
	}
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
	// return true;
}

void pump_enable () {
	Serial.println ("Enable Pump");
	send_action_to_server(enable_pump);
	set_pump_state (true);
	delay (800);		// Wait 1 second to build up some pressure
}

void pump_disable () {
	Serial.println ("Disable Pump");
	send_action_to_server(disable_pump);
	// set_pump_state (false);
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


boolean check_idle_timer (boolean message) {

	if (idle_time_counter < desired_idle_time) {
		idle_time_counter ++;
		delay (100);
		return false;
	} else if (idle_time_counter == desired_idle_time) {
		idle_time_counter++;
		if (message) Serial.println ("Sleep Time!");
		send_action_to_server(enter_idle);
		pump_disable ();
		return true;
	}
	return false;
}

void start_idle_timer (unsigned long  seconds) {
	idle_time_counter = 0;
	desired_idle_time = seconds*10;
}

void end_idle_timer () {
	if (idle_time_counter >= desired_idle_time+1) {
		Serial.println ("Wake UP!");
		send_action_to_server(resume_from_idle);
		pump_enable ();
	}
}