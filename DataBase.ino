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

// Starting position of the table. Each value in structure must be count
// byte counts as 1, int as 2 etc....
#define XYPOS_TBL 55
#define NET_TBL 1
#define SERVO_COUNT_TBL 20
#define number_of_positions 41


struct MyRec {
// Containig
// X coarse - X fine
// Y coarse - Y fine
  unsigned int Xc;
  unsigned int Xf;
  unsigned int Yc;
  unsigned int Yf;
} DB_mposition;

struct MyRec1 {
	// Containig
	byte LOCAL_IP[4];				// example: {10,250,1,8}
	byte UI_IP[4];					// example: {10,250,1,8}
	unsigned int UI_port;			// example: 8000
	byte machine_id;				// example: 1
} DB_Net_config;

struct MyRec2 {
	// Containig
	// Counter records
	unsigned int pick_seed_positionDB;
	unsigned int detect_seed_positionDB;
	unsigned int margin_steps_to_detect_seedDB;
	unsigned int drop_positionDB;
	// Sevo records
	unsigned int servoR_openDB;
	unsigned int servoL_openDB;
	unsigned int servoR_closeDB;
	unsigned int servoL_closeDB;
	unsigned int servoEjection_openDB;
	unsigned int servoEjection_closeDB;
} DB_servo_counter;



// When changing the structure data in the eeprom needs to be rewritten


void init_DBs () {
	db.create(NET_TBL,sizeof(DB_Net_config),1);
	db.create(SERVO_COUNT_TBL,sizeof(DB_servo_counter),1);
	db.create(XYPOS_TBL,sizeof(DB_mposition),number_of_positions);
}

