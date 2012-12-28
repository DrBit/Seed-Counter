#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0xDE, 0xAD, 0xBA, 0xEF, 0xFE, M_ID };
byte local_ip[] = { 10,250,1,199 + M_ID };
byte server[] = { 10,250,1,3 }; 
int port = 8888;


#define bufferSize 18
static char message[bufferSize];		// Variable to send messages to the server
char received_msg[bufferSize];

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;




void setup_network() {
	
	init_NET_DB ();				// Open DB of network config
	M_ID = config.machine_id;
	server[0] = config.UI_IP[0];
	server[1] = config.UI_IP[1];
	server[2] = config.UI_IP[2];
	server[3] = config.UI_IP[3];
	local_ip[0] = config.LOCAL_IP[0];
	local_ip[1] = config.LOCAL_IP[1];
	local_ip[2] = config.LOCAL_IP[2];
	local_ip[3] = config.LOCAL_IP[3];
	port = config.UI_port;
	
	// start the Ethernet connection:
	Ethernet.begin(mac, local_ip);
	// give the Ethernet shield a second to initialize:
	delay(100);

	Serial.print(F("Blister blaster id "));
	Serial.print(M_ID);
	Serial.print(F(" on IP: "));
	Serial.println(ip_to_str(local_ip));
	
	Serial.print (F("Default User Interface server ip: "));
	Serial.println(ip_to_str(server));
	
	Serial.print (F("Default User Interface server port: "));
	Serial.println(port);
	
	Serial.print(F("\r\nChange server, port and ID? [y/n]\r\n"));
	
	if (YN_question (3)) {
		receive_local_IP ();
		receive_server_IP ();
		receive_server_PORT ();
		receive_M_ID ();
		
		Serial.print(F("\r\nSave server, port and ID to internal EEPROM memory? [y/n]\r\n"));
		
		if (YN_question (20)) {
			config.machine_id = M_ID;
			config.UI_IP [0] = server[0];
			config.UI_IP [1] = server[1];
			config.UI_IP [2] = server[2];
			config.UI_IP [3] = server[3];
			config.LOCAL_IP[0] = local_ip[0];
			config.LOCAL_IP[1] = local_ip[1];
			config.LOCAL_IP[2] = local_ip[2];
			config.LOCAL_IP[3] = local_ip[3];
			config.UI_port = port;
			NET_DB_REC ();
		}
	}	
	// Here we don't need to connect yet, just prepare everything
	// connected_to_server = connect_to_server ();
}


boolean check_server()
{
	int timeout =0;
	// Convert this time out in a real timeout (with millis)
	// In case we discconected we no reconnect
	while ((!connected_to_server) && (timeout < 10)) {
		connected_to_server = connect_to_server ();
		#if not defined bypass_server
		delay (2000);
		#endif
		timeout ++; 
	}
	
	// If we are already connected to the server...
	if (connected_to_server) {
		// if there are incoming bytes available 
		// from the server, read them and process them:
		sprintf(message, "%dX\r\n", M_ID);
		client.print(message);
		#if defined Server_com_debug
			Serial.print("Send:");
			sprintf(message, "%dX - ", M_ID);
			Serial.print(message);
		#endif

		if (!receive_server_data ()) {
			// Nothing to do
			// We got a time out so no response
		}else{
			// Something has been received
		}
		
		boolean skip_server = false;
		#if defined bypass_server
			skip_server = true;
		#endif
	 
		// if the server's disconnected, stop the client:
		if (!client.connected() && !skip_server) {
		// if (!client.connected()) {
			Serial.println(F("Server Disconnected."));
			client.stop();
			connected_to_server = false;
		}
	 
	}else{
		// We got a timeout connecting so we didn't succed 
		Serial.println(F("Timeout Connecting to the server..."));
		Serial.println(F("Contact the network administrator or press a key to try again."));
		press_button_to_continue (0);		// Press any key to continue
		return false;
	}
	return true;
}


