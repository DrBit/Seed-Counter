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
        Xaxis.set_direction (true);   // Goes backward towards the sensor
        Xaxis.do_step();
      }
      holdX1 = true;
    }else{
      if ((Yaxis.get_steps_cycles() >= 0) && (Yaxis.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
        Yaxis.set_direction (true);   // Goes backward towards the sensor
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
        Xaxis.set_direction (false);   // Goes forward
        Xaxis.do_step();
      }
      holdX2 = true;
    }else{
      if (Yaxis.get_steps_cycles() < Yaxis_cycles_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
        Yaxis.set_direction (false);   // Goes forward 
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
  delayMicroseconds(1990);
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
  delayMicroseconds(3990);
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
  delayMicroseconds(3990);
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
			}
		break;
		default: 
		// if nothing else matches, do the default
		// default is optional
		break;
	}
}

/***** Pause and return the number of any button pressed  *****/
int return_pressed_button () {
	Serial.flush();
	boolean pause = true;
	int pressed_button = 0;
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
	}
	Serial.flush();
	return pressed_button;
}

/***** Test Menu, all functions included  *****/
void test_functions () {
boolean inTestMenu = true;
	while (inTestMenu) {
		Serial.println("\n	Select Action, to do :");
		Serial.println("		1 to pick up a seed and release");
		Serial.println("		2 to release blister");
		Serial.println("		3 to move XY motors");
		Serial.println("		4 to move counter motors");
		Serial.println("		5 to move blisters motors");
		Serial.println("		6 to init all motors");
		Serial.println("		7 to change motor mode");
		Serial.println("		8 to check sensors status");
		Serial.println("		0 go to Main Menu");
		boolean InMenuTemp = true;				// Init temp value for menu
		switch (return_pressed_button ()) { 
			case 1:
				Serial.println("\n	Pickup one seed");
				if (error_counter) {
						Serial.println("\n	Error ** Counter not initialized correctly, first INIT");
				}else pickup_seed ();
			break;
			
			case 2:
				Serial.println("\n	Release one blister");
				if (error_blister) {
						Serial.println("\n	Error ** Blister not initialized correctly, first INIT");
				}else release_blister ();
			break;
			
			case 3:
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

			case 4:
				Serial.println("\n	Move Counter motors, buttons to move motors and press keyboard key 4 to quit");
				while (InMenuTemp) {
					manual_modeCounter();
					if (Serial.read() == '4')  InMenuTemp = false;
				}
			break;
			
			case 5:
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

			case 6:
				Serial.println("\n	Init all motors");
				init_blocks(ALL);
			break;
			
			case 7:
				Serial.println("\n	Change motor modes");
				//
			break;
			
			case 8:
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



// ************************************************************
// ** MANIN MENU FUNCTIONS
// ************************************************************

/***** Enters into main menu  *****/
void enter_main_menu() {
	boolean inMainMenu = true;
  
	while (inMainMenu) {
		Serial.println("Main Menu:");
		Serial.println("1 to start the seed counter");
		Serial.println("2 go to TEST menu ");
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
		delay (150); // wait 200ms so we unpress any pressed buttons
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
				// Security Check
				Serial.println("Check the seed counter for any blister that may be left in the X axel");
				Serial.println("When ready press button 1 to continue set-up process");
				delay (150);
				// Press button 1 to continue
				press_button_to_continue (1);
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

int init_XY_menu() {
  // Init XY axis
    Serial.print("Initializing XY Axes: ");
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

int init_counter_menu () {
  // Init Counter
    Serial.print("Initializing Seed counter roll: ");
    if (Seedcounter_init()) {  // Initiates seed counters
	print_ok();
        error_counter = false;
        return 1;
    }else{
	print_fail();
        error_counter = true;
        return 0;
    }
}

int init_blisters_menu () {
  //Init blister dispenser
    Serial.print("Initializing blister dispenser: ");
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
