////////////////////
// DEBUG MODE
////////////////////


/***** Test Menu, all functions included  *****/
void debug_mode () {
	
	Serial.print(F("\r\nEnter Debug Mode? [y/n]\r\n"));
	if (YN_question (6)) {
		debug_mode_enabled = true;
		boolean inTestMenu = true;
		while (inTestMenu) {
			Serial.println("\n	Select Action, to do :");
			Serial.println("		=====================================");
			Serial.println("		1 init all motors");
			Serial.println("		2 statistics");
			Serial.println("		3 print all EEPROM data");
			Serial.println("		=====================================");
			Serial.println("		4 calibrate positions XY");
			Serial.println("		5 calirate ejection servo");
			Serial.println("		6 calibrate counter motor");
			Serial.println("		7 calibrate blister servos ");
			Serial.println("		15 calibrate X speed ");
			Serial.println("		=====================================");
			Serial.println("		8 test pick up a seed and release");
			Serial.println("		9 test release blister");
			Serial.println("		10 test eject blister");
			Serial.println("		11 check sensors status");
			Serial.println("		12 activate pneumatics");
			Serial.println("		=====================================");
			Serial.println("		14 go to position");
			Serial.println("		=====================================");
			Serial.println("		0 Exit debug mode");
			boolean InMenuTemp = true;				// Init temp value for menu
			switch (get_number(2)) { 
				case 0:
					debug_action0 ();
					inTestMenu = false;
					debug_mode_enabled = false;
				break;
				case 1:debug_action1 ();break;
				case 2:debug_action2 ();break;
				case 3:debug_action3 ();break;
				case 4:debug_action4 ();break;
				case 5:debug_action5 ();break;
				case 6:debug_action6 ();break;
				case 7:debug_action7 ();break;
				case 8:debug_action8 ();break;
				case 9:debug_action9 ();break;
				case 10:debug_action10 ();break;
				case 11:debug_action11 ();break;
				case 12:debug_action12 ();break;
				case 13:debug_action13 ();break;
				case 14:debug_action14 ();break;
				case 15:debug_action15 ();break;
			}
		}
	}
}

void debug_action0 () {}
void debug_action1 () {
	Serial.println("\n	Init all motors");
	init_all_motors();
}
void debug_action2 () { statistics();}
void debug_action3 () { 
	Show_all_records();
	show_DBservos_data ();
	show_DBnetwork_data();
}
void debug_action4 () { calibrate_positionXY(0);}
void debug_action5 () {	calibrate_ejection_hooks ();}
void debug_action6 () { calibrate_counter();}
void debug_action7 () { calibrate_blister_hooks();}
void debug_action8 () {
	Serial.println("\n	Pickup one seed");
	if (error_counter) {
			Serial.println("\n	Error ** Counter not initialized correctly, first INIT");
	}else pickup_seed ();
}



void debug_action9 () {
		Serial.println("\n	Release one blister");
	if (error_blister) {
			Serial.println("\n	Error ** Blister not initialized correctly, first INIT");
	}else release_blister_servo ();
}
	

void debug_action10 () {
	Serial.println("Eject Blister");
	Serial.println("Do you want to go to ejection position? [Y/N]");
	YN_question ();
	//// go to position
	Serial.println("Are you in a safe position to test ejction? if you are not you could damage the machine. Continue? [Y/N]");
	YN_question ();
	/// contimnue or quit
	go_to_eject_blister ();
	eject_blister ();
}

void debug_action11 () {
	Serial.println("\n	Sensor Stats. Press keyboard key 4 to quit");
	boolean InMenuTemp =true;
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
}

void debug_action12 () {trigger_pneumatic_mechanism ();}
void debug_action13 () {}
void debug_action14 () {selectNgoToPosition ();}
void debug_action15 () {calibrate_accel_profileX ();}

boolean motors_initiated (int motor_type) {
/* MOTOR TYPES:
#define ALL 0
#define BLISTERS 1
#define XY 2
#define COUNTER 3
*/
	switch (motor_type) {
		case XY:
			if (error_XY) {
				Serial.println(F("\n	Error ** XY motors not initialized correctly, first INIT XY"));
				return false;
			}
			return true;
		break;

		case BLISTERS:
			if (error_blister) {
				Serial.println(F("\n	Error ** BLISTER motor not initialized correctly, first INIT BLISTER MOTOR"));
				return false;
			}
			return true;
		break;

		case COUNTER:
			if (error_counter) {
				Serial.println(F("\n	Error ** COUNTER motor not initialized correctly, first INIT COUNTER MOTOR"));
				return false;
			}
			return true;
		break;

		case ALL:
			boolean all_motors_ok = true;
			if (error_counter) {
				Serial.println(F("\n	Error ** COUNTER motor not initialized correctly, first INIT COUNTER MOTOR"));
				all_motors_ok = false;
			}
			if (error_counter) {
				Serial.println(F("\n	Error ** COUNTER motor not initialized correctly, first INIT COUNTER MOTOR"));
				all_motors_ok = false;
			}
			if (error_counter) {
				Serial.println(F("\n	Error ** COUNTER motor not initialized correctly, first INIT COUNTER MOTOR"));
				all_motors_ok = false;
			}

			if (all_motors_ok) {
				return true;
			}
			return false;

		break;
	}
}


/***** Prints the sensor status *****/
void print_sensor_stats() {
	PSupply_ON ();
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
	// Print Pneumatics
	Serial.print ("Pneumatics [");
	if (digitalRead (Pneumatics_sensor)) {
		Serial.print ("TRUE");
	}else{
		Serial.print ("FALSE");
	}
	Serial.println ("]");
	// Print Sensor F
	Serial.print ("Sensor F [");
	if (digitalRead (sensF)) {
		Serial.print ("TRUE - ");
	}else{
		Serial.print ("FALSE - ");
	}
	Serial.print (analogRead (sensF));
	Serial.println ("]");

	// Print Sensor G
	Serial.print ("Sensor G [");
	if (digitalRead (sensG)) {
		Serial.print ("TRUE - ");
	}else{
		Serial.print ("FALSE - ");
	}
	Serial.print (analogRead (sensG));
	Serial.println ("]");
	
}

void check_label_sens () {
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

void check_blister_sens () {
	PSupply_ON ();
	boolean test = true;
	int count = 0;
	while (test) {
		int sensor_state = digitalRead (SensBlister); 
		Serial.println (analogRead (SensBlister));
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
		if (count > 665) test = false;
	}
}

void check_emptyblister_sens () {
	PSupply_ON ();
	boolean test = true;
	int count = 0;
	while (test) {
		int sensor_state = digitalRead (SensOutBlisters); 
		Serial.println (analogRead (SensOutBlisters));
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

	Serial.println ("****** Testing servos");

	for (int f=0; f<20; f++) {
		for (int l = 0; l<100; l++) {
		blister_servoL.write(0);                  // sets the servo position according to the scaled value 
		blister_servoR.write(235);
		delay(5);                           // waits for the servo to get there 
		SoftwareServo::refresh();
		}

		for (int l = 0; l<100; l++) {
		blister_servoL.write(100);                  // sets the servo position according to the scaled value 
		blister_servoR.write(135);
		delay(5);                           // waits for the servo to get there 
		SoftwareServo::refresh();
		}

		Serial.println (f);
	}
	//delay (1300);
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