// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************
// Init the both axes at the same time to save time.
void XYaxes_init () {
  boolean both_sensors = false; 
  // First of all we check if we are already hitting the sensor
  // If we are, we go just before the sensor is hit (Otherwise we would
  // be seeking the sensor forever)
  while (!both_sensors) {
    Xaxis.set_direction (true);   // Goes forth till we are not hitting the sensor
    Yaxis.set_direction (true);   // Goes forth till we are not hitting the sensor
    if (Xaxis.sensor_check()) {
      Xaxis.do_step();
    }else{
      delayMicroseconds(18);
    }
    if (Yaxis.sensor_check()) {
      Yaxis.do_step();
    }else{
      delayMicroseconds(18);
    }
    if (!Xaxis.sensor_check() && !Yaxis.sensor_check()) {
      // When both sensors are NOT activated means we are inside the safe zone, now we can correctly init the axes
      both_sensors = true;
    }
    delayMicroseconds(800);
  }
  
  both_sensors = false;           // Reset sensors variable
  Xaxis.set_direction (false);   // Goes back till we find the sensor
  Yaxis.set_direction (false);   // Goes back till we find the sensor
  
  // Do it till we reach the sensors
  // *********** IMPLEMENT *****  we need to implement an error check if we never find the sensors
  // *********** IMPLEMENT *****  means either that the sensor is not working or the motor is not working
  while (!both_sensors) {
    if (!Xaxis.sensor_check()) {
      Xaxis.do_step();
    }else{
      delayMicroseconds(18);
    }

    if (!Yaxis.sensor_check()) {
      Yaxis.do_step();
    }else{
      delayMicroseconds(18);
    }
		
    if (Xaxis.sensor_check() && Yaxis.sensor_check()) {
      // When both sensors are activated means we reached both init points
      both_sensors = true;
    }
    delayMicroseconds(390);
  }
  
  // we set init ponts for both sensors
  Xaxis.set_init_position();
  Yaxis.set_init_position();
}


// Inits the seedcounter using the sensor as a starting point
// The sensor should be always at the same position in all the seedcunters and the init position hardcoded based on the position of the sensor.
void Seedcounter_init() {
  boolean seed_sensor = false; 
  int count = 0;
  
  // First time we init we just go back one complete cycle in case we have seeds atached and we bring them into the deposit
  Seedcounter1.set_direction (false);   // Set direction
  for (int a=1600; a > 0; a--) {
    Seedcounter1.do_step();
    delayMicroseconds(motor_speed);
  }
    
  Seedcounter1.set_direction (true);   // Set direction
  while (!seed_sensor) {
          // If the vacuum is not on, this would be cheking for the 0 position foreve
          // so we count ten times and if there is no sense of a seed we pause
          count++;
          if (count == (800*10)) {  
            //do a pause
            boolean pause = true;
            Serial.println("*** error in the seed counter roll, check vacuum, check sensor");
            Serial.println("*** Press button 2 to try again");
            while (pause) {
              //Chek if we press the start button
              if (digitalRead(button2) == HIGH) {
                Serial.println("*** trying again...");
                pause = false;   // If we do, unpause
                count = 0;
              }
            }
          }  
          if (!Seedcounter1.sensor_check()) {
	    Seedcounter1.do_step();
	    }
	    if (Seedcounter1.sensor_check()) {
              // If the sensor is true means we found the position of the sensor
              seed_sensor = true;
	    }
	    // This delay slows down the velocity so we won't miss any step
            // Thats because we are not using acceleration in this case
	    delayMicroseconds(motor_speed);
	}
        for (int i=0;i<10; i++) {   // Since the detection of the seed is just at the edge of the same seed we do
                                    // 10 steps further to be in the middle of the sensor
          Seedcounter1.do_step();
          delayMicroseconds(motor_speed);
        }
	Seedcounter1.set_init_position();  

        Seedcounter1.set_direction (false);   // Set direction
        for (int i=0;i<400; i++) {   // we go back at the position we should be for starting point
          Seedcounter1.do_step();
          delayMicroseconds(motor_speed);
        }
        Seedcounter1.set_direction (true);   // Set direction
}


void init_serial() {
  Serial.begin (9600);
  Serial.println ("*****************************");
  Serial.print ("** Seed Counter on-line  ");
  Serial.println (version_prog);
  Serial.println ("*****************************");
  
}

