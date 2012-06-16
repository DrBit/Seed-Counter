#include <SPI.h>
#include <Ethernet.h>


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {  0xDE, 0xAD, 0xBA, 0xEF, 0xFE, M_ID };
byte local_ip[] = { 10,250,1,199 };
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
	// start the Ethernet connection:
	Ethernet.begin(mac, local_ip);
	// give the Ethernet shield a second to initialize:
	delay(1000);

	Serial.print("Blister blaster id ");
	Serial.print(M_ID);
	Serial.print(" on IP: ");
	Serial.println(ip_to_str(local_ip));
	
	Serial.print ("Default User Interface server ip: ");
	Serial.println(ip_to_str(server));
	
	Serial.print ("Default User Interface server port: ");
	Serial.println(port);
	
	Serial.print("\r\nChange server and port? [y/n]\r\n");
	
	if (YN_question ()) {
		receive_server_IP ();
		receive_server_PORT ();
	}
	// Here we dont need to connect yet, just prepare everything
	// connected_to_server = connect_to_server ();
}


boolean check_server()
{
	int timeout =0;
	// Convert this time oput in a real timeout 
	while ((!connected_to_server) && (timeout < 60)) {
		connected_to_server = connect_to_server ();
		delay (50);
		timeout ++; 
	}
	
	if (connected_to_server) {
		// if there are incoming bytes available 
		// from the server, read them and process them:
		if (!receive_server_data ()) {
			// Nothing to do
		}else{
			// Something has been received
		}
		
	 
		// if the server's disconnected, stop the client:
		if (!client.connected()) {
			Serial.println("Server Disconnected.");
			client.stop();
			connected_to_server = false;
		}
	 
	}else{
		// We got a timeout connecting
		Serial.println("Timeout Connecting to the server...");
		return false;
	}
	return true;
}




boolean connect_to_server () {
	Serial.print("\nconnecting to: ");
	Serial.print(ip_to_str(server));
	Serial.print(":"); Serial.println (port);

	// if you get a connection, report back via serial:
	if (client.connect(server, port)) {
		Serial.println("connected!");
		return true;
	} else {
		// if you didn't get a connection to the server:
		Serial.println("connection failed");
	}
	return false;

}

void get_info_from_server (byte command) {
	sprintf(message, "%dI%d\r\n", M_ID, command);
	client.print(message);
	// we have to receive information
	if (!receive_server_data ()) {
		Serial.print ("OK not received or error on sended command I");
		Serial.println (command);
	}
}

void send_status_to_server (byte command) {
	previous_status = global_status;		// Stores previous status
	global_status = command;					// Updates actual status
    sprintf(message, "%dS%d\r\n", M_ID, command);
    client.print(message);

    if (!receive_server_data ()) {
		Serial.print ("OK not received or error on sended command S");
		Serial.println (command);
	}
}

void send_action_to_server(byte command) {		// Inform server that an action has been trigered
	//require an OK back from the server
    sprintf(message, "%dA%d\r\n", M_ID, command);
    client.print(message);

    if (!receive_server_data ()) {
		Serial.print ("OK not received or error on sended command A");
		Serial.println (command);
	}
}

void send_error_to_server (byte command) {		// Inform server that an error has ocurred
	//require an OK back from the server
    sprintf(message, "%dE%d\r\n", M_ID, command);
    client.print(message);

    if (!receive_server_data ()) {
		Serial.print ("OK not received or error on sended command E");
		Serial.println (command);
	}
}

void send_position_to_server (byte command) {	// Inform server that we are going to a position
	//require an OK back from the server
    sprintf(message, "%dG%d\r\n", M_ID, command);
    client.print(message);

    if (!receive_server_data ()) {
		Serial.print ("OK not received or error on sended command G");
		Serial.println (command);
	}
}	

