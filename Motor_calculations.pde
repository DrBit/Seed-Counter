
/*****************************************************************************
 * Stepper motor driver, increment/decrement the position and outputs the
 * correct signals to stepper motor.
 *
 * - AppNote:            Linear speed control of stepper motor
 *
 *
 * $Name: Stepper_ac $
 * $Revision: 0.5 $
 *****************************************************************************/
  // Define variables for motor handling
 /*! \brief Holding data used by timer interrupt for speed ramp calculation.
 *
 *  Contains data used by timer interrupt to calculate speed profile.
 *  Data is written to it by speed_cntr_Move(), when stepper motor is moving (timer
 *  interrupt running) data is read/updated when calculating a new step_delay
 */


#define TRUE 1
#define FALSE 0

////////////////////////
// VARIABLES
////////////////////////
// These are being used inside an interruption hence the volatile prefix
//! True when stepper motor is running.
volatile unsigned char glob_running = 1;

//! What part of the speed ramp we are in.
unsigned char srd_run_state = 3;
//! Direction stepper motor should move.
unsigned char srd_dir = 1;
//! Peroid of next timer delay. At start this value set the accelration rate.
unsigned int srd_step_delay;
//! What step_pos to start decelaration
unsigned int srd_decel_start;
//! Sets deceleration rate.
signed int srd_decel_val;
//! Minimum time delay (max speed)
signed int srd_min_delay;
//! Counter used when accelerateing/decelerateing to calculate step_delay.
volatile signed int srd_accel_count;
///////////////////////////////////


///////////////////////////////////
// MATH DEFINES
//////////////////////////////////
#define FULLSTEPS // not necessary
// Direction of stepper motor movement
#define CW  0
#define CCW 1
// ARDUINO INTERRUPT FREQ
// Timer/Counter 1 running on 16 MHz / 64 = 250 kHz (500nS - 0.5uS). (T1-FREQ 250000)
// real one #define T1_FREQ 2000000   // Timer 1 Frequency in Hz
#define T1_FREQ 250000   // Timer 1 Frequency in Hz
//! Number of (full)steps per round on stepper motor in use.
#define SPR 400    // DEPENDS ON EACH MOTOR


// Maths constants. To simplify maths when calculating in speed_cntr_Move().
#define ALPHA (2*3.14159/SPR)                    // 2*pi/spr
#define A_T_x100 ((long)(ALPHA*T1_FREQ*100))     // (ALPHA / T1_FREQ)*100
#define T1_FREQ_148 ((int)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676
#define A_SQ (long)(ALPHA*2*10000000000LL)       // ALPHA*2*10000000000
#define A_x20000 (int)(ALPHA*20000)              // ALPHA*20000
// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3
///////////////////////////////////////



/*! \brief Move the stepper motor a given number of steps.
 *
 *  Makes the stepper motor move the given number of steps.
 *  It accelrate with given accelration up to maximum speed and decelerate
 *  with given deceleration so it stops at the given step.
 *  If accel/decel is to small and steps to move is to few, speed might not
 *  reach the max speed limit before deceleration starts.
 *
 *  \param step  Number of steps to move (pos - CW, neg - CCW).
 *  \param accel  Accelration to use, in 0.01*rad/sec^2.
 *  \param decel  Decelration to use, in 0.01*rad/sec^2.
 *  \param speed  Max speed, in 0.01*rad/sec.
 */
