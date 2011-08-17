
void init_serial() {
  Serial.begin (9600);
  Serial.println ("*****************************");
  Serial.println ("** Seed Counter on-line    **");
  Serial.println ("*****************************");
  Serial.print   ("Version: ");
  Serial.println (version_prog);
  Serial.println (" ");
}


void print_ok () {
	Serial.println("[OK]");
}

void print_fail() {
	Serial.println("[FAIL]");
}
