// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************
// Init the both axes at the same time to save time.
#define max_insensor_stepsError 3000
#define max_outsensor_stepsError (long) 528000


boolean XYaxes_init () {

	// IMPLEMENT THE FOLLOWING:

	// There are two scenarios in which we can find our X axis and Y axis motors:
	// 1: We are hitting the sensor, means we are in a place we shouldn't be, so we go back in mode 1
	// and when we are out of sensor we go forward again in mode 8 till we touch it again. Then we are at position 0
	
	// 2: We are not hitting the sensor, means we need to find it. We go fas to it, wen we touch it 
	// we go back slow till we don touch it anymore

	// This way the motors will be very accurate.

	// We should move the motors at this point in mode 1 for top speed
	boolean both_sensors = false; 		// Falg for sensor checking
	unsigned long temp_counter=0;		// Counter for error checking
	while (!both_sensors) {					// While we are htting the sensor we gou outside the reach of it
		Xaxis.set_direction (false);			// Goes forth till we are not hitting the sensor
		Yaxis.set_direction (false);			// Goes forth till we are not hitting the sensor
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
		delayMicroseconds(motor_speed_XY);
		// Error checking, if we cannot reach a point where we dont hit the sensor meands that there is a problem
		temp_counter++;
		if (temp_counter > max_insensor_stepsError) {			// More than 3200 steps will generate an error (3200 steps = to 2 complete turns in step mode 8)
			if (Xaxis.sensor_check()) {
				// send_error("i3");				//still to implemetn an error message system
				// error in axis X off sensor range
			}
			if (Yaxis.sensor_check()) {
				// send_error("i4");				//still to implemetn an error message system
				// error in axis Y off sensor range
			}
			//send_error("i1");					//still to implemetn an error message system
			// Sensor error,  might be obstructed or disconnected.
			return false;
		}
	}
	temp_counter = 0;					// Reset the temop counter for error checking next step
	both_sensors = false;				// Reset sensors variable
	Xaxis.set_direction (true);		// Goes back till we find the sensor
	Yaxis.set_direction (true);		// Goes back till we find the sensor

	// We should move the motors at this point in mode 1 for top speed
	while (!both_sensors) {			// While we dont hit the sensor...
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
		delayMicroseconds(motor_speed_XY);
		// Error checking, if we cannot reach a point where we hit the sensor means that ther is a problem
		temp_counter++;
		if (temp_counter > max_outsensor_stepsError) {			// recheck the limit of revolutions
			if (!Xaxis.sensor_check()) {
				// send_error("i3");				//still to implemetn an error message system
				// error in axis X off sensor range
			}
			if (!Yaxis.sensor_check()) {
				// send_error("i4");				//still to implemetn an error message system
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

/*
***********************
** DEFINE ARRAY DIMENSION FOR BLISTERS
***********************
**
** Basic Positions:
** 
** 1. X - INIT POSITION						Y - INIT POSITION
** 2. X - Right before the blisters			Y - INIT POSITION
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
********************************************************/
/*
// Y
#define Yc1 0
#define Ys1 0
#define Yc2 7
#define Ys2 368
// X
#define Xc1 70
#define Xs1 1472
#define Xc2 79
#define Xs2 132
#define Xc3 87
#define Xs3 964
#define Xc4 95
#define Xs4 944
#define Xc5 103
#define Xs5 956

#define number_of_positions 20
// PROGMEM  prog_uchar name_of_array
// prog_uchar is an unsigned char (1 byte) 0 to 255
// prog_uint is an unsigned int (2 byte) 0 to 65,535
PROGMEM  const prog_uint16_t x_axis_set[number_of_positions*2] = {
// Basic positions
0,0,			// 1- INIT position
6,0,			// 2- Blister position
213,1400,		// 3- Printer position
280,1,			// 4- Exit position
// 2 rows blisters
Xc1,Xs1,		// 5- Position Hole 1
Xc1,Xs1,		// 6- Position Hole 2
Xc2,Xs2,		// 7- Position Hole 3
Xc2,Xs2,		// 8- Position Hole 4
Xc3,Xs3,		// 9- Position Hole 5
Xc3,Xs3,		// 10- Position Hole 6
Xc4,Xs4,		// 11- Position Hole 7
Xc4,Xs4,		// 12- Position Hole 8
Xc5,Xs5,		// 13- Position Hole 9
Xc5,Xs5,			// 14- Position Hole 10
// 1row blisters
Xc1,Xs1,		// 5- Position Hole 1
Xc1,Xs1,		// 6- Position Hole 2
Xc2,Xs2,		// 7- Position Hole 3
Xc2,Xs2,		// 8- Position Hole 4
Xc3,Xs3		// 9- Position Hole 5
};


PROGMEM  const prog_uint16_t y_axis_set[number_of_positions*2] = {
// Basic positions
0,0,			// 1- INIT position
0,0,			// 2- Blister position
3,834,			// 3- Printer position
0,0,			// 4- Exit position
// 2 rows blisters
Yc1,Ys1,		// 5- Position Hole 1
Yc2,Ys2,		// 6- Position Hole 2
Yc2,Ys2,		// 7- Position Hole 3
Yc1,Ys1,		// 8- Position Hole 4
Yc1,Ys1,		// 9- Position Hole 5
Yc2,Ys2,		// 10- Position Hole 6
Yc2,Ys2,		// 11- Position Hole 7
Yc1,Ys1,		// 12- Position Hole 8
Yc1,Ys1,		// 13- Position Hole 9
Yc2,Ys2,			// 14- Position Hole 10
// 1row blisters
Yc1,Ys1,		// 5- Position Hole 1
Yc2,Ys2,		// 6- Position Hole 2
Yc2,Ys2,		// 7- Position Hole 3
Yc1,Ys1		// 8- Position Hole 4
};

*/

// read back a 2-byte int example:
// pgm_read_word_near(y_axis_set + N)
// calculate cycle index position if index is N  -- > N = N + (N - 1)
// calculate step index position if index is N  -- > N = N + N


// advice: change names to Coarse & fine
/*
int get_cycle_Xpos_from_index(int index) {
	return pgm_read_word_near(x_axis_set + (2*index) - 2);
}

int get_step_Xpos_from_index(int index) {
	return pgm_read_word_near(x_axis_set + (2*index) - 1);
}

int get_cycle_Ypos_from_index(int index) {
	return pgm_read_word_near(y_axis_set + (2*index) - 2);
}

int get_step_Ypos_from_index(int index) {
	return pgm_read_word_near(y_axis_set + (2*index) - 1);
}*/

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

	int Xcycles = get_cycle_Xpos_from_index(position_index_to_go);
	int Xsteps = get_step_Xpos_from_index(position_index_to_go);
	int Ycycles = get_cycle_Ypos_from_index(position_index_to_go);
	int Ysteps = get_step_Ypos_from_index(position_index_to_go); 
	
	go_to_posXY (Xcycles, Xsteps, Ycycles, Ysteps) ;
}

void go_to_posXY (int Xcy,int Xst,int Ycy,int Yst) {
	check_pause ();			// Check for any pause button
	Xaxis.got_to_position (Xcy,Xst) ;
	check_pause ();			// Check for any pause button
	Yaxis.got_to_position (Ycy,Yst) ;
}