void speed_cntr_Move(signed int step_, unsigned int accel, unsigned int decel, unsigned int speed_)
{
	//! Number of steps before we hit max speed.
	unsigned int max_s_lim;
	//! Number of steps before we must start deceleration (if accel does not hit max speed).
	unsigned int accel_lim;

	// Set direction from sign on step_ value.
	if(step_ < 0){
		srd_dir = CCW;
		step_ = -step_;
	}else{
		srd_dir = CW;
	}

	// If moving only 1 step.
	if(step_ == 1){
		// Move one step...
		srd_accel_count = -1;
		// ...in DECEL state.
		srd_run_state = DECEL;
		// Just a short delay so main() can act on 'running'.
		srd_step_delay = 1000;
		glob_running = TRUE;
		OCR1A = 100;
		// Run Timer/Counter 1 with prescaler = 8. 
		//TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
		// No prescaler (p.134)
		TCCR1B = (TCCR1B & ~(_BV(CS12)) | _BV(CS11) | _BV(CS10));
	} else if(step_ != 0) {			// Only move if number of steps to move is not zero.
		// Refer to documentation for detailed information about these calculations.

		// Set max speed limit, by calc min_delay to use in timer.
		// min_delay = (alpha / tt)/ w
		srd_min_delay = A_T_x100 / speed_;

		// Set accelration by calc the first (c0) step delay .
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
		srd_step_delay = (T1_FREQ_148 * sqrt_(A_SQ / accel))/100;                   
		// Find out after how many steps does the speed hit the max speed limit.
		// max_s_lim = speed^2 / (2*alpha*accel)
		max_s_lim = (long)speed_*speed_/(long)(((long)A_x20000*accel)/100);
		// If we hit max speed limit before 0,5 step it will round to 0.
		// But in practice we need to move atleast 1 step to get any speed at all.
		if(max_s_lim == 0){
			max_s_lim = 1;
		}

		// Find out after how many steps we must start deceleration.
		// n1 = (n1+n2)decel / (accel + decel)
		accel_lim = ((long)step_*decel) / (accel+decel);
		// We must accelrate at least 1 step before we can start deceleration.
		if(accel_lim == 0){
			accel_lim = 1;
		}

		// Use the limit we hit first to calc decel.
		if(accel_lim <= max_s_lim){
			srd_decel_val = accel_lim - step_;
		}else{
			srd_decel_val = -((long)max_s_lim*accel)/decel;
		}
		// We must decelrate at least 1 step to stop.
		if(srd_decel_val == 0){
			srd_decel_val = -1;
		}

		// Find step to start decleration.
		srd_decel_start = step_ + srd_decel_val;

		// If the maximum speed is so low that we dont need to go via accelration state.
		if(srd_step_delay <= srd_min_delay){
			srd_step_delay = srd_min_delay;
			srd_run_state = RUN;
		}else{
			srd_run_state = ACCEL;
		}

		// Reset counter.
		srd_accel_count = 0;
		glob_running = true;
		OCR1A = 0xFFFF;
		// Set Timer/Counter to divide clock by 8
		//TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
		// No prescaler (p.134)
		//TCCR1B = (TCCR1B & ~(_BV(CS12) | _BV(CS11))) | _BV(CS10);
		//prescaler 64
		TCCR1B = (TCCR1B & ~(_BV(CS12)) | _BV(CS11) | _BV(CS10));
	}
}

/*! \brief Init of Timer/Counter1.
 *
 *  Set up Timer/Counter1 to use mode 1 CTC and
 *  enable Output Compare A Match Interrupt.
 */
void speed_cntr_Init_Timer1(void)
{
	// Tells what part of speed ramp we are in.
	srd_run_state = STOP;
	/*
	Serial.println("Setup timer");
	// Timer/Counter 1 in mode 4 CTC (Not running).
	TCCR1B = (1<<WGM12);
	// Timer/Counter 1 Output Compare A Match Interrupt enable.
	TIMSK1 = (1<<OCIE1A);
	*/
	cli();
	unsigned char sreg;
	sreg = SREG;
	// Set CTC mode (Clear Timer on Compare Match) (p.133)
	// Have to set OCR1A *after*, otherwise it gets reset to 0!
	TCCR1B = (TCCR1B & ~_BV(WGM13)) | _BV(WGM12);
	TCCR1A = TCCR1A & ~(_BV(WGM11) | _BV(WGM10));
	// Enable interrupt when TCNT1 == OCR1A (p.136)
	TIMSK1 |= _BV(OCIE1A);  
	SREG = sreg;
	sei();
}
  
  
/*! \brief Timer/Counter1 Output Compare A Match Interrupt.
 *
 *  Timer/Counter1 Output Compare A Match Interrupt.
 *  Increments/decrements the position of the stepper motor
 *  exept after last position, when it stops.
 *  The \ref step_delay defines the period of this interrupt
 *  and controls the speed of the stepper motor.
 *  A new step delay is calculated to follow wanted speed profile
 *  on basis of accel/decel parameters.
 */

