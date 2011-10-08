// Some predefined info

//SA
#define server_address	"office.pygmalion.nl"		// Host name of the public server to request a label 
//SS
#define server_script	"/labelgenerator/generate.php?batch_id="		// Parameters to generat label
//IP
#define printer_IP		"10.10.249.105"  	// Printer server IP (subjetc to change)
//PP
#define printer_port	"8000"				// Printer server port 		

#define password		"YXJkdWlubzpQQXBhWXViQTMzd3I="
unsigned int seeds_batch = 290;

/*  test
SA: drbit.nl
SS: /index.php
IP: 10.10.249.105:8000
*/

/*
//////////////////////////
// LIST OF COMMANDS
//////////////////////////

// All commands are begined and ended with a carriage '/0'
// any received data failling to have '/0' at the begining anb at the end will be descarted.
-------------
C00 - FALSE 
C01 - TRUE
C02 - ERROR
-------------
C03 - Update network configuration
C04 - Print label
C05 - Network module ready for operation (normally after reset)
C06 - Label printed correctly
------------
C07 - Send SA (server_address)
C08 - Send SS (server_script)
C09 - Send SB (seeds_batch)
C10 - Send IP (printer_IP)
C11 - Send PS (password)
C12 - Send PP (printer_port)
------------
C13 - Begining of data stream
C14 - End of data stream
------------
//////////////////////////
// LIST OF POSSIBLE ERRORS
//////////////////////////

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
			Serial.print (c);	// JUST for debug
			
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

void send_data (char* data_to_send) {


}

void send_data (unsigned int data_to_send) {


}


int receive_next_answer (int default_answer) {

	// default command is always  01
	last_command_received = receiveNextValidCommand();
	
	if (last_command_received == default_answer) {
		//ok
		return default_answer;
	}else{
		switch (last_command_received) { 
			case 00:
				print_fail();
				Serial.println (" * (C00) Failed execution ");
				// Serial.print (last_command_received);
				return 00;
			break;
			
			case 01:
				//Serial.println ("TRUE (C01)");
				return 01;
			break;
			
			case 02:
				print_fail();
				Serial.println (" * (C02) Error");
				receiveNextValidError ();
				return 02;
			break;
			
			default:
				print_fail();
				Serial.println (" * NOT A VALID RESPONSE: ");
				// Serial.print (last_command_received);
				return 00;
			break;
		}
	}
}
	

//////////////////////////
// Other Functions
//////////////////////////

void init_printer () {
	
	// Reset arduino (TODO connect reset cable)
	EthernetModuleReset ();
	Serial1.begin (9600);
	Serial.print   ("Init Ethernet module: ");
	
	// Repeat until we receive the right start command from the network module
	boolean cReceived = false;
	while (!cReceived) {
		if (receive_next_answer(05) == 05) {
			print_ok();
			cReceived = true;
		}
	}
	
	
	select_batch_number ();
	
	update_network_configuration ();
	
	// Send a configure command
	/*send_command (03);		// start configuration process
	// Receive an OK
	if (receive_next_answer(01) == 01) { 	// Command accepted
		// All correct , continue
	}else{
		print_fail();
		Serial.println (" * Command (C03) Failed");
		Serial.println(" * Press button 1 to continue");
		press_button_to_continue (1);
	}

	
	boolean start_config = false;
	while (!start_config) {
		send_command (03);		// start configuration process
		// Receive an OK
		if (receive_next_answer(01) == 01) { 	// Command accepted
			// All correct , continue
			start_config = true;
		}else{
			print_fail();
			Serial.println (" * Command (C03) Failed");
			Serial.println(" * Press button 1 to try again");
			press_button_to_continue (1);
		}
	} */
}


void EthernetModuleReset () {
	Serial.println (" * Press reset button of the network module to continue");
}


void prepare_printer() {
	// Print 2 stickers at the begining
	Serial.println ("Label printer should have 2 labels printed before packaging can start");
	Serial.println (" * Press 1 when ready to start");
	Serial.println (" * Press 2 to print one label");
	
	boolean ready = false;
	
	while (!ready) {
		switch (return_pressed_button ()) {
			case 1	:
				// do nothing so we wond detect any error and we will continue
				ready = true;
			break;
			//Print process
			case 2:
				print_one_label ();
			break;
		}
	}
	
}

// Types in a batch nomber for update it
int select_batch_number () {
	boolean inNumber = true;
	while (inNumber) {
		Serial.print (" Type in batch number: ");
		seeds_batch = get_number(3);
		Serial.println (seeds_batch);
		inNumber = false;
		
		Serial.print (" Correct? Y/N ");
		if (YN_question()) {
			// do nothing and quit
		}else{
			inNumber = true;	// Ask number again
		}
	}
}


// TODO
void update_network_configuration () {

/* 
C07 - Send SA (server_address)
C08 - Send SS (server_script)
C09 - Send SB (seeds_batch)
C10 - Send IP (printer_IP)
C11 - Send PS (password)
C12 - Send PP (printer_port)
*/
	Serial.print   ("Update network module: ");
	
	// Here the network module needs to be listening to the command C03 update network configuration
	
	send_command (7);
	send_data (server_address);
	delay (400);	
	send_command (8);
	send_data (server_script);
	delay (400);	
	send_command (9);
	send_data (seeds_batch);
	delay (400);	
	send_command (10);
	send_data (printer_IP);
	delay (400);	
	send_command (11);
	send_data (password);
	delay (400);	
	send_command (12);
	send_data (printer_port);

	
	if (receive_next_answer(01) == 01) { 	// Command accepted
		// All correct , continue
		print_ok();
	}else{
		print_fail();
		Serial.println (" * Command (C03) Failed");
		Serial.println(" * Press button 1 to try again");
		press_button_to_continue (1);
	}
}


void print_one_label () {

	Serial.print ("Generate label (C04): ");						
	send_command (04);			// Print one label
	
	if (receive_next_answer(01) == 01) { 	// Command accepted
		// Wait for answer (command 06 indicates sucsesful printing
		if (receive_next_answer(06) == 06) { 
			print_ok();				// All went OK
		}else{
			print_fail();
			Serial.println (" * Expected response (C06)");
			Serial.println(" * Press button 1 to continue");
			press_button_to_continue (1);
		}
	}else{
		print_fail();
		Serial.println (" * Command (C04) Failed");
		Serial.println(" * Press button 1 to continue");
		press_button_to_continue (1);
	}
}

