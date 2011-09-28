// Some predefined info

#define server_address	"office.pygmalion.nl"
#define server_script	"/labelgenerator/generate.php?batch_id=290"
// Password needs to be in form user:password ( all enconded in base 64)
// Check website http://maxcalci.com/base64.html for encoding/decoding
#define password		"YXJkdWlubzpQQXBhWXViQTMzd3I="
#define printer_IP		"10.10.249.105"  // subjetc to change
#define printer_port	"8000"

/*  test
SA: drbit.nl
SS: /index.php
IP: 10.10.249.105:8000
*/

void init_printer () {

	// Reset arduino
	Serial.println (" Configuring network ");
	Serial1.println("IN");				// Tell arduino to INIT
	delay(300);

	
	delay (300);
	Serial1.println("SA");				// Tell arduino next data is the server_address
	delay(300);
	Serial1.println(server_address);	// Send server_address
	delay(300);
	Serial1.println("SS");				// Tell arduino next data is the server_scrip
	delay(300);
	Serial1.println(server_script);		// Send server_script
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
	while (!serial1.available) {}
	while (Serial1.available) {
		char c = Serial1.read();
		Serail.print (c);
		delay (100);		// just give enough time to receive another character if 
	}
	
	// Confirmation of data:
	while (!serial1.available) {}
	while (Serial1.available) {
		char c = Serial1.read();
		if (c == "1") {
			Serial.println (" Configuring network ");
		}else if (c == "0") {
			Serial.println (" Configuring network ");
		}
		delay (100);		// just give enough time to receive another character if 
	}
	
	
	
	
	// We configure the arduino with the right directions
}
	
	
	

void print_label () {
	delay (5000);
}