// Arduino Interrupt handling
ISR(TIMER1_COMPA_vect) 
{
	// Holds next delay period.
	unsigned int new_step_delay;
	// Remember the last step delay used when accelrating.
	static int last_accel_delay;
	// Counting steps when moving.
	static unsigned int step_count = 0;
	// Keep track of remainder from new_step-delay calculation to incrase accurancy
	static unsigned int rest = 0;

	OCR1A = srd_step_delay;

	switch(srd_run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      // Stop Timer/Counter 1.
      TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
      glob_running = false;
      break;

    case ACCEL:
      sm_driver_StepCounter(srd_dir);
      step_count++;
      srd_accel_count++;
      new_step_delay = srd_step_delay - (((2 * (long)srd_step_delay) + rest)/(4 * srd_accel_count + 1));
      rest = ((2 * (long)srd_step_delay)+rest)%(4 * srd_accel_count + 1);
      // Chech if we should start decelration.
      if(step_count >= srd_decel_start) {
        srd_accel_count = srd_decel_val;
        srd_run_state = DECEL;
      }
      // Chech if we hitted max speed.
      else if(new_step_delay <= srd_min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd_min_delay;
        rest = 0;
        srd_run_state = RUN;
      }
      break;

    case RUN:
      sm_driver_StepCounter(srd_dir);
      step_count++;
      new_step_delay = srd_min_delay;
      // Chech if we should start decelration.
      if(step_count >= srd_decel_start) {
        srd_accel_count = srd_decel_val;
        // Start decelration with same delay as accel ended with.
        new_step_delay = last_accel_delay;
        srd_run_state = DECEL;
      }
      break;

    case DECEL:
      sm_driver_StepCounter(srd_dir);
      step_count++;
      srd_accel_count++;
      new_step_delay = srd_step_delay - (((2 * (long)srd_step_delay) + rest)/(4 * srd_accel_count + 1));
      rest = ((2 * (long)srd_step_delay)+rest)%(4 * srd_accel_count + 1);
      // Check if we at last step
      if(srd_accel_count >= 0){
        srd_run_state = STOP;
      }
      break;
  }
  srd_step_delay = new_step_delay;
}


/*! \brief Square root routine.
 *
 * sqrt routine 'grupe', from comp.sys.ibm.pc.programmer
 * Subject: Summary: SQRT(int) algorithm (with profiling)
 *    From: warwick@cs.uq.oz.au (Warwick Allison)
 *    Date: Tue Oct 8 09:16:35 1991
 *
 *  \param x  Value to find square root of.
 *  \return  Square root of x.
 */
static unsigned long sqrt_(unsigned long x)
{
	register unsigned long xr;	// result register
	register unsigned long q2;	// scan-bit register
	register unsigned char f;		// flag (one bit)

	xr = 0;									// clear result
	q2 = 0x40000000L;				// higest possible result bit
	while(q2 >>= 2) 					// shift twice
	{
		if((xr + q2) <= x)
		{
			x -= xr + q2;
			f = 1;							// set flag
		}else{
			f = 0;							// clear flag
		}
		xr >>= 1;
		if(f){
			xr += q2;						// test flag
		}
	} 
	if(xr < x){
		return xr +1;					// add for rounding
	}else{
		return xr;
	}
}