void Show_all_records() {
	db.open(XYPOS_TBL);
	Serial.print(F("Number of records in DB: "));Serial.println(db.nRecs(),DEC);
	show_pos_list ();
	if (db.nRecs()) Serial.println(F("\nDATA RECORDED IN INTERNAL MEMORY:"));
	for (int i = 1; i <= db.nRecs(); i++)
	{
		db.read(i, DB_REC DB_mposition);
		Serial.print(F("Memory position: ")); Serial.print(i); 
		Serial.print(F(" * Xc: ")); Serial.print(DB_mposition.Xc);
		Serial.print(F(" * Xf: ")); Serial.print(DB_mposition.Xf);
		Serial.print(F(" * Yc: ")); Serial.print(DB_mposition.Yc);
		Serial.print(F(" * Yf: ")); Serial.println(DB_mposition.Yf);  
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
	Serial.println(F("*     21.    * ?"));
	Serial.println(F("*     22.    * ?"));
	Serial.println(F("*     23.    * ?"));
	Serial.println(F("********************************************"));
}


void manual_data_write () {
    
	// MANUAL WRITE OF Position 20 (Brush)
	int position_n = 20;			// Hole number
	DB_mposition.Xc = 8;				// Xcycles
	DB_mposition.Xf = 589;  			// Csteps
	DB_mposition.Yc = 13;				// Ycycles
	DB_mposition.Yf = 1520;			// Ysteps
	
	db.write(position_n, DB_REC DB_mposition);
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
	DB_mposition.Xc = 6;		// Xcycles
	DB_mposition.Xf = 1558;  	// Csteps
	DB_mposition.Yc = _1RowYc;		// Ycycles
	DB_mposition.Yf = _1RowYf;	// Ysteps
	
	db.write(position_n, DB_REC DB_mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	
	position_n = 6;			// Hole number
	DB_mposition.Xc = 6;		// Xcycles
	DB_mposition.Xf = 1558;  	// Csteps
	DB_mposition.Yc = _2RowYc;		// Ycycles
	DB_mposition.Yf = _2RowYf;	// Ysteps
	
	db.write(position_n, DB_REC DB_mposition);
	Serial.print("Position ");
	Serial.print (position_n);
	Serial.println ("recorded!");
	//=========================================================
	*/
	
}


///////////////////////////////////
// XY POSITIONS
//////////////////////////////////

int get_cycle_Xpos_from_index(int index) {
	db.open(XYPOS_TBL);
	db.read(index, DB_REC DB_mposition);
	return DB_mposition.Xc;
}

int get_step_Xpos_from_index(int index) {
	db.open(XYPOS_TBL);
	db.read(index, DB_REC DB_mposition);
	return DB_mposition.Xf;
}

int get_cycle_Ypos_from_index(int index) {
	db.open(XYPOS_TBL);
	db.read(index, DB_REC DB_mposition);
	return DB_mposition.Yc;
}

int get_step_Ypos_from_index(int index) {
	db.open(XYPOS_TBL);
	db.read(index, DB_REC DB_mposition);
	return DB_mposition.Yf;
}

void recordSetXYpositionToDB(int index) {
	// WRITE
	db.open(XYPOS_TBL);
	DB_mposition.Xc = Xaxis.get_steps_cycles();
	DB_mposition.Xf = Xaxis.get_steps();
	DB_mposition.Yc = Yaxis.get_steps_cycles();
	DB_mposition.Yf = Yaxis.get_steps();
	db.write(index, DB_REC DB_mposition);
}

void recordCustomPositionToDB (unsigned int index,unsigned int Xcoarse, unsigned int Xfine, unsigned int Ycoarse, unsigned int Yfine) {
	// Load database position
	db.open(XYPOS_TBL);
	db.read(index, DB_REC DB_mposition);
	// Compare with the real value.. do we have to update??
	if (Xcoarse != DB_mposition.Xc|| Xfine != DB_mposition.Xf || Ycoarse != DB_mposition.Yc || Yfine != DB_mposition.Yf) {		// If data is different from the eeprom
		// store data in eeprom, data is different
		DB_mposition.Xc = Xcoarse;
		DB_mposition.Xf = Xfine;
		DB_mposition.Yc = Ycoarse;
		DB_mposition.Yf = Yfine;
		
		db.write(index, DB_REC DB_mposition);
		#if defined Server_com_debug
		Serial.println (F(" - Updated!"));
		#endif
	}else{
		#if defined Server_com_debug
		Serial.println (F(" - Was correct"));
		#endif
	}
}


///////////////////////////////////
// Servos
//////////////////////////////////
void show_DBservos_data () {
	db.open(SERVO_COUNT_TBL);
	db.read(1, DB_REC DB_servo_counter);

	Serial.println(F("\n*Counter stepper positions:"));
	Serial.print(F("Pick up position: "));
	Serial.println(DB_servo_counter.pick_seed_positionDB);
	Serial.print(F("Detect position: "));
	Serial.println(DB_servo_counter.detect_seed_positionDB);
	Serial.print(F("Maring steps: "));
	Serial.println(DB_servo_counter.margin_steps_to_detect_seedDB);
	Serial.print(F("Drop position: "));
	Serial.println(DB_servo_counter.drop_positionDB);

	Serial.println(F("\n*Blister servo positions:"));
	Serial.print(F("Servo Right Open position: "));
	Serial.println(DB_servo_counter.servoR_openDB);
	Serial.print(F("Servo Left Open position: "));
	Serial.println(DB_servo_counter.servoL_openDB);
	Serial.print(F("Servo Right Close position: "));
	Serial.println(DB_servo_counter.servoR_closeDB);
	Serial.print(F("Servo Left Close position: "));
	Serial.println(DB_servo_counter.servoL_closeDB);

	Serial.println(F("\n*Ejections servo positions:"));
	Serial.print(F("Ejection Open position: "));
	Serial.println(DB_servo_counter.servoEjection_openDB);
	Serial.print(F("Ejection Close position: "));
	Serial.println(DB_servo_counter.servoEjection_closeDB);
}

void read_database_DB_servo () {
	db.open(SERVO_COUNT_TBL);
	db.read(1, DB_REC DB_servo_counter);
	servoR_open = DB_servo_counter.servoR_openDB;
	servoL_open = DB_servo_counter.servoL_openDB;
	servoR_close = DB_servo_counter.servoR_closeDB;
	servoL_close = DB_servo_counter.servoL_closeDB;
	servoEjection_open = DB_servo_counter.servoEjection_openDB;
	servoEjection_close = DB_servo_counter.servoEjection_closeDB;
}

void record_blister_servo_DB (unsigned int RO, unsigned int LO, unsigned int RC, unsigned int LC){
	db.open(SERVO_COUNT_TBL);
	DB_servo_counter.servoR_openDB = RO;
	DB_servo_counter.servoL_openDB = LO;
	DB_servo_counter.servoR_closeDB = RC;
	DB_servo_counter.servoL_closeDB = LC;
	db.write(1, DB_REC DB_servo_counter);
}

void record_ejection_servo_DB (unsigned int EO, unsigned int EC) {
	db.open(SERVO_COUNT_TBL);
	DB_servo_counter.servoEjection_openDB = EO;
	DB_servo_counter.servoEjection_closeDB = EC;
	db.write(1, DB_REC DB_servo_counter);
}


///////////////////////////////////
// COUNTER
//////////////////////////////////

void read_database_DB_counter () {
	db.open(SERVO_COUNT_TBL);
	db.read(1, DB_REC DB_servo_counter);
	pick_seed_position = DB_servo_counter.pick_seed_positionDB;
	detect_seed_position = DB_servo_counter.detect_seed_positionDB;
	margin_steps_to_detect_seed = DB_servo_counter.margin_steps_to_detect_seedDB;
	drop_position = DB_servo_counter.drop_positionDB;
}

void record_database_DB_counter (unsigned int PS,unsigned int DS,unsigned int MS,unsigned int DP) {
	db.open(SERVO_COUNT_TBL);
	DB_servo_counter.pick_seed_positionDB = PS;
	DB_servo_counter.detect_seed_positionDB = DS;
	DB_servo_counter.margin_steps_to_detect_seedDB = MS;
	DB_servo_counter.drop_positionDB = DP;
	db.write(1, DB_REC DB_servo_counter);
}


///////////////////////////////////
// NET CONFIG
//////////////////////////////////

void open_NET_DB () {
	db.open(NET_TBL);
	db.read(1, DB_REC DB_Net_config);			// Read configuration (1rst line)
	M_ID = DB_Net_config.machine_id;
	server[0] = DB_Net_config.UI_IP[0];
	server[1] = DB_Net_config.UI_IP[1];
	server[2] = DB_Net_config.UI_IP[2];
	server[3] = DB_Net_config.UI_IP[3];
	local_ip[0] = DB_Net_config.LOCAL_IP[0];
	local_ip[1] = DB_Net_config.LOCAL_IP[1];
	local_ip[2] = DB_Net_config.LOCAL_IP[2];
	local_ip[3] = DB_Net_config.LOCAL_IP[3];
	port = DB_Net_config.UI_port;
}

void NET_DB_REC () {
	db.open(NET_TBL);
	DB_Net_config.machine_id = M_ID;
	DB_Net_config.UI_IP [0] = server[0];
	DB_Net_config.UI_IP [1] = server[1];
	DB_Net_config.UI_IP [2] = server[2];
	DB_Net_config.UI_IP [3] = server[3];
	DB_Net_config.LOCAL_IP[0] = local_ip[0];
	DB_Net_config.LOCAL_IP[1] = local_ip[1];
	DB_Net_config.LOCAL_IP[2] = local_ip[2];
	DB_Net_config.LOCAL_IP[3] = local_ip[3];
	DB_Net_config.UI_port = port;
	db.write(1, DB_REC DB_Net_config);
}

void show_DBnetwork_data () {
	db.open(NET_TBL);
	db.read(1, DB_REC DB_Net_config);	
	Serial.println(F("\n*Network configuration:"));
	Serial.print(F("Machine ID: "));
	Serial.println(DB_Net_config.machine_id);
	Serial.print(F("Server IP: "));
	Serial.println(ip_to_str(DB_Net_config.UI_IP));
	Serial.print(F("Machine IP (local): "));
	Serial.println(ip_to_str(DB_Net_config.LOCAL_IP));
	Serial.print(F("Server Port: "));
	Serial.println(DB_Net_config.UI_port);
}




