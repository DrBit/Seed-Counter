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
    MotorA.set_direction (true);   // Goes forth till we are not hitting the sensor
    MotorB.set_direction (true);   // Goes forth till we are not hitting the sensor
    if (MotorA.sensor_check()) {
      MotorA.do_step();
    }else{
      delayMicroseconds(18);
    }
    if (MotorB.sensor_check()) {
      MotorB.do_step();
    }else{
      delayMicroseconds(18);
    }
    if (!MotorA.sensor_check() && !MotorB.sensor_check()) {
      // When both sensors are NOT activated means we are inside the safe zone, now we can correctly init the axes
      both_sensors = true;
    }
    delayMicroseconds(800);
  }
  
  both_sensors = false;           // Reset sensors variable
  MotorA.set_direction (false);   // Goes back till we find the sensor
  MotorB.set_direction (false);   // Goes back till we find the sensor
  
  // Do it till we reach the sensors
  // *********** IMPLEMENT *****  we need to implement an error check if we never find the sensors
  // *********** IMPLEMENT *****  means either that the sensor is not working or the motor is not working
  while (!both_sensors) {
    if (!MotorA.sensor_check()) {
      MotorA.do_step();
    }else{
      delayMicroseconds(18);
    }

    if (!MotorB.sensor_check()) {
      MotorB.do_step();
    }else{
      delayMicroseconds(18);
    }
		
    if (MotorA.sensor_check() && MotorB.sensor_check()) {
      // When both sensors are activated means we reached both init points
      both_sensors = true;
    }
    delayMicroseconds(390);
  }
  
  // we set init ponts for both sensors
  MotorA.set_init_position();
  MotorB.set_init_position();
}

void motorc_init () {
	// WE GO AS FAR AS IT COULD GO THE MOTOR
	MotorC.set_direction (true);   // Goes forth till we are not hitting the sensor
	for (x=total_mc_steps; x=0; x++) {
		MotorC.do_step();
		delayMicroseconds(800);
	}	

}



// ************************************************************
// ** POSITION FUNCTIONS
// ************************************************************
void go_position1 () { //arne comment: go to next position on x axle 
  motor_select = 1;
  speed_cntr_Move(6530,19000,20000,20000);
  delay (5550);
}


void go_position2 () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(6500,15000,16000,16000);
  delay (3550);
}

void go_positionRow () { //arne comment: go to next position on y axle
  motor_select = 2;
  speed_cntr_Move(8800,15000,16000,16000);
  delay (3500);
}

void go_position3 () { //arne comment: go to next position on x axle backwards 
  motor_select = 1;
  speed_cntr_Move(-6530,19000,20000,20000);
  delay (2550); 
}



// change names to Coarse & fine

int get_cycle_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*N) - 2)
}

int get_step_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*N) - 1)
}

int get_cycle_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*N) - 2)
}

int get_step_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*N) - 1)
}

void go_to_position (int position_to_go) {
  int X_cycles_to_move = MotorA.get_steps_cycles() - get_cycle_Xpos(position_to_go);
  int X_steps_to_move = MotorA.get_steps() - get_step_Xpos(position_to_go);
  int Y_cycles_to_move = MotorB.get_steps_cycles() - get_cycle_Ypos(position_to_go);
  int Y_steps_to_move = MotorB.get_steps() - get_step_Ypos(position_to_go); 
}


// cycle = 1600 steps
//  
