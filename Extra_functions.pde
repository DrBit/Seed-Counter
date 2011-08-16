// ************************************************************
// ** EXTRA FUNCTIONS  (NOT USED)
// ************************************************************

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
  
  delayMicroseconds(390);
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
  //delayMicroseconds(390);
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