void get_positions_from_server (byte command) {	// Receive position information stored in the server
	if (command == 0) {  // Ask for all positions
		sprintf(message, "%dP*\r\n", M_ID);
		client.print(message);
	}else{
		sprintf(message, "%dP%d\r\n", M_ID, command);
		client.print(message);
		// we have to receive one position
		// this means P + number equal the one we have asked for
		// puls 4 numbers that form the data of the position
		
		// so...
		if (!receive_server_data ()) {
			Serial.print ("OK not received or error on sended command P");
			Serial.println (command);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
boolean receive_server_data (){

	clean_buffer (received_msg,bufferSize);		// Prepare buffer
	
	// wait 3 seconds for incoming data before a time out
	int timeout =0;
	while ((client.available() == 0) && (timeout < 60)) {
		delay (50);
		timeout ++;
	}			
	
	boolean receivedO = false;					// Used to control the first letter of the OK sentence
	while (client.available() > 0) {
		char inChar = client.read();				// read
		
		switch (inChar) {
			case 'O': {
				receivedO = true;
			break; }
			
			case 'K': {
				return true;
			break; }
			
			case 'X': {
				// some data waiting for us receive it
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
				global_status = receiving_status;					// Updates actual status
				// Inform
				Serial.print("Received Status: ");
				Serial.println(receiving_status);
			break; }
			
			case 'I': {	// INFORMATION
				//#define get_seeds_mode 1				// 5 or 10 seeds per blister
				//#define get_default_idle_time 2		// Defaul idle time to go to sleep on user input 120 = 2 minutes.
				
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				int receiving_info = atoi(thisChar);
				
				switch (receiving_info) {	// What info are we going to receive? 
				
					case get_seeds_mode: {	// Should we define the seeds per blister? for now 2 modes 1 or 2 (10 or 5 seeds)
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						int receiving_seedmode = atoi(thisChar);
						
						if (receiving_seedmode == 1) {
							blister_mode = seeds10;
						}
						if (receiving_seedmode == 2) {
							blister_mode = seeds5;
						}
					break; }
					
					case get_default_idle_time: {
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned int receiving_idle_time = atoi(thisChar);
						default_idle_time = receiving_idle_time;
					break; }
					
					case get_default_off_time: {
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned int receiving_off_time = atoi(thisChar);
						default_off_time = receiving_off_time;
					break; }
				}
				// Inform
				Serial.print("Received Information: ");
				Serial.println(receiving_info);
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
				
				// Inform
				Serial.print("Received Position: ");
				Serial.print(receiving_position);
				// Compare with the real value.. do we have to update??
				if (Xc != mposition.Xc || Xf != mposition.Xf || Yc != mposition.Yc || Yf != mposition.Yf) {		// If data is different from the eeprom
					// store data in eeprom, data is different
					mposition.Xc = Xc;
					mposition.Xf = Xf;
					mposition.Yf = Yf;
					mposition.Yc = Yc;
					db.write(receiving_position, DB_REC mposition);
					Serial.println (" - Updated!");
				}else{
					Serial.println (" - OK");
				}
				
				// Finished. go back to the origin. If we receive another command we will sense it there.
			break; }
			
			default: {
				// Undefined command received
				Serial.print("Received undefined command: ");
				Serial.println(inChar);
				recevie_data_telnet (received_msg,bufferSize);
				char * thisChar = received_msg;
				// data
				Serial.print("With Data: ");
				Serial.println(thisChar);
			break; }
		}
	}
	return false;
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
		Serial.print("buffer full, max ");
		Serial.print(len-1,DEC);
		Serial.println(" characters per command.");
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

void receive_server_IP () {
	Serial.print ("Type server ip: ");
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
	Serial.print ("Type server port: ");
	Serial.flush ();
	const int buf_port = 6;
	char printerPort[buf_port];
	recevie_data (printerPort,buf_port);
	char * thisChar = printerPort;
	port = atoi(thisChar);
	Serial.println (port);
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


