// ************************************************************
// ** INIT FUNCTIONS
// ************************************************************

boolean blisters_init () {
  int steps_to_do = blisters_steps_absoulut_limit / blisters.get_step_accuracy();
  blisters.set_direction (true);
  for (int i = 0 ; i< steps_to_do; i++) {
   blisters.do_step();
   delayMicroseconds (2500);
  } 
  blisters.set_init_position();
  return true;
}

// RECHEK
void release_blister () {
  int steps_now = 1600;
  blisters.set_direction (true);
  for (int i = 0 ; i< steps_now; i++) {
    blisters.do_step();
    delayMicroseconds (40);
  } 
  steps_now = 1000;
  blisters.set_direction (false);
  for (int i = 0 ; i< steps_now; i++) {
    blisters.do_step();
    delayMicroseconds (40);
  }
}

