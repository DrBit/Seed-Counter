
#include <Servo.h> 
Servo myservo1;  // create servo object to control a servo 
Servo myservo2;  // create servo object to control a servo 

#define direction 12
#define step 13


void setup () {
  pinMode (direction, OUTPUT);
  pinMode (step, OUTPUT);
  myservo1.attach(9);  // attaches the servo on pin 9 to the servo object 
  myservo2.attach(10);  // attaches the servo on pin 9 to the servo object 
}

void loop () {
 
  digitalWrite (direction, LOW);
  //do_steps (9400,200);

  delay(1000); 

  myservo1.write(100);                  // sets the servo position according to the scaled value 
  myservo2.write(40);                  // sets the servo position according to the scaled value
  delay(1000);                           // waits for the servo to get there 
  myservo1.write(140);                  // sets the servo position according to the scaled value 
  myservo2.write(0);                  // sets the servo position according to the scaled value
  delay(1000);                           // waits for the servo to get there 
  
  //delay(3000); 
  
  digitalWrite (direction, HIGH);
  //do_steps (9400,200);
  //delay(1000);

  
}

void do_steps (int number_steps, int time) {
 for (int a = 0; a < number_steps; a++) {
  digitalWrite (step,HIGH);
  delayMicroseconds (time);
  digitalWrite (step, LOW);
  delayMicroseconds (time);
  } 
}