////////////////////////////////////////////////////////////
// This function comes back from the acceleration function with a proper timing to 
// do steps on the motors
// As we dont have any way of knowing which motor we are moving back from this function
// we preselect the motor (0,1,2) before doing the movement
/////////////////////////////////////////////////////////////
void sm_driver_StepCounter(unsigned char Mdirection) {
	if (motor_select == 0) {
		counter.set_direction (Mdirection);
		counter.do_step();
	}else if (motor_select == 1) {
		Xaxis.set_direction (!Mdirection);
		Xaxis.do_step();
	}else if (motor_select == 2) {
		Yaxis.set_direction (!Mdirection);
		Yaxis.do_step();
	}
}


// ************************************************************
// ** NEW move function
// ************************************************************

/***** Main fucntion, move motor a certain number of steps and cicles at a designed direction  *****/
void move_motor(int motor_number, unsigned int cycles,unsigned int steps, int accel_factor, boolean direction)
// Still to implement selection of motor!! probably this piece of code should be inside the library.
{
		// we set direction
		Xaxis.set_direction (!direction);

#if defined DEBUG
		Serial.println ("\n ****** Movement started ********");
		Serial.print (" * Direction set: ");
		Serial.println (direction,DEC);
#endif	
	// accel factor standard = 40
	// Some previos calculations
	// 920*2 = 1840
	unsigned int total_steps_cycles_for_aceleration = (accel_factor + (accel_factor*2) + (accel_factor*4) + (accel_factor*8*2))*2;
	unsigned int total_steps_for_aceleration = total_steps_cycles_for_aceleration % 1600;
	unsigned int total_cycles_for_aceleration = total_steps_cycles_for_aceleration / 1600;
	
	if ((total_cycles_for_aceleration > cycles) || ((total_steps_for_aceleration > steps) && (total_cycles_for_aceleration == cycles))) {		// Meaning that we need to move less than we need to just acelerate
#if defined DEBUG
		Serial.println (" * Switch into plain mode, no acceleration.");
#endif
		// Calculation total amount of steps
		unsigned int total_steps_to_move = (cycles*1600)+steps;
		move_n_steps_slow (total_steps_to_move);
#if defined DEBUG
		Serial.print (" * Total moved: ");
		print_x_pos ();
#endif
		
	}else{
#if defined DEBUG
		Serial.println (" * Switch into acceleratde mode.");
		Serial.print ("Total to move:  ");
		Serial.print(steps);
		Serial.print (" steps and  ");
		Serial.print(cycles);
		Serial.println (" cycles.");
		Serial.print ("Acceleration will consume a total of: ");
		Serial.print(total_steps_for_aceleration);
		Serial.print (" steps and ");		
		Serial.print(total_cycles_for_aceleration);
		Serial.println (" cyces.");	
#endif

		// firs we prepare all calculations so we dont need to calculate in the middle
		// removing fixed steps to accelerate from the total steps to move
		if (steps > total_steps_for_aceleration) {				// If we have less steps that we got to rest then whe need to substract one cycle and use those steps to acomplish the rest
			steps = steps - total_steps_for_aceleration;
		}else{
			total_steps_for_aceleration = total_steps_for_aceleration - steps;
			steps = 1600;
			cycles --;
			steps = steps - total_steps_for_aceleration;
		}
		cycles = cycles - total_cycles_for_aceleration;
		
#if defined DEBUG
		Serial.println (" * As a result we will be moving at max speed a total of: ");
		Serial.print(steps);
		Serial.print (" steps and ");
		Serial.print(cycles);
		Serial.println (" cycles.");
#endif

		// we start the ramp up and achieve certain velocity
		ramp_up (accel_factor);												// 920 steps FIXED

#if defined DEBUG
		Serial.println (" * Finished ramp up ");
		Serial.print ("Actual position: ");
		print_x_pos ();
#endif	
		// at a certain velocity we move the steps
		int remaining_steps = move_n_steps_fast (steps) ;
		// inside "remaining_steps" there is the steps we couldnt do beacuse of a lack of resolution
		// we will move them slowlty at the end because now we are going at max speed.
#if defined DEBUG
		Serial.println (" * Finished move steps at top speed ");
		Serial.print ("Actual position: ");
		print_x_pos ();
		Serial.print ("Remaining to move: ");
		Serial.println (remaining_steps);
#endif	
		// then we move all cycles also at max speed
		for (int a = cycles; a > 0; a--) {
			move_n_steps_fast (1600) ;			
		}
#if defined DEBUG
		Serial.println (" * Finished move cycles at top speed ");
		Serial.print ("Actual position: ");
		print_x_pos ();
#endif	
		
		// once we moved all steps we start the ramp down
		ramp_down(accel_factor);											// 920 steps FIXED
#if defined DEBUG
		Serial.println (" * Finished ramp down ");
		Serial.print ("Actual position: ");
		print_x_pos ();
#endif

		//Now that we are almost stop and the resolution of the motor is max, we move the steps we missed at max speed
		move_n_steps_slow (remaining_steps);

		#if defined DEBUG
		Serial.print (" * Moved remaingin steps: ");
		Serial.println (remaining_steps);
		Serial.print ("Actual position: ");
		print_x_pos ();
#endif
	}
}

