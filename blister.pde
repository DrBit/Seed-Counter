// ***********************
// ** Physical limits of the blister motors
// ***********************
#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000
#define steps_to_move_when_blister_falls 800


// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************

boolean blisters_init () {
	int steps_to_do = (blisters_steps_absoulut_limit+100) / blisters.get_step_accuracy();		// The absolut limit would be 1000, but we add an extra 100 to be sure that we hit the maximum point so we init correctly
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 
	blisters.set_init_position();
	return true;
}

// ************************************************************
// ** USEFUL FUNCTIONS
// ************************************************************

void release_blister () {
	int steps_to_do = blisters_steps_limit / blisters.get_step_accuracy();
	blisters.set_direction (!default_directionB);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	} 

	// SHAKE to fit the blister (2 is blister position)
	int tempYcycles = get_cycle_Ypos_from_index(2);
	int tempYsteps = get_step_Ypos_from_index(2); 
	
	Yaxis.got_to_position (tempYcycles, tempYsteps+steps_to_move_when_blister_falls);
	delay (100);
	Yaxis.got_to_position (tempYcycles, tempYsteps-steps_to_move_when_blister_falls);
	delay (100);
	Yaxis.got_to_position (tempYcycles, tempYsteps);

	
	blisters.set_direction (default_directionB);
	for (int i = 0 ; i< steps_to_do + 50; i++) {
		blisters.do_step();
		delayMicroseconds (motor_speed_blisters);
	}
}