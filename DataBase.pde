//*********************
// DEFINE DATABASE
//*********************
#include "WProgram.h"
#include <EEPROM.h>
#include <DB.h>
#include <string.h>

DB db;

#define MY_TBL 1
#define number_of_positions 20

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
	Serial.print("Number of records in DB: ");Serial.println(db.nRecs(),DEC);
}

void Show_all_records()
{
	if (db.nRecs()) Serial.println("-----");
	for (int i = 1; i <= db.nRecs(); i++)
	{
		db.read(i, DB_REC mposition);
		Serial.print("Memory position: "); Serial.print(i); 
		Serial.print(" * Xc: "); Serial.print(mposition.Xc);
		Serial.print(" * Xf: "); Serial.print(mposition.Xf);
		Serial.print(" ** Yc: "); Serial.print(mposition.Yc);
		Serial.print(" * Yf: "); Serial.println(mposition.Yf);
		Serial.println("-----");  
	} 
}