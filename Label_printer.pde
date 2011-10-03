// Some predefined info

//SA
#define server_address	"office.pygmalion.nl"		// Host name of the public server to request a label 
//SS
#define server_script	"/labelgenerator/generate.php?batch_id="		// Parameters to generat label
//IP
#define printer_IP		"10.10.249.105"  	// Printer server IP (subjetc to change)
//PP
#define printer_port	"8000"				// Printer server port 		

#define password		""
unsigned int seeds_batch = 290;

/*  test
SA: drbit.nl
SS: /index.php
IP: 10.10.249.105:8000
*/


//////////////////////////
// LIST OF COMMANDS
//////////////////////////

// All commands are begined and ended with a carriage '/0'
// any received data failling to have '/0' at the begining anb at the end will be descarted.

/*



C00 - FALSE 
C01 - TRUE
C02 - ERROR
C03 - Configure network
C04 - Print label
C05 - Network ready to be configured (normally after reset)
C06 - Label printed correctly



*/

//////////////////////////
// LIST OF POSSIBLE ERRORS
//////////////////////////

/*

E00 - Failed to open connection. Network down or website not available
E01 - Time out receiving and aswer of the server 
E02 - We didnt get any tag equal of what we where expecting
E10 - Not expected command  // When we sended a command that receiver wasn't expecting
							// Normalli means receiver expects a concrete command and opnly will react to that
*/
	
#define endOfLine '*'

	
//////////////////////////
// Receive Command
//////////////////////////
#define number_of_commands 30
#define number_of_errors 30
char numberIndex = 0;
#define command_digits 2
char commandNumber[command_digits];
boolean lookForLetter = false;
boolean lookForNumber = false;
boolean incomingCommand = false;
boolean incomingError = false;
int commandNumberInt = -1;
int errorNumberInt = 0;
int last_command_received = 00;

int receiveNextValidCommand () {

	while (true) {
		while (Serial1.available()) {
			char c = Serial1.read();
			//Serial.print (c);	// JUST for debug
			
			if (c == endOfLine) { 		// begining or end of command
				//Serial.print ("-End of line detected-");
				// In this case we check if we had previous data in the buffer and process it if necessary
				// restart all and ready to receive a commmand
				if (lookForNumber) {
					lookForNumber = false;
					if (processCommand()) {	// we got a valid command!
						//Serial.print ("-Process command-");						
						if (incomingCommand) {
							reset_command ();
							return commandNumberInt;
						}else if (incomingError) {
							reset_command ();
							return errorNumberInt;
						}
						
					}else{
						// failed to process comand
					}
				}
				
				lookForLetter = true;
				numberIndex = 0;
			}
			
			if (lookForLetter && (c == 'C')) {
				//Serial.print ("-C detected-");
				// we got an incoming comand, start receive command number
				lookForNumber = true;
				incomingCommand = true;
				incomingError = false;
				lookForLetter = false;
			}else if (lookForNumber) {
				//Serial.print ("-Number-");
				// We look for the command number
				commandNumber[numberIndex] = c;
				if (numberIndex == command_digits) { 
					reset_command ();	// Command invalid too many characters
				}
				numberIndex++;
			}
			//delay (100);		// just give enough time to receive another character if 
		}
	}
}

int receiveNextValidError () {

	while (true) {
		while (Serial1.available()) {
			char c = Serial1.read();
			// Serial.print (c);	// JUST for debug
			
			if (c == endOfLine) { 		// begining or end of command
				//Serial.print ("-End of line detected-");
				// In this case we check if we had previous data in the buffer and process it if necessary
				// restart all and ready to receive a commmand
				if (lookForNumber) {
					lookForNumber = false;
					if (processCommand()) {	// we got a valid command!
						//Serial.print ("-Process command-");						
						if (incomingError) {
							reset_command ();
							return errorNumberInt;
						}
					}else{
						// failed to process comand
					}
				}
				
				lookForLetter = true;
				numberIndex = 0;
			}
			
			if (lookForLetter && (c == 'E')) {
				//Serial.print ("-E detected-");
				// we got an incoming error, start receive error number
				lookForNumber = true;
				incomingError = true;
				lookForLetter = false;
			}else if (lookForNumber) {
				//Serial.print ("-Number-");
				// We look for the command number
				commandNumber[numberIndex] = c;
				if (numberIndex == command_digits) { 
					reset_command ();	// Command invalid too many characters
				}
				numberIndex++;
			}
			//delay (100);		// just give enough time to receive another character if 
		}
	}
}




boolean processCommand () {
	//Serial.print ("-P-");
	if (incomingCommand) {
		// convert commandNumber
		//Serial.print ("-Incoming-");
		for (int i = (command_digits -1) ; i >= 0; i--) {
			commandNumberInt = atoi(&commandNumber[i]);		// Transform received string into integuer
			// Serial.println ("");
			// Serial.println (commandNumberInt);
		}
		// is valid?
		if ((commandNumberInt >= 0) && (commandNumberInt <= number_of_commands)) { 
			//Serial.print ("VALID");
			return true;
		}else {		
			//Serial.print ("NVALID");
			return false;
		}
	} else if (incomingError) {
	    for (int i = (command_digits -1) ; i >= 0; i--) {
			errorNumberInt = atoi(&commandNumber[i]);		// Transform received string into integuer
		}
		// is valid?
		if ((errorNumberInt >= 0) && (errorNumberInt <= number_of_errors)) { 
			return true;
		}else {		
			return false;
		}
	} else {
		// command not valid
		//Serial.println ("-not expectig command-");
		return false;
	}
}

