// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************


// ***********************
// ** Physical limits of the motors
// ***********************
// Init the both axes at the same time to save time.
#define max_insensor_stepsError 4000
#define Xaxis_cycles_limit 280
#define Yaxis_cycles_limit 18


boolean XYaxes_init () {

	send_action_to_server (XY_init);

	// few previous calculations for the different speeds profiles
	int speed1 = motor_speed_XY;						// Main speed is always the slowest and safer
	int speed2 = speed1 - (motor_speed_XY/5);
	int speed3 = speed2 - (motor_speed_XY/5);
	int speed4 = speed3 - (motor_speed_XY/5);
	unsigned long vIncrement = 900;			// Amount f time in ms that motors will increase speed

	// IMPLEMENT THE FOLLOWING:

	// There are two scenarios in which we can find our X axis and Y axis motors:
	
	// 1: We are hitting the sensor, means we are in a place we shouldn't be, so we go back in mode 1
	// and when we are out of sensor we go forward again in mode 8 till we touch it again. Then we are at position 0
	
	// 2: We are not hitting the sensor, means we need to find it. We go fas to it, wen we touch it 
	// we go back slow till we don touch it anymore

	// This way the motors will be very accurate.

	// We should move the motors at this point in mode 1 for top speed
	
	/////////////////// LOGICS
	/*
	one or both sensors are outside the sensor
		both or one go inside at max speed
	delay to stop inertia
	when both inside
		they go outside at min speed till we release the sensor
	done!
	*/
	
	boolean both_sensors = false;				// Falg for sensor checking
	unsigned long temp_counter=0;				// Counter for error checking
	
	Xaxis.set_direction (!default_sensor_directionX);		// Goes back till we find the sensor 
	Yaxis.set_direction (!default_sensor_directionY);		// Goes back till we find the sensor
	// set speed max
	Xaxis.change_step_mode(4);		// Set stepper mode to 1 (Max speed)
	Yaxis.change_step_mode(4); 		// Set stepper mode to 1 (Max speed)
	
	unsigned long start_time = millis ();
	
	boolean skip_x = false;
	boolean skip_y = false;
	#if defined Xmotor_debug
		skip_x = true;
	#endif
	#if defined Ymotor_debug
		skip_y = true;
	#endif
	
	// We should move the motors at this point in mode 1 at top speed
	while (!both_sensors) {			// While we dont hit the sensor...
		if (!Xaxis.sensor_check()) {
			Xaxis.do_step();
		}else{
			delayMicroseconds(19);
		}

		if (!Yaxis.sensor_check()) { 
			Yaxis.do_step();
		}else{
			delayMicroseconds(19);
		}

		if ((Xaxis.sensor_check() || skip_x) && (Yaxis.sensor_check() || skip_y)) {
			// When both sensors are activated means we reached both init points
			both_sensors = true;
		}
		
		// Speed handeling
		unsigned long now = millis();
		
		if ((now - start_time) < vIncrement) {
			delayMicroseconds(speed1);
			// Serial.print("s1");
		}else if ((now - start_time) < (2*vIncrement)) {
			delayMicroseconds(speed2);
			// Serial.print("s2");
		}else if ((now - start_time) < (3*vIncrement)) {
			delayMicroseconds(speed3);
			// Serial.print("s3");
		}else {
			delayMicroseconds(speed4);
			// Serial.print("s4");
		}
		
		// Error checking, if we cannot reach a point where we hit the sensor means that there is a problem
		temp_counter++;
		if ((temp_counter/1600) > Yaxis_cycles_limit) {			// recheck the limit of revolutions
			send_error_to_server (init_Y1_fail);				//still to implement an error message system
			return false;
			}
		
		if ((temp_counter/1600) > Xaxis_cycles_limit) {			// recheck the limit of revolutions
			send_error_to_server (init_X1_fail);				//still to implement an error message system
		return false;
		}
	}
	// we can stop at maximum speed as we didnt init motors yet, so we dont care if we lose steps.
	
	delay (500);							// Enough for the motor to completely stop the inertia
	
	temp_counter = 0;						// Reset the temop counter for error checking next step
	both_sensors = false;					// Reset sensors variable
	Xaxis.set_direction (!default_sensor_directionX);			// Goes forth till we are not hitting the sensor
	Yaxis.set_direction (default_sensor_directionY);			// Goes forth till we are not hitting the sensor
	
	// set speed max
	Xaxis.change_step_mode(8);				// Set stepper mode to 8 (Max speed)
	Yaxis.change_step_mode(8); 				// Set stepper mode to 8 (Max speed)
	
	while (!both_sensors) {					// While we are hitting the sensor 

		// change mode to hi speed
		
		if (Xaxis.sensor_check()) {
			Xaxis.do_step();
		}else{
			delayMicroseconds(19);
		}
		if (Yaxis.sensor_check()) {
			Yaxis.do_step();
		}else{
			delayMicroseconds(19);
		}
               // Serial.print ("skip_Y = "); Serial.println ((!Xaxis.sensor_check() || (skip_x)) && (!Yaxis.sensor_check() || (skip_y)));
		if ((!Xaxis.sensor_check() || skip_x) && (!Yaxis.sensor_check() || skip_y)) {
			// When both sensors are NOT activated means we are inside the safe zone, now we can correctly init the axes
			both_sensors = true;
		}
            //Serial.print ("both_sensors = "); Serial.println (both_sensors);
		delayMicroseconds(motor_speed_XY);		// here we go at minimum speed so we asure we wont lose any step and we will achieve maximum acuracy
		
		// Error checking, if we cannot reach a point where we dont hit the sensor meands that there is a problem
		temp_counter++;
		if (temp_counter > max_insensor_stepsError) {			// recheck the limit of revolutions
			send_error_to_server (init_Y2_fail);				//still to implement an error message system
			return false;
                }
		
		if (temp_counter > max_insensor_stepsError) {			// recheck the limit of revolutions
			send_error_to_server (init_X2_fail);				//still to implement an error message system
			return false;
		}
	}
	delay (500);										// Enough for the motor to completely stop the inertia

	// we set init ponts for both sensors
	Xaxis.set_init_position();
	Yaxis.set_init_position();

	Yaxis.set_direction(false);
	// All correct , return true
	return true;
}


