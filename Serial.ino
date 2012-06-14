
void init_serial() {
  delay (200);
  Serial.begin (9600);
  Serial.println (" ");
  Serial.println ("*****************************");
  Serial.println ("** Seed Counter on-line    **");
  Serial.println ("*****************************");
  Serial.print   ("Version: ");
  Serial.println (version_prog);
  Serial.print   ("Library: V");
  Serial.println (lib_version);
  Serial.println (" ");
  print_set_up ();			// Begin SET-UP process
}


void print_ok () {
	Serial.println("[OK]");
}

void print_fail() {
	Serial.println("[FAIL]");
}

void print_set_up () {
	Serial.println("\n*****************");
	Serial.println("** SETTING UP  **");
	Serial.println("*****************");
}