void reset_command () {				// whenever data validation fails we reset all
	numberIndex = 0;
	lookForNumber = false;
	incomingCommand = false;
	incomingError = false;
	lookForLetter = false;
}


bool recevie_data () {

// receive
// print
// until C4 is received

}

//////////////////////////
// Send Command
//////////////////////////

void send_command (unsigned int command) {
	//delay (300);
	Serial1.print(endOfLine);	// Print begining command
	Serial1.print("C");
	// We need to send in form of two digits like (01)
	if (command < 10) {
		Serial1.print('0');
	}
	Serial1.print(command);
	Serial1.print(endOfLine);	// Print end command
	//delay(300);
}

void send_error (unsigned int command) {
	//delay (300);
	Serial1.print(endOfLine);	// Print begining command
	Serial1.print("E");
	// We need to send in form of two digits like (01)
	if (command < 10) {
		Serial1.print('0');
	}
	Serial1.print(command);
	Serial1.print(endOfLine);	// Print end command
	//delay(300);
}

void send_data () {


}

	
boolean print_label () {
	//Serial1.flush ();			// Clean buffer before sending command so we know we are receving
								// The right answer
	Serial.println (" Send print command... (C04)");						
	send_command (04);			// Print one label
	
	// Receive an OK
	last_command_received = receiveNextValidCommand();
	if ( last_command_received == 01) {				// Correct
		Serial.println ("\n Accepted!\n Starting printing 1 label ");
		return true;
	} else if (last_command_received == 02) {		// Not ready yet
		Serial.println ("\nCommand not accepted");
		return false;
	} else if (last_command_received == 03) {		// Error 
		// Check error
		//act on error
		Serial.println ("\nError received...");
		return false;
	} else { 
		Serial.println ("\nNOT A VALID COMMAND: ");
		Serial.print (last_command_received);
		return false;
	}
}


boolean printed_successfully () {

	Serial.println (" Checking if printed sccessfully... ");						

	
	last_command_received = receiveNextValidCommand();
	if ( last_command_received == 06) {				// Correct
		Serial.println ("\nLast printed label ended successfully");
		return true;
	} else if (last_command_received == 02) {		// Not ready yet
		Serial.println ("\nCommand not accepted");
		return false;
	} else if (last_command_received == 00) {		// Error 
		// Check error
		//act on error
		Serial.print ("\nError received: ");
		receiveNextValidError ();
		return false;
	} else { 
		Serial.println ("\nNOT A VALID COMMAND");
		return false;
	}
}

//////////////////////////
// Other Functions
//////////////////////////

void init_printer () {
	
	// Reset arduino (TODO connect reset cable)
	EthernetModuleReset ();
	init_serial1 ();
	
	// Repeat until we receive the right start command from the network module
	boolean cReceived = false;
	while (!cReceived) {
		// Wait for a ready command
		last_command_received = receiveNextValidCommand();
		if ( last_command_received == 05) {				// Correct
			Serial.println (" Network ready to operate");
			cReceived = true;
		} else {
			Serial.println (" Command unexpected");
			Serial.println (last_command_received);
		}
	}
	
	// Send a configure command
	if (false) { // we just skip this part for now
		send_command (03);		// start cpnfiguration process
		// Receive an OK
		last_command_received = receiveNextValidCommand();
		if ( last_command_received == 01) {				// Correct
			Serial.println (" Starting network configuring process ");
		} else if (last_command_received == 02) {		// Not ready yet
			Serial.println (" Network module not ready for starting configuration process...");
		} else if (last_command_received == 03) {		// Error 
			// Check error
			//act on error
			Serial.println (" Error starting nertwork configuration process...");
		}
	}
	
	Serial.print (" Self configuring....");
	// delay (10000); // let configure first....
	Serial.println (" Done!");

	
	
	// Start configuration
	
	// We configure the arduino with the right directions
	
	
	/*
	delay (300);
	Serial1.println("SA");				// Tell arduino next data is the server_address
	delay(300);
	Serial1.println(server_address);	// Send server_address
	delay(300);
	Serial1.println("SS");				// Tell arduino next data is the server_scrip
	delay(300);
	Serial1.print(server_script);		// Send server_script
	Serial1.println(seeds_batch);		// Add batch ID in the parameters
	delay(300);
	Serial1.println("IP");				// Tell arduino next data is the printer_IP
	delay(300);
	Serial1.println(printer_IP);		// Send printer_IP
	delay(300);
	Serial1.println("PS");				// Tell arduino next data is the password
	delay(300);
	Serial1.println(password);			// Send password
	delay(300);
	Serial1.println("PP");				// Tell arduino next data is the printer_port
	delay(300);
	Serial1.println(printer_port);		// Send printer port
	delay(300);

	
	Serial.print ("Received IP: ");
	// print IP...
	while (!Serial1.available()) {}
	while (Serial1.available()) {
		char c = Serial1.read();
		Serial.print (c);
		delay (100);		// just give enough time to receive another character if 
	}
	
	// Confirmation of data:
	while (!Serial1.available()) {}
	while (Serial1.available()) {
		char c = Serial1.read();
		if (c == '1') {
			Serial.println (" Configuring network ");
		}else if (c == '0') {
			Serial.println (" Configuring network ");
		}
		delay (100);		// just give enough time to receive another character if 
	}
	
	
	*/
	
}

void EthernetModuleReset () {
	Serial.println (" Reset network module");
}