// Basic routine to keep trying until gets connected
void server_connect () {
	boolean _connected=false;
	while (!_connected) {
		connected_to_server = connect_to_server ();
		if (connected_to_server) {
			_connected = true;
		}
	} 
}
///////////////  add this -->> connected_to_server = connect_to_server ();



boolean connect_to_server () {
	Serial.print(F("\nconnecting to: "));
	Serial.print(ip_to_str(server));
	Serial.print(F(":")); Serial.println (port);

boolean skip_server = false;
#if defined bypass_server
skip_server = true;
#endif
	// if you get a connection, report back via serial:
	if (client.connect(server, port) || skip_server) {
		Serial.println(F("connected!"));
		return true;
	} else {
		// if you didn't get a connection to the server:
		Serial.println(F("connection failed"));
		delay (5000);

	}
	return false;

}

void get_info_from_server (byte command) {
	sprintf(message, "%dI%d\r\n", M_ID, command);
	client.print(message);
	#if defined Server_com_debug
		Serial.print(F("Get Info:"));
		sprintf(message, "%dI%d - ", M_ID, command);
		Serial.print(message);
	#endif
	// we have to receive information
	if (!receive_server_data ()) {
		//Serial.print (F("-OK not received or error on sent command I");
		//Serial.println (command);
	}
}

void send_status_to_server (byte command) {
	previous_status = global_status;		// Stores previous status
	global_status = command;					// Updates actual status
	sprintf(message, "%dS%d\r\n", M_ID, command);
	client.print(message);
	#if defined Server_com_debug
		Serial.print(F("Send Status:"));
		sprintf(message, "%dS%d - ", M_ID, command);
		Serial.print(message);
	#endif

	if (!receive_server_data ()) {
		//Serial.print (F("-OK not received or error on sended command S"));
		//Serial.println (command);
	}
}

void send_action_to_server(byte command) {		// Inform server that an action has been trigered
	//require an OK back from the server
	sprintf(message, "%dA%d\r\n", M_ID, command);
	client.print(message);
	#if defined Server_com_debug
		Serial.print(F("Send Action:"));
		sprintf(message, "%dA%d - ", M_ID, command);
		Serial.print(message);
	#endif

	if (!receive_server_data ()) {
		#if defined Server_com_error_debug
		Serial.print (F("-OK not received or error on sended command A"));
		Serial.println (command);
		#endif
	}
}

void send_error_to_server (byte command) {		// Inform server that an error has ocurred
	//require an OK back from the server
	sprintf(message, "%dE%d\r\n", M_ID, command);
	client.print(message);
	#if defined Server_com_debug
		Serial.print(F("Send Error:"));
		sprintf(message, "%dE%d - ", M_ID, command);
		Serial.print(message);
	#endif

	if (!receive_server_data ()) {
		#if defined Server_com_error_debug
		Serial.print (F("-OK not received or error on sended command E"));
		Serial.println (command);
		#endif
	}
}

void send_position_to_server (byte command) {	// Inform server that we are going to a position
	//require an OK back from the server
	sprintf(message, "%dG%d\r\n", M_ID, command);
	client.print(message);
	#if defined Server_com_debug
		Serial.print(F("Send actual position:"));
		Serial.print(message);
		sprintf(message, "%dG%d - ", M_ID, command);
	#endif

	if (!receive_server_data ()) {
		#if defined Server_com_error_debug
		//Serial.print (F("-OK not received or error on sended command G"));
		Serial.println (command);
		#endif
	}
}	

void get_positions_from_server (byte command) {	// Receive position information stored in the server
	if (command == 0) {  // Ask for all positions
		sprintf(message, "%dP*\r\n", M_ID);
		client.print(message);
		#if defined Server_com_debug
			Serial.print(F("Get ALL positions from server:"));
			Serial.print(message);
		#endif
		if (!receive_server_data ()) {
			#if defined Server_com_error_debug
			//Serial.print (F("-OK not received or error on sended command P"));
			Serial.println (command);
			#endif
		}
	}else{
		sprintf(message, "%dP%d\r\n", M_ID, command);
		client.print(message);
		#if defined Server_com_debug
			Serial.print(F("Get position from server:"));
			sprintf(message, "%dP%d - ", M_ID, command);
			Serial.print(message);
		#endif
		// we have to receive one position
		// this means P + number equal the one we have asked for
		// puls 4 numbers that form the data of the position
		
		// so...
		if (!receive_server_data ()) {
			#if defined Server_com_error_debug
			Serial.print (F("-OK not received or error on sended command P"));
			Serial.println (command);
			#endif	
		}
	}
}


