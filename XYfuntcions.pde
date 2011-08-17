// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************
// Init the both axes at the same time to save time.
#define max_insensor_stepsError 3000
#define max_outsensor_stepsError (long) 528000


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
  unsigned long temp_counter=0;		// Counter for error checking
  while (!both_sensors) {			// While we are htting the sensor we gou outside the reach of it
    Xaxis.set_direction (false);    // Goes forth till we are not hitting the sensor
    Yaxis.set_direction (false);    // Goes forth till we are not hitting the sensor
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
    delayMicroseconds(600);
	// Error checking, if we cannot reach a point where we dont hit the sensor meands that there is a problem
	temp_counter++;
	if (temp_counter > max_insensor_stepsError) {   // More than 3200 steps will generate an error (3200 steps = to 2 complete turns in step mode 8)
          if (Xaxis.sensor_check()) {
            // send_error("i3");     //still to implemetn an error message system
            // error in axis X off sensor range
          }
          if (Yaxis.sensor_check()) {
            // send_error("i4");     //still to implemetn an error message system
            // error in axis Y off sensor range
          }
	  //send_error("i1");     still to implemetn an error message system
	  // Sensor error,  might be obstructed or disconnected.
	  return false;
	}
  }
  temp_counter = 0;					// Reset the temop counter for error checking next step
  both_sensors = false;          	// Reset sensors variable
  Xaxis.set_direction (true);   	// Goes back till we find the sensor
  Yaxis.set_direction (true);   	// Goes back till we find the sensor
    
  // We should move the motors at this point in mode 1 for top speed
  while (!both_sensors) {  			// While we dont hit the sensor...
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
    delayMicroseconds(600);
	// Error checking, if we cannot reach a point where we hit the sensor means that ther is a problem
	temp_counter++;
	if (temp_counter > max_outsensor_stepsError) {   	// recheck the limit of revolutions
                if (!Xaxis.sensor_check()) {
                  // send_error("i3");     //still to implemetn an error message system
                  // error in axis X off sensor range
                }
                if (!Yaxis.sensor_check()) {
                  // send_error("i4");     //still to implemetn an error message system
                  // error in axis Y off sensor range
                }
		// sensor error, might be broquen, out of its place, disconected or failing
		return false;
	}
  }
  
  // we set init ponts for both sensors
  Xaxis.set_init_position();
  Yaxis.set_init_position();
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


// ***********************
// ** DEFINE ARRAY DIMENSION FOR BLISTERS
// ***********************
// code description:
// PROGMEM  prog_uchar name_of_array[]
// prog_uchar is an unsigned char (1 byte) 0 to 255
PROGMEM  prog_uint16_t x_axis_set[]  = { 2,14,43,800,85,153,125,181,165,717,206,67,247,150};
PROGMEM  prog_uint16_t y_axis_set[]  = { 1,1,38,465,57,567};
// read back a 2-byte int
// pgm_read_word_near(charSet + N)
// cycle of blister N = N + (N - 1)
// step position of blister N = N + N


// change names to Coarse & fine
int get_cycle_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*pos_num) - 2);
}

int get_step_Xpos(int pos_num) {
  return pgm_read_word_near(x_axis_set + (2*pos_num) - 1);
}

int get_cycle_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*pos_num) - 2);
}

int get_step_Ypos(int pos_num) {
  return pgm_read_word_near(y_axis_set + (2*pos_num) - 1);
}

void go_to_position (int position_to_go) {
  int X_cycles_to_move = Xaxis.get_steps_cycles() - get_cycle_Xpos(position_to_go);
  int X_steps_to_move = Xaxis.get_steps() - get_step_Xpos(position_to_go);
  int Y_cycles_to_move = Yaxis.get_steps_cycles() - get_cycle_Ypos(position_to_go);
  int Y_steps_to_move = Yaxis.get_steps() - get_step_Ypos(position_to_go); 
}
