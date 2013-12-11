#define sensJ A2  // 1rst in the connector
#define sensK 11  // Last in the connector

void setup () {
	//here the setup data
	pinMode (sensK, INPUT);
	pinMode (sensJ, INPUT);
	Serial.begin (9600);
}

void loop () {
	// Here loop data
	
	unsigned int sensor = analogRead (sensJ);
	Serial.println (sensor);
	delay (1000);

}