// ************************************************************
// ** POSITION FUNCTIONS
// ************************************************************

/*
***********************
** DEFINE ARRAY DIMENSION FOR BLISTERS
***********************
**
** Basic Positions:
** 
** 1. X - INIT POSITION						Y - INIT POSITION
** 2. X - Right before the blisters			        Y - INIT POSITION
** 3. X - Printer position					Y - Printer position
** 4. X - Exit position						Y - Exit position
**
**
** Blister type 10 Holes:
**
** 5. X - Hole 1								Y - Row 1
** 6. X - Hole 2								Y - Row 2
** 7. X - Hole 3								Y - Row 1
** 8. X - Hole 4								Y - Row 2
** 9. X - Hole 5								Y - Row 1
** 10. X - Hole 6								Y - Row 2
** 11. X - Hole 7								Y - Row 1
** 12. X - Hole 8								Y - Row 2
** 13. X - Hole 9								Y - Row 1
** 14. X - Hole 10								Y - Row 2
**
**
** Blister type 5 Holes:
**
** 15. X - Hole 1								Y - Row 1
** 16. X - Hole 2								Y - Row 1
** 17. X - Hole 3								Y - Row 1
** 18. X - Hole 4								Y - Row 1
** 19. X - Hole 5								Y - Row 1
**
**
** Extra positions
**
** 20. X - Brush position					Y - Brush position
**
********************************************************/


// read back a 2-byte int example:
// pgm_read_word_near(y_axis_set + N)
// calculate cycle index position if index is N  -- > N = N + (N - 1)
// calculate step index position if index is N  -- > N = N + N


// advice: change names to Coarse & fine
// READ

int get_cycle_Xpos_from_index(int index) {
	db.read(index, DB_REC mposition);
	return mposition.Xc;
}

int get_step_Xpos_from_index(int index) {
	db.read(index, DB_REC mposition);
	return mposition.Xf;
}

int get_cycle_Ypos_from_index(int index) {
	db.read(index, DB_REC mposition);
	return mposition.Yc;
}

int get_step_Ypos_from_index(int index) {
	db.read(index, DB_REC mposition);
	return mposition.Yf;
}

void go_to_memory_position (int position_index_to_go) {
	// Disable this function in case we are ending batch
	if (!skip_function()) {
		//send_position_to_server (position_index_to_go);		// Inform server that we are going to a position
		int Xcycles = get_cycle_Xpos_from_index(position_index_to_go);
		int Xsteps = get_step_Xpos_from_index(position_index_to_go);
		int Ycycles = get_cycle_Ypos_from_index(position_index_to_go);
		int Ysteps = get_step_Ypos_from_index(position_index_to_go); 
		
		go_to_posXY (Xcycles, Xsteps, Ycycles, Ysteps) ;
	}
}

void go_to_posXY (int Xcy,int Xst,int Ycy,int Yst) {
	check_stop (false);			// Check for any pause button
	Xaxis.got_to_position (Xcy,Xst) ;
	check_stop (false);			// Check for any pause button
	Yaxis.got_to_position (Ycy,Yst) ;
}


// Global vars for recorded position (for save mode)
int temp_Xcycles = Xaxis.get_steps_cycles();
int temp_Xsteps = Xaxis.get_steps();
int temp_Ycycles = Yaxis.get_steps_cycles();
int temp_Ysteps = Yaxis.get_steps();

void record_actual_position () {
	temp_Xcycles = Xaxis.get_steps_cycles();
	temp_Xsteps = Xaxis.get_steps();
	temp_Ycycles = Yaxis.get_steps_cycles();
	temp_Ysteps = Yaxis.get_steps();
}

void go_to_last_saved_position () {
	go_to_posXY (temp_Xcycles, temp_Xsteps, temp_Ycycles, temp_Ysteps) ;
}
