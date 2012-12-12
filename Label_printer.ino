void print_one_label () {
	// Print one label 
	send_action_to_server(ask_for_label);
}

void print_and_release_label () {
	print_one_label ();
	// Wait for the printer to print a label
	boolean released = check_label_realeased (true);
	while (!released) {
		Serial.println("Label error, remove any label that might be left and press number 1 to try again or 2 to continue.");
		int button_pressed = return_pressed_button ();
		if (button_pressed == 2) break;
		
		Serial.println("Goto print position");
		go_to_memory_position (3);			// Print position
		print_one_label ();
		released = check_label_realeased (true);
	}
}

boolean check_label_realeased (boolean print) {
	boolean skip_print_sens = false;
#if defined Sensor_printer
	skip_print_sens = true;
#endif
	boolean label = false;
	boolean timeout_label = false;
	int count = 0;
	if (print) Serial.print("Label released: ");
	
	// Check if we got a label, or we timeout
	while (!label && !timeout_label) {
		label = digitalRead (SensLabel); 
		count ++;
		if (count == 200) timeout_label = true;
		delay (80);
		if (skip_print_sens) count == 199;
	}
	
	
	if (label || skip_print_sens) {
		if (print) print_ok();
		send_action_to_server(label_ok);
		delay (500);		// Just give sometime to the printer to finsh the job before we move
		Serial.println("Go to brush position");
		go_to_memory_position (20);
		label = digitalRead (SensLabel);		// After moving we check the label again, could be that wasn't completely stiked and moved on the way.
		if (!label && !skip_print_sens) return false;
		return true;
	}

	if (print) print_fail ();
	send_error_to_server (label_timeout);
	return false;
}


//unsigned int seeds_batch = 290;

//#define number_of_commands 20
//#define number_of_errors 20

/*
//////////////////////////
// LIST OF INTERNAL ACTIONS/COMMANDS
//////////////////////////
// All commands are begined and ended with a '*'
// any received data failling to have '*' at the begining anb at the end will be descarted.
-------------
C00 - FALSE 
C01 - TRUE
C02 - ERROR
-------------
C03 - Fetch All positions 
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
EXTRA DATA
------------
// Extra data will be used to transfer further data. Like in order go to position
// in go to position extradata will indicate the position itself
C13 - Begining of data stream
C14 - End of data stream
------------
C15 - Ask for data of network configuration
------------
C16 - Send US (ui_server)
C17 - Send MI (Machine ID)
*/


