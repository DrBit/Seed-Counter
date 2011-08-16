#include <Stepper_ac04.h>

// ***********************
// ** CONFIG MOTOR PINS
// ***********************

// Stteing up motor 1, step pin = 9, direction pin = 8 , sensor pin 6, limit steps counter to 1600 pulses per turn
Stepper_ac Seedcounter1(9,8,6,1600);   
// Stteing up motor 2, step pin = 11, direction pin = 10 , sensor pin 4, no limit on steps counter (0)
Stepper_ac Xaxis(11,10,4,0);   
// Stteing up motor 3, step pin = 13, direction pin = 12 , sensor pin 5, no limit on steps counter (0)
Stepper_ac Yaxis(13,12,5,0);  


// ***********************
// ** DEFINES PINS EXTRA BUTTONS
// ***********************
#define button1 2
#define button2 3

//
/////////////////////////////////////////////////
void setup() {
  //init_serial();

  //Configure 2 Input Buttons
  pinMode (button1, INPUT);
  pinMode (button2, INPUT);
  
  // Initiate the Timer1 config function in order to prepare the timing functions of motor acceleration
  speed_cntr_Init_Timer1();
  delay (10);  // Delay to be safe
  

  Serial.println (" Initialization of stepper motors ");
  
  // Reset all motors and find point 0
  XYaxes_init();       // Initiates X and Y axes
  Seedcounter_init();  // Initiates seed counters
}


int a = 0;
long old_xpos=0;
long old_ypos=0;
int motor_select=0;


// ************************************************************
// ** MAIN LOOP FUNCTION
// ************************************************************

void loop() {
   
  // Press button 2 to start
  boolean start = false;
  while (!start)
  {
    if (digitalRead(button2) == HIGH) {
      start = true;
      // We chek if we got a seed, if we dont we init again
      if (!Seedcounter1.sensor_check()) {
        Seedcounter_init();
      }
    }
  }
  
  // Start whith the process
  first_time_drop ();
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  
  
 
  // 2nd row
  go_position2 (); //arne comment: go to  next y axle position
  pickup_seed_extended ();  
  //////////////////////////

  go_position3(); //arne comment: go to  next x axle position backwards
  pickup_seed_extended ();  
 
  go_position3();
  pickup_seed_extended ();
  
  go_position3();
  pickup_seed_extended (); 
  
  go_position3();
  pickup_seed_extended ();
  
  // 3th row ...  and go on
  go_position2 (); //arne comment: go to  next y axle position
  pickup_seed_extended ();  
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended ();  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed_extended (); 
  
  // 4th row ...  and go on
  go_position2 (); //arne comment: go to  next y axle position
  pickup_seed_extended (); 
  
  go_position3(); //arne comment: go to  next x axle position backwards
  pickup_seed_extended ();  
 
  go_position3();
  pickup_seed_extended ();
  
  go_position3();
  pickup_seed_extended (); 
  
  go_position3();
  pickup_seed_extended ();
  
  XYaxes_init();       // Initiates X and Y axes
  delay (500);  
}


////////////////////////////////////////////////////////////
// This function comes back from the acceleration function with a proper timing to 
// do steps on the motors
// As we dont have any way of knowing which motor we are moving back from this function
// we preselect the motor (0,1,2) before doing the movement
/////////////////////////////////////////////////////////////
void sm_driver_StepCounter(unsigned char Mdirection) {
  if (motor_select == 0) {
    Seedcounter1.set_direction (Mdirection);
    Seedcounter1.do_step();
  }else if (motor_select == 1) {
    Xaxis.set_direction (!Mdirection);
    Xaxis.do_step();
  }else if (motor_select == 2) {
    Yaxis.set_direction (!Mdirection);
    Yaxis.do_step();
  }
}

// Especial function for the first time drop (as we used one seed to detect the 0 position and is now at the sensor position
void first_time_drop () {
  motor_select = 0;
  speed_cntr_Move(-400,20000,20000,20000);
  while (Seedcounter1.get_position() != 400) {
    // wait doing nothing until we drop the seed
    delay (100); // This delay also waits for the seed to fall
  }
}

