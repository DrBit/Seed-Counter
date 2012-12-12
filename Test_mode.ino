////////////////////
// TEST MODE
////////////////////

void vibrate_solenoid (byte solenoid_number, byte power, byte duration) {

// power 1 - 10 its the delay of the inner oscilation, the fastes (lowest number) less powerful
// the slowest (higher number) stronger the vibration
	for (int c=0; c<duration; c++) {
		for (int a=0; a < 100; a++) {
			digitalWrite (solenoid_number, HIGH);
			delay (power*2);
			digitalWrite (solenoid_number, LOW);
			delay (power*2);
		}
	}
}


/*
// vibrate_solenoid (byte solenoid_number, byte power, byte duration)
for (int a = 1; a<=10;a++) {
	Serial.println(F("power "));
	Serial.println(a);
	vibrate_solenoid (solenoid1, a, 30);
	delay (5000);
}*/


void check_sensorG () {
	PSupply_ON ();
	boolean test = true;
	int count = 0;
	while (test) {
		int sensor_state = digitalRead (SensLabel); 
		Serial.println (analogRead (SensLabel));
		if (sensor_state) {
			// We got the begining of the blister
			print_ok();
			// We got a blisters
			// Now we know that we have just a few left
			// We start counting
			// How can we reset this count when blisters are refilled? Database?
				// In database case. Check database, if refilled reset state.
		}else{
			print_fail ();
			// lister not detected, send error
			// press_button_to_continue (1);
		}
		delay (500);
		count ++;
		if (count > 25) test = false;
	}
}




void testing_motors () {

	//go_to_posXY (int Xcy,int Xst,int Ycy,int Yst)
	go_to_posXY (0,0,2,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,4,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,6,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,8,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,10,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,12,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,14,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,16,0);
	go_to_posXY (0,0,0,0);
	go_to_posXY (0,0,18,0);

	Serial.println ("Testing finished, restarting...");
	delay (1000);
}


void servo_test () {
	// Prepare to init motors
	if (get_power_state () == false) { 
		PSupply_ON ();		// Switch Power supply ON
	}
	if (get_motor_enable_state () == false) {
		motors_enable ();	// Enable motors
	}
	if (get_motor_sleep_state () == false) {
		motors_awake ();	// Awake motors
	}

	for (int f=0; f<20; f++) {
		for (int l = 0; l<100; l++) {
		myservo_left.write(0);                  // sets the servo position according to the scaled value 
		myservo_right.write(180);
		delay(15);                           // waits for the servo to get there 
		SoftwareServo::refresh();
		}

		for (int l = 0; l<100; l++) {
		myservo_left.write(180);                  // sets the servo position according to the scaled value 
		myservo_right.write(0);
		delay(15);                           // waits for the servo to get there 
		SoftwareServo::refresh();
		}
	}
	//delay (1300);
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
						//manual_modeXY();					// Manual mode, adjust position
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
						//manual_modeXY();
						if (Serial.read() == '4')  InMenuTemp = false;
					}
				}
			break;

			case 5:
				Serial.println("\n	Move Counter motors, buttons to move motors and press keyboard key 4 to quit");
				while (InMenuTemp) {
					//manual_modeCounter();
					if (Serial.read() == '4')  InMenuTemp = false;
				}
			break;
			
			case 6:
				Serial.println("\n	Move Blisters motors, buttons to move motors and press keyboard key 4 to quit");
				if (error_blister) {
						Serial.println("\n	Error ** Blisters motors not initialized correctly, first INIT");
				}else{
					while (InMenuTemp) {
						//manual_mode_blisters();
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

/*
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
*/

/***** MANUAL MODE BLISTERS  *****/
boolean holdC1 = false;
boolean holdC2 = false;
/*
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
*/

/***** MANUAL MODE Counter  *****/
boolean holdD1 = false;
boolean holdD2 = false;
/*
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
*/

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