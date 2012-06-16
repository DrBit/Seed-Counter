//*********************
// DEFINE DATABASE
//*********************

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
#include <EEPROM.h>
#include <DB.h>
#include <string.h>

DB db;

#define MY_TBL 1
#define number_of_positions 23

struct MyRec {
// Containig
// X coarse - X fine
// Y coarse - Y fine
  unsigned int Xc;
  unsigned int Xf;
  unsigned int Yc;
  unsigned int Yf;
} mposition;

void init_DB () {
	db.create(MY_TBL,sizeof(mposition),number_of_positions);
	db.open(MY_TBL);
}

void Show_all_records()
{
	Serial.print("Number of records in DB: ");Serial.println(db.nRecs(),DEC);
	show_pos_list ();
	if (db.nRecs()) Serial.println("\nDATA RECORDED IN INTERNAL MEMORY:");
	for (int i = 1; i <= db.nRecs(); i++)
	{
		db.read(i, DB_REC mposition);
		Serial.print("Memory position: "); Serial.print(i); 
		Serial.print(" * Xc: "); Serial.print(mposition.Xc);
		Serial.print(" * Xf: "); Serial.print(mposition.Xf);
		Serial.print(" * Yc: "); Serial.print(mposition.Yc);
		Serial.print(" * Yf: "); Serial.println(mposition.Yf);  
	} 
	Serial.println("-----");
}


void show_pos_list ()  {
	Serial.println("List of position numbers");
	Serial.println("********************************************");
	Serial.println("* Pos number * Description");
	Serial.println("********************************************");
	Serial.println("*     1.     * INIT POSITION");	
	Serial.println("*     2.     * Right before the blisters");			
	Serial.println("*     3.     * Printer position");					
	Serial.println("*     4.     * Exit position");						
	Serial.println("*****************************************10Holes");
	Serial.println("*     5.     * Hole 1");
	Serial.println("*     6.     * Hole 2");
	Serial.println("*     7.     * Hole 3");
	Serial.println("*     8.     * Hole 4");
	Serial.println("*     9.     * Hole 5");
	Serial.println("*     10.    * Hole 6");
	Serial.println("*     11.    * Hole 7");
	Serial.println("*     12.    * Hole 8");
	Serial.println("*     13.    * Hole 9");
	Serial.println("*     14.    * Hole 10");
	Serial.println("******************************************5Holes");
	Serial.println("*     15.    * Hole 1");
	Serial.println("*     16.    * Hole 2");
	Serial.println("*     17.    * Hole 3");
	Serial.println("*     18.    * Hole 4");
	Serial.println("*     19.    * Hole 5");
	Serial.println("********************************************Extra Positions");
	Serial.println("*     20.    * Brush position");
	Serial.println("********************************************");
}


void manual_data_write () {
    
	// MANUAL WRITE OF Position 20 (Brush)
	int position_n = 20;			// Hole number
	mposition.Xc = 8;				// Xcycles
	mposition.Xf = 589;  			// Csteps
	mposition.Yc = 13;				// Ycycles
	mposition.Yf = 1520;			// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");

	/*
	// MANUAL WRITE OF EEPROM WITH ALL POSITIONS
	
	int _1RowYc = 2;
	int _1RowYf = 25;
	
	int _2RowYc = 10;
	int _2RowYf = 630;
	
	
	int position_n = 5;			// Hole number
	mposition.Xc = 6;		// Xcycles
	mposition.Xf = 1558;  	// Csteps
	mposition.Yc = _1RowYc;		// Ycycles
	mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 6;			// Hole number
	mposition.Xc = 6;		// Xcycles
	mposition.Xf = 1558;  	// Csteps
	mposition.Yc = _2RowYc;		// Ycycles
	mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================

	position_n = 7;			// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 250;  	// Csteps
	mposition.Yc = _2RowYc;		// Ycycles
	mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 8;			// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 250;  	// Csteps
	mposition.Yc = _1RowYc;		// Ycycles
	mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 9;			// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 590;  	// Csteps
	mposition.Yc = _1RowYc;		// Ycycles
	mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 10;			// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 590;  	// Csteps
	mposition.Yc = _2RowYc;		// Ycycles
	mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 11;		// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 920;  	// Csteps
	mposition.Yc = _2RowYc;		// Ycycles
	mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 12;		// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 920;  	// Csteps
	mposition.Yc = _1RowYc;		// Ycycles
	mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 13;		// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 1235;  	// Csteps
	mposition.Yc = _1RowYc;		// Ycycles
	mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 14;		// Hole number
	mposition.Xc = 7;		// Xcycles
	mposition.Xf = 1235;  	// Csteps
	mposition.Yc = _2RowYc;		// Ycycles
	mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	*/
	
}