void get_config_from_server (byte command) {	// Receive configuration information stored in the server
	if (command == 0) {  // Ask for all configuration
		sprintf(message, "%dC*\r\n", M_ID);
		client.print(message);
		#if defined Server_com_debug
			Serial.print(F("Get ALL config from server:"));
			Serial.print(message);
		#endif
		if (!receive_server_data ()) {
			#if defined Server_com_error_debug
			//Serial.print (F("-OK not received or error on sended command C"));
			//Serial.println (command);
			#endif
		}
	}else{
		sprintf(message, "%dC%d\r\n", M_ID, command);
		client.print(message);
		#if defined Server_com_debug
			Serial.print(F("Get config from server:"));
			sprintf(message, "%dC%d - ", M_ID, command);
			Serial.print(message);
		#endif
		// we have to receive one position
		// this means P + number equal the one we have asked for
		// puls 4 numbers that form the data of the position
		
		// so...
		if (!receive_server_data ()) {
			#if defined Server_com_error_debug
			//Serial.print (F("-OK not received or error on sended command C"));
			//Serial.println (command);
			#endif
		}
	}
}
 

//////////////////////////////////////////////////////////////////////////////////////////
boolean receive_server_data (){

	clean_buffer (received_msg,bufferSize);		// Prepare buffer
	
	// wait 3 seconds for incoming data before a time out
	// 100ms * 10 = 1s so... 100*20 = 2000 (time to wait 2 seconds)
	int times_to_try = 20;
	#if defined bypass_server
	times_to_try = 1;
	#endif
	int timeout = 0;
	while (!(client.available() > 0) && (timeout < times_to_try)) {
		delay (100);
		timeout ++;
	}			
	
	boolean receivedO = false;					// Used to control the first letter of the OK sentence
	boolean receivedK = false;
	while (client.available() > 0) {
		char inChar = client.read();				// read
		
		switch (inChar) {
			case 'O': {
				#if defined Server_com_debug
				Serial.print (inChar);
				#endif
				receivedO = true;
			break; }
			
			case 'K': {
				#if defined Server_com_debug
				Serial.println (inChar);
				#endif
				receivedK = true;
			break; }
						
			case 'E': {
				Serial.print (F("Error received: "));
				Serial.print (inChar);
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				Serial.print(thisChar);
			break; }
			
			case 'X': {
				// some data waiting for us receive it
			break; }
			
			case 'A': {
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_info = atoi(thisChar);
				
				switch (receiving_info) {	// What info are we going to receive? 
					case enable_pump: {	// Software enable pump
						pump_enable ();
					break; }
					case disable_pump: {	// Software enable pump
						pump_disable ();
					break; }
					////////////////////////////////////////
					// answers for errors
					case button_continue: {
						server_answer = button_continue;
					break;}
					case button_ignore: {
						server_answer = button_ignore;
					break;}
				}
			break; }

			case 13: {
				// omit return
			break; }
			
			case 10: {
				// omit feed line
			break; }
			
			case 'S': {	// STATUS
				previous_status = global_status;					// Stores previous status
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_status = atoi(thisChar);
				global_status = receiving_status;

									// Updates actual status
				#if defined Server_com_debug
				// Inform
				Serial.print(F("Received Status: "));
				Serial.print(receiving_status);
				Serial.print(F(" - "));
				#endif
				if (global_status == S_finishing_batch)
				// We have to finish the batch and get into stop
				send_status_to_server (S_finishing_batch);
			break; }
			
			case 'I': {	// INFORMATION
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_info = atoi(thisChar);
				
				switch (receiving_info) {	// What info are we going to receive? 
				
					case get_seeds_mode: {	// Should we define the seeds per blister? for now 2 modes 1 or 2 (10 or 5 seeds)
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						int receiving_seedmode = atoi(thisChar);
						
						if (receiving_seedmode == seeds10) {
							blister_mode = seeds10;
						}
						if (receiving_seedmode == seeds5) {
							blister_mode = seeds5;
						}
						#if defined Server_com_debug
							Serial.print(F("Received Information Blister mode: "));
							Serial.print(blister_mode);
							Serial.print(F(" - "));
						#endif
					break; }
				}
			break; }
						
			case 'C': {	// CONFIGURATION
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_config = atoi(thisChar);

				switch (receiving_config) {	// What info are we going to receive? 

					case Cget_default_idle_time: {
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned int receiving_idle_time = atoi(thisChar);
						default_idle_time = receiving_idle_time;
					break; }

					case Cget_default_off_time: {
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned int receiving_off_time = atoi(thisChar);
						default_off_time = receiving_off_time;
					break; }
				}
				#if defined Server_com_debug
				Serial.print(F("Received Configuration: "));
				Serial.print(receiving_config);
				switch (receiving_config) {	// What info are we going to receive? 
					case Cget_default_idle_time: {
						Serial.print(F(" - default_idle_time: "));
						Serial.println(default_idle_time);
					break; }
					case Cget_default_off_time: {
						Serial.print(F(" - default_off_time: "));
						Serial.println (default_off_time);
					break; }
				}
				#endif
			break; }
			
			case 'P': {	// POSITION
			
				// what we will do now is check what addres are we receiving and compare it with the one on the memory
				// if the data is different we will update the memory wth the newly received data.
				
				// receive memory position to check  /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_position = atoi(thisChar);
				// Load database position
				db.read(receiving_position, DB_REC mposition);	
				
				// receive Xc /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Xc = atoi(thisChar);
				// receive Xf /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Xf = atoi(thisChar);
				// receive Yc /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Yc = atoi(thisChar);
				// receive Yf /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Yf = atoi(thisChar);
				
				#if defined Server_com_debug
				// Inform
				Serial.print(F("Received Position: "));
				Serial.print(receiving_position);
				#endif
				// Compare with the real value.. do we have to update??
				if (Xc != mposition.Xc || Xf != mposition.Xf || Yc != mposition.Yc || Yf != mposition.Yf) {		// If data is different from the eeprom
					// store data in eeprom, data is different
					mposition.Xc = Xc;
					mposition.Xf = Xf;
					mposition.Yf = Yf;
					mposition.Yc = Yc;
					db.write(receiving_position, DB_REC mposition);
					#if defined Server_com_debug
					Serial.println (F(" - Updated!"));
					#endif
				}else{
					#if defined Server_com_debug
					Serial.println (F(" - Correct!"));
					#endif
				}
				
				// Finished. go back to the origin. If we receive another command we will sense it there.
			break; }

			case 'G': {	// GO, command sended from the server that say to the machine where to go (exact position)	
				
				// We will receive data and go where the server tell us to go

				// receive Xc /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				unsigned int Xc = atoi(thisChar);
				// receive Xf /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Xf = atoi(thisChar);
				// receive Yc /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Yc = atoi(thisChar);
				// receive Yf /////////////////////////////////////////
				recevie_data_telnet (received_msg,bufferSize);
				thisChar = received_msg;
				unsigned int Yf = atoi(thisChar);
				
				#if defined Server_com_debug
				// Inform
					Serial.print(F("Received GoTo: "));
					Serial.print (Xc);
					Serial.print(F(","));
					Serial.print (Xf);
					Serial.print(F(","));
					Serial.print (Yc);
					Serial.print(F(","));
					Serial.print (Yf);
				#endif

				if (global_status == S_test) {
					// Go to the defined position
					go_to_posXY (Xc,Xf,Yc,Yf);
					Serial.print(F(" ... Done!"));
				}else{
					// we are not in TEST mode so we shouldn move
					Serial.println(F(" ... Error not in TEST mode, WRONG STATUS** NOT MOVING"));
				}

			break; }
			
			default: {
				// If command is not a letter means we ara handling raw data
				// just print it
				//if (inChar != is_a_letter) {
					// Serial.print(inChar);

				// }
				// Undefined command received
				Serial.print(F("Received undefined command: "));
				Serial.println(inChar);
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				// data
				Serial.print(F("With Data: "));
				Serial.println(thisChar);
			break; }
		}
	}
	#if defined Server_com_debug
		#if defined bypass_server
		timeout = 5;
		#endif
		if (timeout >= 5) Serial.println(F("*timeout answer*"));
	#endif
	if (receivedK) {
		return true;
	}else{
		return false;
	}
	// DONE!
}