// Function to pick up a seed
// Automatically detects when there is , or not a seed and drops it.
void pickup_seed_extended () {
  motor_select = 0;
  boolean seed_detected = false;
  
  while (!seed_detected) {
    if (Seedcounter1.get_position() == 400) {
      delay (100);   // Wait for the interruption to reset itself   // CHEK WHY IS THIS HAPPENING --
      speed_cntr_Move(-1600,5500,19000,19000);  // We do a full turn, NOTICE that the acceleration in this case is lower
                                                // Thats to avoid trowing seeds and to achieve a better grip on the seed
    }
    if ((Seedcounter1.get_position() >= 1180) || (Seedcounter1.get_position() <= 20)) {   // We chek the sensor only when we are in the range of the sensor
      if (Seedcounter1.sensor_check()){                 // We got a seed!!!
          seed_detected = true; 
          Seedcounter1.wait_till_reach_position(400);   // We wait until we are bak at the deposit position (400)
          delay (100); // Wait for the seed to fall
      }
    }
  }
}

// ************************************************************
// ** POSITION FUNCTIONS
// ************************************************************
void go_position1 () { //arne comment: go to next position on x axle 
  motor_select = 1;                               // Select motor 1 (Xaxis) this could be change to something more logical
  long position = Xaxis.get_position ();          // Record the acutal position
  speed_cntr_Move(6500,19000,20000,20000);        // Move to the next hole in the blister
  Xaxis.wait_till_reach_position(position+6500);  // Wait till the movement has finished
}

void go_position2 () { //arne comment: go to next position on y axle
  motor_select = 2;                               // Select motor 2 (Yaxis) this could be change to something more logical
  long position = Yaxis.get_position ();
  speed_cntr_Move(6430,19000,20000,20000);
  Yaxis.wait_till_reach_position(position+6430);
}

void go_position3 () { //arne comment: go to next position on x axle backwards
  motor_select = 1;
  long position = Xaxis.get_position ();
  speed_cntr_Move(-6500,19000,20000,20000);
  Xaxis.wait_till_reach_position(position-6500);
}


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
    delayMicroseconds(400);
  }
  
  // we set init ponts for both sensors
  Xaxis.set_init_position();
  Yaxis.set_init_position();
  Serial.println (" - XY axes in position");
}

// Inits the seedcounter using the sensor as a starting point
// The sensor should be always at the same position in all the seedcunters and the init position hardcoded based on the position of the sensor.
void Seedcounter_init() {
  boolean seed_sensor = false; 
  int count = 0;
  Seedcounter1.set_direction (true);   // Goes back till we find the sensor
  while (!seed_sensor) {
          // If the vacuum is not on, this would be cheking for the 0 position foreve
          // so we count ten times and if there is no sense of a seed we pause
          count++;
          if (count == (1600*10)) {
            //do a pause
            boolean pause = true;
            while (pause) {
              //Chek if we press the start button
              if (digitalRead(button2) == HIGH) {
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
	    delayMicroseconds(600);
	}
        for (int i=0;i<10; i++) {   // Since the detection of the seed is just at the edge of the same seed we do
                                    // 10 steps further to be in the middle of the sensor
          Seedcounter1.do_step();
          delayMicroseconds(600);
        }
	Seedcounter1.set_init_position();  
}


// ************************************************************
// ** EXTRA FUNCTIONS  (NOT USED)
// ************************************************************

void manual_modeXY() {
  
  Xaxis.set_direction (true);   // Goes back till we find the sensor
  Yaxis.set_direction (true);   // Goes back till we find the sensor

  if (digitalRead(button1) == HIGH) {
    Xaxis.do_step();
  }else{
     delayMicroseconds(18);    
  }
  
  if (digitalRead(button2) == HIGH) {
    Yaxis.do_step();
  }else{
     delayMicroseconds(18);
  }
  
  delayMicroseconds(400);
}


void manual_modeCounter() {
  //
  if (digitalRead(button1) == HIGH) {
    Seedcounter1.set_direction (true);   // Goes back till we find the sensor
    Seedcounter1.do_step();
  }
  if (digitalRead(button2) == HIGH) {
    Seedcounter1.set_direction (false);   // Goes back till we find the sensor
    Seedcounter1.do_step();
  }
  delayMicroseconds(400);
}

void init_serial() {
  Serial.begin (9600);
  Serial.println ("*****************************");
  Serial.println ("** Seed Counter on-line    **");
  Serial.println ("*****************************");
  Serial.println (" ");
}
