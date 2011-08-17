// ***********************
// ** Physical limits of the blister motors
// ***********************

#define blisters_steps_limit 300
#define blisters_steps_absoulut_limit 1000


// ***********************
// ** Blister timings
// ***********************

#define blisters_timing 2000		// Timing in betwen steps for the blister motors


// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************

boolean blisters_init () {
	int steps_to_do = (blisters_steps_absoulut_limit+100) / blisters.get_step_accuracy();		// The absolut limit would be 1000, but we add an extra 100 to be sure that we hit the maximum point so we init correctly
	blisters.set_direction (true);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (blisters_timing);
	} 
	blisters.set_init_position();
	return true;
}

// ************************************************************
// ** USEFUL FUNCTIONS
// ************************************************************

void release_blister () {
	int steps_to_do = blisters_steps_limit / blisters.get_step_accuracy();
	blisters.set_direction (false);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (blisters_timing);
	} 
	blisters.set_direction (true);
	for (int i = 0 ; i< steps_to_do; i++) {
		blisters.do_step();
		delayMicroseconds (blisters_timing);
	}
}