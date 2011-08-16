// ************************************************************
// ** EXTRA FUNCTIONS  (NOT USED)
// ************************************************************
boolean holdX = false;
boolean holdY = false; 
boolean holdX1 = false;
boolean holdY1 = false; 
boolean holdX2 = false;
boolean holdY2 = false;
boolean motorA_enabled = true;

/***** MANUAL MODE XY AXIS  *****/
void manual_modeXY() {
  if (digitalRead(button1) == HIGH) {
    motorA_enabled =true;
  }else{
    motorA_enabled =false;
  }
  // Botton 2 moves motor BACKWARDS
  if (digitalRead(button2) == HIGH) {
    if (motorA_enabled) {
      if ((Xaxis.get_steps_cycles() >= 0) && (Xaxis.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
        Xaxis.set_direction (true);   // Goes backward towards the sensor
        Xaxis.do_step();
      }
      holdX1 = true;
    }else{
      if ((Yaxis.get_steps_cycles() >= 0) && (Yaxis.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
        Yaxis.set_direction (true);   // Goes backward towards the sensor
        Yaxis.do_step();
      }
      holdY1 = true;
    } 
    delayMicroseconds(18);
  }else{
    if (holdX1) {
      holdX1 = false;
      print_x_pos();
    }else if (holdY1) {    
      holdY1 = false;
      print_y_pos();
    }
  }
  // Botton 3 moves motor FORWARD
  if (digitalRead(button3) == HIGH) {
    if (motorA_enabled) {
      if (Xaxis.get_steps_cycles() < Xaxis_cycles_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
        Xaxis.set_direction (false);   // Goes forward
        Xaxis.do_step();
      }
      holdX2 = true;
    }else{
      if (Yaxis.get_steps_cycles() < Yaxis_cycles_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
        Yaxis.set_direction (false);   // Goes forward 
        Yaxis.do_step();
      }
      holdY2 = true;
    }
    delayMicroseconds(18);
  }else{
    if (holdX2) {
      holdX2 = false;
      print_x_pos();
    }else if (holdY2) {    
      holdY2 = false;
      print_y_pos();
    }
  }
  delayMicroseconds(1990);
}


/***** MANUAL MODE BLISTERS  *****/
boolean holdC1 = false;
boolean holdC2 = false;

void manual_mode_blisters() {
  // Botton 2 moves motor BACKWARDS
  if (digitalRead(button2) == HIGH) {
    if ((blisters.get_steps_cycles() >= 0) && (blisters.get_steps() > 0)) {// If the position is bigger than 0 then we can move backwards
      blisters.set_direction (true);   // Goes backward towards the sensor
      blisters.do_step();
    }
    holdC1 = true;
    delayMicroseconds(18);
  }else{
    if (holdC1) {
      holdC1 = false;
      print_blisters_pos();
    }
  }
  // Botton 3 moves motor FORWARD
  if (digitalRead(button3) == HIGH) {
    if (blisters.get_steps() < blisters_steps_limit)  { // If the position is lesser than defined in Yaxis_cycles_limit then we can move forwards
      blisters.set_direction (false);   // Goes forward
      blisters.do_step();
    }
    holdC2 = true;
    delayMicroseconds(18);
  }else{
    if (holdC2) {
      holdC2 = false;
      print_blisters_pos();
    }
  }
  delayMicroseconds(3990);
}

/***** Print Position X axis  *****/
void print_x_pos () {
  Serial.print ("* X_cycles: ");
  Serial.print (Xaxis.get_steps_cycles());
  Serial.print (", X_steps: ");
  Serial.println (Xaxis.get_steps());
}

/***** Print Position Y axis  *****/
void print_y_pos () {
  Serial.print ("* Y_cycles: ");
  Serial.print (Yaxis.get_steps_cycles());
  Serial.print (", Y_steps: ");
  Serial.println (Yaxis.get_steps());
}

/***** Print Position Blisters axis  *****/
void print_blisters_pos () {
  Serial.print ("* Y_cycles: ");
  Serial.print (blisters.get_steps_cycles());
  Serial.print (", Y_steps: ");
  Serial.println (blisters.get_steps());
}

/***** Print Position Counter axis  *****/
void print_counter_pos () {
  Serial.print ("* Couter cycles: ");
  Serial.print (counter.get_steps_cycles());
  Serial.print (", Counter steps: ");
  Serial.println (counter.get_steps());
}

/***** Print Position Counter axis  *****/
/***** NOT FINISHED!!!!!!!!!!!!!!!  *****/
boolean holdD1 = false;
boolean holdD2 = false;
void manual_modeCounter() {

  if (digitalRead(button1) == HIGH) {
    holdD2 = true;
  }
  else{    
    delayMicroseconds(190);
    if (holdD1) {
      holdD1 = false;
      print_counter_pos ();
    }
  }

  if (digitalRead(button2) == HIGH) {
    holdD2 = true;
  }
  else{    
    delayMicroseconds(190);
    if (holdD2) {
      holdD2 = false;
      print_counter_pos ();
    }
  }
  delayMicroseconds(390);
}

// Memory check
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void mem_check () {
  //checking memory:
  Serial.println("\n[memCheck]");
  Serial.println(freeRam());
}
