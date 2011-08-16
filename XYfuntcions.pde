// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************
// Init the both axes at the same time to save time.
#define max_insensor_stepsError 5000

boolean XYaxes_init () {
  
  // IMPLEMENT THE FOLLOWING:
  
  // There are to scenarios in which we can find our X axis and Y axis motors:
  // 1: We are hitting the sensor, means we are in a place we shouldn't be, so we go back in mode 1
  // and when we are out of sensor we go forward again in mode 8 till we touch it again. Then we are at position 0
  
  // 2: We are not hitting the sensor, means we need to find it. We go fas to it, wen we touch it 
  // we go back slow till we don touch it anymore
  
  // This way the motors will be very accurate.
  
  // We should move the motors at this point in mode 1 for top speed
  boolean both_sensors = false; 	// Falg for sensor checking
  long tem_counter=0;				// Counter for error checking
  while (!both_sensors) {			// While we are htting the sensor we gou outside the reach of it
    MotorA.set_direction (false);   // Goes forth till we are not hitting the sensor
    MotorB.set_direction (false);   // Goes forth till we are not hitting the sensor
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
	// Error checking, if we cannot reach a point where we dont hit the sensor meands that ther is a problem
	temp_counter++;
	if (temp_counter > max_insensor_stepsError) {   // More than 3200 steps will generate an error (3200 steps = to 2 complete turns in step mode 8)
		//send_error("i1");     still to implemetn an error message system
		// Sensor error,  might be obstructed or disconnected.
		return false;
	}
  }
  
  both_sensors = false;          	// Reset sensors variable
  MotorA.set_direction (true);   	// Goes back till we find the sensor
  MotorB.set_direction (true);   	// Goes back till we find the sensor
    
  // We should move the motors at this point in mode 1 for top speed
  while (!both_sensors) {  			// While we dont hit the sensor...
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
	// Error checking, if we cannot reach a point where we hit the sensor means that ther is a problem
	temp_counter++;
	// RECHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	//if (temp_counter > 3200) {   // recheck the limit of revolutions
		//send_error("i2");     still to implemetn an error message system
		// sensor error, might be broquen, out of its place, disconected or failing
		//return false;
	//}
  }
  
  // we set init ponts for both sensors
  MotorA.set_init_position();
  MotorB.set_init_position();
  // All correct , return true
  return true;
}












// ************************************************************
// ** POSITION FUNCTIONS
// ************************************************************
/*  OLD EXAMPLE 
void go_position1 () { //arne comment: go to next position on x axle 
  motor_select = 1;
  speed_cntr_Move(6530,19000,20000,20000);
  delay (5550);
}
*/
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
