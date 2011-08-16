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

void loop() {
  if (false) {
   manual_modeXY();
  //manual_modeCounter();
  //delay (10);
  
  //Serial.print(" Seed counter Position:  ");
  //Serial.println(Seedcounter1.get_position());
  if (Xaxis.get_position() == (10 + old_xpos)) {
    Serial.print(" X motor Position:  ");
    old_xpos = Xaxis.get_position();
    Serial.println(old_xpos);
  }
  if (Yaxis.get_position() == (10 + old_ypos)) {
    Serial.print(" Y motor Position:  ");
    old_ypos = Yaxis.get_position();
    Serial.println(old_ypos);
  }
  }
  

  //Serial.stop();
  release_seed (); //arne comment: on first position
  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  

  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  
  go_position1 (); //arne comment: go to  next x axle position
  pickup_seed ();
  release_seed (); 
  
  
  // 2nd row

  go_position2 (); //arne comment: go to  next y axle position
  pickup_seed ();
  release_seed ();  //arne comment: on first position
  

  go_position3(); //arne comment: go to  next x axle position backwards
  pickup_seed ();
  release_seed (); 
  
 
  go_position3();
  pickup_seed ();
  release_seed (); 
  

  go_position3();
  pickup_seed ();
  release_seed (); 
  

  go_position3();
  pickup_seed ();
  release_seed (); 
  
  
  XYaxes_init();       // Initiates X and Y axes
  delay (500);
  boolean reset = true;
  // Start over again?
  while (reset)
  {
    if (digitalRead(button1) == HIGH) {
      reset = false;
      Seedcounter_init();  // Initiates seed counters
    }
  }
  
 
 //speed_cntr_Move(-25000,8000,8000,9000); // 1 full turn
 //speed_cntr_Move(move steps, accel, decel, max speed_)
 //Yaxis.set_direction (false);
 //Yaxis.do_step();
 //delayMicroseconds(800);  
 
}


///////////////////////////////////////////////////
void sm_driver_StepCounter(unsigned char Mdirection) {
  
  if (motor_select == 0) {
    Seedcounter1.set_direction (true);
    Seedcounter1.do_step();
  }else if (motor_select == 1) {
    Xaxis.set_direction (!Mdirection);
    Xaxis.do_step();
  }else if (motor_select == 2) {
    Yaxis.set_direction (!Mdirection);
    Yaxis.do_step();
  }
}

void release_seed () {
  motor_select = 0;
  speed_cntr_Move(380,10000,10000,10000);
  delay (800);
}

void pickup_seed () {
  motor_select = 0;
  boolean seed_detected = false;
  
  while (!seed_detected) {
    speed_cntr_Move(1180,6500,10000,10000);  // The steps to go to the sensor are 1200, but we count 1180 so we have 20 steps margin
                                             // for diferent size seeds. NOTICE that the acceleration in this case is lower
                                             // Thats to avoid trowing seeds and to achieve a better grip on the seed
    delay (1500);            
    for (int a=0; a < 40; a++) { 
      Seedcounter1.do_step();
      delayMicroseconds(1500);
      if (Seedcounter1.sensor_check()){
          seed_detected = true;
      }
    }
    if (!seed_detected) {
      speed_cntr_Move(380,10000,10000,10000);  // The steps to go to the sensor are 1200, but we count 1180 so we have 20 steps margin
                                            // for diferent size seeds
      delay (800); 
    }
  }
}

void go_position1 () { //arne comment: go to next position on x axle
  motor_select = 1;
  speed_cntr_Move(6500,8000,8000,9000);
  delay (2800);
  
}

void go_position2 () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(6430,8000,8000,9000);
  delay (2800);
}

void go_position3 () { //arne comment: go to next position on x axle backwards
  motor_select = 1;
  speed_cntr_Move(-6500,8000,8000,9000);
  delay (2800);
  
}

  

// ********************
// INIT FUNCTIONS
// ********************
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
  Seedcounter1.set_direction (true);   // Goes back till we find the sensor
  while (!seed_sensor)
	{
		if (!Seedcounter1.sensor_check()) {
			Seedcounter1.do_step();
		}
		
		if (Seedcounter1.sensor_check()) {
                  // If the sensor is true means we found the position of the sensor
                  seed_sensor = true;
		}
		// This delay slows down the velocity so we won't miss any step
                // Thats because we are not using acceleration in this case
		delayMicroseconds(500);
	}
        for (int i=0;i<10; i++) {   // Since the detection of the seed is just at the edge of the same seed we do
                                    // 10 steps to be more in the middle of the seed
          Seedcounter1.do_step();
          delayMicroseconds(500);
        }
	Seedcounter1.set_init_position();
        for (int i=0;i<20; i++) {   // We position at steps 20, so it like we just got a seed
          Seedcounter1.do_step();
          delayMicroseconds(500);
        }
        Serial.println (" - Seedcounter in position");
}



/// EXTRA FUCNTIONS


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
