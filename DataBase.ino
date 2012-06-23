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

#define MY_TBL 20
#define NET_TBL 1
#define number_of_positions 41


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
	Serial.print(F("Number of records in DB: "));Serial.println(db.nRecs(),DEC);
	show_pos_list ();
	if (db.nRecs()) Serial.println(F("\nDATA RECORDED IN INTERNAL MEMORY:"));
	for (int i = 1; i <= db.nRecs(); i++)
	{
		db.read(i, DB_REC mposition);
		Serial.print(F("Memory position: ")); Serial.print(i); 
		Serial.print(F(" * Xc: ")); Serial.print(mposition.Xc);
		Serial.print(F(" * Xf: ")); Serial.print(mposition.Xf);
		Serial.print(F(" * Yc: ")); Serial.print(mposition.Yc);
		Serial.print(F(" * Yf: ")); Serial.println(mposition.Yf);  
	} 
	Serial.println(F("-----"));
}


void show_pos_list ()  {
	Serial.println(F("List of position numbers"));
	Serial.println(F("********************************************"));
	Serial.println(F("* Pos number * Description"));
	Serial.println(F("********************************************"));
	Serial.println(F("*     1.     * INIT POSITION"));	
	Serial.println(F("*     2.     * Right before the blisters"));
	Serial.println(F("*     3.     * Printer position"));
	Serial.println(F("*     4.     * Exit position"));
	Serial.println(F("*****************************************10Holes"));
	Serial.println(F("*     5.     * Hole 1"));
	Serial.println(F("*     6.     * Hole 2"));
	Serial.println(F("*     7.     * Hole 3"));
	Serial.println(F("*     8.     * Hole 4"));
	Serial.println(F("*     9.     * Hole 5"));
	Serial.println(F("*     10.    * Hole 6"));
	Serial.println(F("*     11.    * Hole 7"));
	Serial.println(F("*     12.    * Hole 8"));
	Serial.println(F("*     13.    * Hole 9"));
	Serial.println(F("*     14.    * Hole 10"));
	Serial.println(F("******************************************5Holes"));
	Serial.println(F("*     15.    * Hole 1"));
	Serial.println(F("*     16.    * Hole 2"));
	Serial.println(F("*     17.    * Hole 3"));
	Serial.println(F("*     18.    * Hole 4"));
	Serial.println(F("*     19.    * Hole 5"));
	Serial.println(F("********************************************Extra Positions"));
	Serial.println(F("*     20.    * Brush position"));
	Serial.println(F("********************************************"));
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
	*/
	
}


struct MyRec1 {
	// Containig
	byte LOCAL_IP[4];				// example: {10,250,1,8}
	byte UI_IP[4];					// example: {10,250,1,8}
	unsigned int UI_port;			// example: 8000
	byte machine_id;				// example: 1
} config;
// When changing the structure data in the eeprom needs to be rewritten

void init_NET_DB () {
	db.create(NET_TBL,sizeof(config),1);
	db.open(NET_TBL);
	db.read(1, DB_REC config);			// Read configuration (1rst line)
}

void NET_DB_REC () {
	db.write(1, DB_REC config);
}