/***** Ramping up, achieve velocity  *****/
void ramp_up (int accel_factor) {
	for (int i = 1; i< 6; i++) {
		select_case (i, accel_factor);
	}
}

/***** Ramping down, decreasing velocity  *****/
void ramp_down (int accel_factor) {
	for (int i = 5; i> 0; i--) {
		select_case (i, accel_factor);
	} 
}

/***** Move N steps at the max velocity *****/
int move_n_steps_fast (unsigned int mov_steps) {
	Xaxis.change_step_mode(1);		// Change mode 1
	for (int a = 0; a < mov_steps/Xaxis.get_step_accuracy(); a++) {
		Xaxis.do_step();
		delayMicroseconds (490);
	}
	return (mov_steps % Xaxis.get_step_accuracy());
}

/***** Move N steps at the min velocity *****/
void move_n_steps_slow (unsigned int mov_steps){
	Xaxis.change_step_mode(8);		// Change mode 8 steps 
	// Executing steps
	for (int a = 0; a < mov_steps; a++) {
		Xaxis.do_step();
		delayMicroseconds (260);
	}
}

// ************************************************************
// ** Extra functions for ramping and testing
// ************************************************************

/***** Ramping cases for changin velocities *****/
void select_case (int vel_case, int steps_x_case) {
	//const int steps_x_case = 40;  // ol variable for defining number of steps to do in each velocity step

	// for acelerating or deacelerating (for both = result*2)
	// formula = steps_x_case + (steps_x_case*2) + (steps_x_case*4) + (steps_x_case*8*2) 
	
	if (vel_case == 1) {
		Xaxis.change_step_mode(8);		// Change mode 8
		for (int a = 0; a < steps_x_case; a++) {
			Xaxis.do_step();
			delayMicroseconds (260);
		}
	}
	if (vel_case == 2) {
		Xaxis.change_step_mode(4);		// Change mode 4
		for (int a = 0; a < steps_x_case; a++) {
			Xaxis.do_step();
			delayMicroseconds (350);
		}
	}
	if (vel_case == 3) {
		Xaxis.change_step_mode(2);		// Change mode 2
		for (int a = 0; a < steps_x_case; a++) {
			Xaxis.do_step();
			delayMicroseconds (450);
		}
	}
	if (vel_case == 4) {
		Xaxis.change_step_mode(1);		// Change mode 1
		for (int a = 0; a < steps_x_case; a++) {
			Xaxis.do_step();
			delayMicroseconds (560);
		}
	}
	if (vel_case == 5) {
		Xaxis.change_step_mode(1);		// Change mode 1
		for (int a = 0; a < steps_x_case; a++) {
			Xaxis.do_step();
			delayMicroseconds (500);
		}
	}
}