// ***********************
// ** Physical limits of the blister motors
// ***********************

#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000


// ************************************************************
// ** MANUAL FUNCTIONS  (NOT USED)
// ************************************************************
boolean holdX = false;
boolean holdY = false; 
boolean holdX1 = false;
boolean holdY1 = false; 
boolean holdX2 = false;
boolean holdY2 = false;
boolean Xaxis_enabled = true;

/***** MANUAL MODE XY AXIS  *****/
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
  Serial.print ("* Y_cycles: ");
  Serial.print (blisters.get_steps_cycles());
  Serial.print (", Y_steps: ");
  Serial.println (blisters.get_steps());
}

/***** Print Position Counter axis  *****/
void print_counter_pos () {
  Serial.print ("* Couter cycles: ");
  Serial.print (counter.get_steps_cycles());
  Serial.print (", Counter steps: ");
  Serial.println (counter.get_steps());
}

/***** Print Position Counter axis  *****/
/***** NOT FINISHED!!!!!!!!!!!!!!!  *****/
boolean holdD1 = false;
boolean holdD2 = false;
void manual_modeCounter() {

  if (digitalRead(button1) == HIGH) {
    holdD2 = true;
  }
  else{    
    delayMicroseconds(190);
    if (holdD1) {
      holdD1 = false;
      print_counter_pos ();
    }
  }

  if (digitalRead(button2) == HIGH) {
    holdD2 = true;
  }
  else{    
    delayMicroseconds(190);
    if (holdD2) {
      holdD2 = false;
      print_counter_pos ();
    }
  }
  delayMicroseconds(390);
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
    }  
  }
  Serial.flush();
  return pressed_button;
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
    Serial.println("2 to check sensors status");
    Serial.println("3 to *****");
    
    switch (return_pressed_button ()) {
		case 1:   //Button 1 - to start the seed counter
		  Serial.println("Check the seed counter for any blister that may be left in the X axel");
                  Serial.println("When ready press button 1 to start set-up process");
                  delay (150);
                  // Press button 1 to start
                  press_button_to_continue (1);
                  inMainMenu = false;   // Quit main menu and start
		break;
                case 2:   //Button 1
                  Serial.println (" Press 3 to go back to main menu");
                  boolean inSensorCheck = true;
                  // While insede sensor chek we show sensors every second
		  while (inSensorCheck) {
		    //Chek if we press button 3
		    print_sensor_stats();
                    // 10 times 100 = 1000, so we wait 1 second while cheking the button each 300 ms
                    for (int i = 0; i <= 10; i++) {
                      delay (100);
                      // if button 3 is pressed we go back to the menu
                      if (digitalRead(button3) == HIGH) {
                        inSensorCheck = false;
                        i = 10;
                      }
                    }
                  }
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
     return init_XY_menu();
   break;
   //Init counter
   case 2:
     return init_counter_menu ();
   break;
   //Init blisters
   case 3:
     return init_blisters_menu ();
   break;  
   //Init ALL
   default:
     int error = 0; 
     if (!init_XY_menu()) error++;
     if (!init_counter_menu ()) error++;
     if (!init_blisters_menu ()) error++;
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