////////////////////////////
// Functions to pharse text
////////////////////////////
void buffer_char (char character, char* bufferContainer, int max_size) {
	int len = max_size;
	int actualLen = strlen(bufferContainer);
	char temp_char = character;
	
	if (actualLen < len-1) {
		bufferContainer[actualLen] = temp_char;
	}else{
		Serial.print(F("buffer full, max "));
		Serial.print(len-1,DEC);
		Serial.println(F(" characters per command."));
	}
}

void clean_buffer (char* bufferContainer, int max_size) {
	int len = max_size;
	for (int c = 0; c < len; c++) {
		bufferContainer[c] = 0;
	}
}
 
////////////////
// EXTRAS
////////////////

 // Just a utility function to nicely format an IP address.
const char* ip_to_str(const uint8_t* ipAddr)
{
	static char buf[16];
	sprintf(buf, "%d.%d.%d.%d\0", ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
	return buf;
}

void receive_local_IP () {
	Serial.print (F("Type local ip: "));
	Serial.flush ();
	int buf_ip =17; // 17 is the maximum numbers an IP can contain (including dots) 
	char localIP[buf_ip]; 
	recevie_data (localIP,buf_ip);
	// Serial.println (printerIP);
	// Staring of script
	String SprinterIP = localIP;
	// convert into -> byte printer_ipAddr[4]
	// ip 10.11.12.13
	int firstDot = SprinterIP.indexOf('.');
	int secondDot = SprinterIP.indexOf('.', firstDot + 1 );
	int thirdDot = SprinterIP.indexOf('.', secondDot + 1 );
	int lastChar = SprinterIP.length();


	int num = 0; 
	// when you cast the individual chars to ints you will get their ascii table equivalents 
	// Since the ascii values of the digits 1-9 are off by 48 (0 is 48, 9 is 57), 
	// you can correct by subtracting 48 when you cast your chars to ints.
	for (int i = (firstDot-1); i>=0 ; i--) {
		num = atoi(&localIP[i]);
	}
	//Serial.println (num);
	local_ip[0] = (byte) num;
	num = 0;
	for (int i = (secondDot-1); i>=(firstDot+1) ; i--) {
		num = atoi(&localIP[i]);
	}
	//Serial.println (num);
	local_ip[1] = (byte) num;
	num = 0;
	for (int i = (thirdDot-1); i>=(secondDot+1) ; i--) {
		num = atoi(&localIP[i]);
	}
	//Serial.println (num);
	local_ip[2] = (byte) num;
	num = 0;
	for (int i = (lastChar-1); i>=(thirdDot+1) ; i--) {
		num = atoi(&localIP[i]);
	}
	//Serial.println (num);
	local_ip[3] = (byte) num;


	Serial.println (ip_to_str(local_ip));
}


void receive_server_IP () {
	Serial.print (F("Type server ip: "));
	Serial.flush ();
	int buf_ip =17; // 17 is the maximum numbers an IP can contain (including dots) 
	char printerIP[buf_ip]; 
	recevie_data (printerIP,buf_ip);
	// Serial.println (printerIP);
	// Staring of script
	String SprinterIP = printerIP;
	// convert into -> byte printer_ipAddr[4]
	// ip 10.11.12.13
	int firstDot = SprinterIP.indexOf('.');
	int secondDot = SprinterIP.indexOf('.', firstDot + 1 );
	int thirdDot = SprinterIP.indexOf('.', secondDot + 1 );
	int lastChar = SprinterIP.length();


	int num = 0; 
	// when you cast the individual chars to ints you will get their ascii table equivalents 
	// Since the ascii values of the digits 1-9 are off by 48 (0 is 48, 9 is 57), 
	// you can correct by subtracting 48 when you cast your chars to ints.
	for (int i = (firstDot-1); i>=0 ; i--) {
		num = atoi(&printerIP[i]);
	}
	//Serial.println (num);
	server[0] = (byte) num;
	num = 0;
	for (int i = (secondDot-1); i>=(firstDot+1) ; i--) {
		num = atoi(&printerIP[i]);
	}
	//Serial.println (num);
	server[1] = (byte) num;
	num = 0;
	for (int i = (thirdDot-1); i>=(secondDot+1) ; i--) {
		num = atoi(&printerIP[i]);
	}
	//Serial.println (num);
	server[2] = (byte) num;
	num = 0;
	for (int i = (lastChar-1); i>=(thirdDot+1) ; i--) {
		num = atoi(&printerIP[i]);
	}
	//Serial.println (num);
	server[3] = (byte) num;


	Serial.println (ip_to_str(server));
}

void receive_server_PORT () {
	Serial.print (F("Type server port: "));
	Serial.flush ();
	const int buf_port = 6;
	char printerPort[buf_port];
	recevie_data (printerPort,buf_port);
	char * thisChar = printerPort;
	port = atoi(thisChar);
	Serial.println (port);
}

void receive_M_ID () {
	Serial.print (F("Type machine ID: "));
	Serial.flush ();
	const int buf_id = 3;
	char tempID[buf_id];
	recevie_data (tempID,buf_id);
	char * thisChar = tempID;
	M_ID = atoi(thisChar);
	Serial.println (M_ID);
}


boolean recevie_data (char* parameter_container,int buffer) {
	// first clean data
	int len = buffer;
	for (int c = 0; c < len; c++) {
		parameter_container[c] = 0;
	}


	while (true) {
		while (Serial.available()) {
			char c = (char) Serial.read();
			//Serial.print (Serial.read()); // JUST for debug
			if ((c == 13) || (c == 10)) { 	// begining or end of command
				//end
				if (strlen(parameter_container) > 0) {	// We have to receive something first
					parameter_container[strlen(parameter_container)] = '\0';
					return true;
				}
			}else{
					if (strlen(parameter_container) == buffer ) {
					// Serial.println (" Reached the data max lengh, we reset the tag" );
					// Error!! buffer overload
					return false;
				}else{
					// DATA comes here
					// Serial.print (c);
					parameter_container[strlen(parameter_container)]=c;
					// DEBUG IP
				}
			}
		}
	}
}

boolean recevie_data_telnet (char* parameter_container,int buffer) {
	// first clean data
	int len = buffer;
	for (int c = 0; c < len; c++) {
		parameter_container[c] = 0;
	}


	while (true) {
		while (client.available()) {
			char c = (char) client.read();
			//Serial.print (Serial.read()); // JUST for debug
			if ((c == 13) || (c == 10)) { 	// begining or end of command
				//end
				if (strlen(parameter_container) > 0) {	// We have to receive something first
					parameter_container[strlen(parameter_container)] = '\0';
					return true;
				}
			}else{
					if (strlen(parameter_container) == buffer ) {
					// Serial.println (" Reached the data max lengh, we reset the tag" );
					// Error!! buffer overload
					return false;
				}else{
					// DATA comes here
					// Serial.print (c);
					parameter_container[strlen(parameter_container)]=c;
					// DEBUG IP
				}
			}
		}
	}
}