/*


#define endOfLine '*'
	

//////////////////////////
// Main Functions
//////////////////////////

void init_ethernet () {
	// Reset arduino
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
	
	// The module is now ready
	
	// 	It is waiting for a *C01* command confirmation to continue
}

void configure_network () {

	// First we tell the module to get ready to configure itself (after a hard reset)
	send_command (1);		// Send confirmation

	// Right now the module will configure itsef and send us a confirmation or an error
	// Whait for it...
	
	// The module will contact the main server and retrieve all configuration data
	// If the data is new it will update its eeprom memory with the new configuration
	// In case the connection fails or server is down, arduino has previous 
	// config information stored in the memory.
	
	cReceived = false;
	while (!cReceived) {
		int last_com = receive_next_answer(01);
		if (last_com == 01) {
			print_ok();
			cReceived = true;
		}else if (last_com == 01) {
		
		}
	}
	
	// if configured succesfully -> print_ok();
	// else
	// first ask to USER if the DNS name is ok
	// get name from the ethernet module and show it to the user
	// is correct? 
		// yes try again
		// NO ask for a new name
	// If has changed get the name and send it back to ethernet module
	// If configuration still fail offer option to continue with previous configuration
}

void EthernetModuleReset () {
	// Serial.println (" * Reseting network module...");
	// Reset ethernet
	digitalWrite (ethReset, LOW);
	delay (1000);
	digitalWrite (ethReset, HIGH);
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

void print_and_release_label () {

	print_one_label ();
	
	// Wait for the printer to print a label
	boolean released = check_label_realeased (true);
	while (!released) {
		Serial.println("Label error, remove any label that might be left and press number 1 to try again or 2 to continue.");
		int button_pressed = return_pressed_button ();
		if (button_pressed == 2) break;
		
		Serial.println("Goto print position");
		go_to_memory_position (3);			// Print position
		print_one_label ();
		released = check_label_realeased (true);
	}
	
}

void print_one_label () {
	// Print one label 
	Serial.print ("Generate label (C04): ");						
	send_command (4);			// Print one label
	
	send_action_to_server(ask_for_label);
	
	// This is just to see if command was sent correctly
	if (receive_next_answer(01) == 01) { 	// Command accepted
		// Wait for answer (command 06 indicates sucsesful printing
		if (receive_next_answer(06) == 06) { 
			print_ok();				// All went OK
		}else{
			// print_fail();
			Serial.println (" * Expected response (C06)");
			//Serial.println(" * Press button 1 to continue");
			//press_button_to_continue (1);
		}
	}else{
		print_fail();
		Serial.println (" * Command (C04) Failed");
		//Serial.println(" * Press button 1 to continue");
		//press_button_to_continue (1);
	}
}


boolean check_label_realeased (boolean print) {
	
	boolean label = false;
	boolean timeout_label = false;
	int count = 0;
	if (print) Serial.print("Label released: ");
	
	// Check if we got a label, or we timeout
	while (!label && !timeout_label) {
		label = digitalRead (sensE); 
		count ++;
		if (count == 200) timeout_label = true;
		delay (50);
	}
	
	
	if (label) {
		if (print) print_ok();
		send_action_to_server(label_ok);
		delay (500);		// Just give sometime to the printer to finsh the job before we move
		Serial.println("Go to brush position");
		go_to_memory_position (20);
		label = digitalRead (sensE);		// After moving we check the label again, could be that wasn't completely stiked and moved on the way.
		if (!label) return false;
		return true;
	}

	if (print) print_fail ();
	send_error_to_server (label_timeout);
	return false;
}


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


boolean recevie_data (char* parameter_container,int buffer) {
	
	// first clean data
	int len = buffer;
	for (int c = 0; c < len; c++) {
		parameter_container[c] = 0;
	}

	while (true) {
		while (Serial.available()) {
			char c = (char) Serial.read();
			
			//Serial.print (Serial.read());	// JUST for debug
			
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
							if (commandNumberInt == 14) {
								parameter_container[strlen(parameter_container)] = '\0';
								return true;
								// End of data stream
							}else{
								return false;
							}
						}
					}else{
						// failed to process comand
					}
				}
				lookForLetter = true;
				numberIndex = 0;
			}else{
			
				if (lookForLetter && (c == 'C')) {
					//Serial.print ("-C detected-");
					// we got an incoming comand, start receive command number
					lookForNumber = true;
					incomingCommand = true;
					lookForLetter = false;
				}else if (lookForNumber) {
					//Serial.print ("-Number-");
					// We look for the command number
					commandNumber[numberIndex] = c;
					if (numberIndex == command_digits) { 
						reset_command ();	// Command invalid too many characters
					}
					numberIndex++;
				}else if (strlen(parameter_container) == buffer ) {
					// Serial.println (" Reached the data max lengh, we reset the tag" );
					// Error!! buffer overload
					return false;
				}else{
					// DATA comes here
					//Serial.print (c);
					parameter_container[strlen(parameter_container)]=c;
					// DEBUG IP
				}
			}
			

			//delay (100);		// just give enough time to receive another character if 
		}
	}
}

boolean recevie_data_and_print () {
	//boolean recevie_data (char* parameter_container,int buffer) 
	while (true) {
		while (Serial.available()) {
			char c = (char) Serial1.read();
			Serial.print (c);	// JUST for debug
			
			if (c == endOfLine) { 		// begining or end of command
				// In this case we check if we had previous data in the buffer and process it if necessary
				// restart all and ready to receive a commmand
				if (lookForNumber) {
					lookForNumber = false;
					if (processCommand()) {	// we got a valid command!						
						if (incomingCommand) {
							reset_command ();
							if (commandNumberInt == 14) {
								return true;
								// End of data stream
							}else{
								return false;
							}
						}
					}else{
						// failed to process comand
					}
				}
				lookForLetter = true;
				numberIndex = 0;
			}else{
			
				if (lookForLetter && (c == 'C')) {
					// we got an incoming comand, start receive command number
					lookForNumber = true;
					incomingCommand = true;
					lookForLetter = false;
				}else if (lookForNumber) {
					// We look for the command number
					commandNumber[numberIndex] = c;
					if (numberIndex == command_digits) { 
						reset_command ();	// Command invalid too many characters
					}
					numberIndex++;
				}
			}
		}
	}
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
	//Serial.print("SendC:");Serial.println(command);
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
	Serial1.print (data_to_send);
	send_command (14);		// END of data
}

void send_data (unsigned int data_to_send) {
	Serial1.print (data_to_send);
	send_command (14);		// END of data
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

*/