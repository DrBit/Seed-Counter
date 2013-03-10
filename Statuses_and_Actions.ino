
boolean check_status (boolean safe) {

	if (safe) {
		// Restore the flag endingBatch cause we now are at a save place and we can restart
		endingBatch = false;
		do_a_restart = false;
		// endingBatch is enabled in "counter" inside function "pick a seed" and disables the basic functions of the loop
		// Should we also enable this variable within this function also?
	}
	check_server();		// Should we?
	
	#if defined bypass_server 
	global_status = S_running;
	#endif

	int done = false;
	while (global_status != S_running && global_status != S_pause) {
		switch (global_status) {

			case S_finishing_batch: {
				Serial.println(F(" **Finishing batch - Checking server global status... "));
				// We should wait until we have successfully finished last batch.
				if (safe) {
					send_status_to_server (S_stopped);
				}else{
					if (previous_status != S_finishing_batch) send_status_to_server (S_finishing_batch);
					return true;
				}
			break;}

			case S_stopped: {
				if (!done) start_idle_timer (default_idle_time);
				done = true;
				go_to_safe_position();		// We go to a safe place so we can stop
				while (global_status == S_stopped) {
					#if defined Server_com_debug
					Serial.println(F(" **STOP - Checking server global status... "));
					#endif
					delay(1000);
					check_server();
					check_idle_timer (true);
				}
			break;}

			case S_switch_off: {
				go_to_safe_position();		// We go to a safe place so we can disconect
				switch_off_machine ();		// Switch off machine
				while (global_status == S_switch_off) {
					// Do nothing while everithing is off
					#if defined Server_com_debug
					Serial.println(F(" **OFF - Checking server global status... "));
					#endif
					check_server();
					delay (5000);
				}
				send_action_to_server(power_on); 
				reset_machine ();		// When changes we will switch back ON
				start_idle_timer (default_idle_time);		// Reset the timings for IDLE as we are not leaving this hole while
			break;}

			case S_test: {
				#if defined Server_com_debug
				Serial.println(F(" ** Enterning TEST MODE"));
				#endif	
				end_idle_timer();

				while (global_status == S_test) {
					check_server();
					// check_idle_timer (true);
				}
			break;}

			case S_setting_up: {
				if (!block_loop) {			// If we already are inside a block loop just ingnore all, we should first go off.
					if (safe) {				// If it is safe to do a restart we do it
						if (do_a_restart) do_a_restart = false;		// If we got a flag to restart first reset the blag
						#if defined Server_com_debug
						Serial.println(F(" ** Setting UP"));
						#endif
						reset_machine ();
						start_idle_timer (default_idle_time);		// We restart the timer as there was a call inside reset_machine
					}
				}
			break;}

			default: {
				#if defined Server_com_error_debug
				Serial.print(F(" ** Status not recognized: "));
				Serial.println(global_status);
				#endif
			break;}
		}
	}
	// global_status MUST be ready to get here
	
	// Emergency button handler
	int button_emergency = digitalRead (emergency); // Conected at sens C?????
	// if (button_emergency) {		// Bypas for now,, re-enable when connected
	if (false) {
		send_status_to_server (S_pause);
		Serial.println ("Emergency Enabled");
		MySW.stop();
		// Send error
		start_idle_timer (default_idle_time);
		while (button_emergency) {
			button_emergency = digitalRead (emergency); 
			delay (500);
			check_idle_timer (true);
		}
		end_idle_timer ();
		MySW.start();
		Serial.println ("Emergency disabled");
		send_status_to_server (previous_status);
	}
	
	// Checking pause in the software
	if ((pause || global_status == S_pause) && !manual_enabled) {
		// Record actual position
		record_actual_position ();
		// go to a safe position
		go_to_safe_position();		// We go to a safe place so we can securily pause

		pause = true;
		MySW.stop();
		// We don't have a timer here cause we can not reestart after unpause
		// Must be enabled and disable by a user.
		pump_disable ();		// We do only switch off the pump
		while(pause) {
			check_server();
			if (global_status != S_pause) {
				pause = false;
			}
		}
		pump_enable ();
		MySW.start();
		pause = false;
		// go back to last saved position
		go_to_last_saved_position ();
	}
	end_idle_timer();
}



//////////////////////////////////////////////////////////////////////////////////////////
boolean receive_server_data (){

	clean_buffer (received_msg,bufferSize);		// Prepare buffer
	
	// wait 3 seconds for incoming data before a time out
	// 100ms * 10 = 1s so... 100*20 = 2000 (time to wait 2 seconds)
	int times_to_try = 60;
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
					case button_finish: {
						server_answer = button_finish;
					break;}
				}
				#if defined Server_com_debug
				// Inform
				Serial.print(F("Received Action "));
				Serial.print(receiving_info);
				Serial.print(F(" - "));
				#endif
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
				if (global_status == S_finishing_batch) {
					// We have to finish the batch and get into stop
					// send_status_to_server (S_finishing_batch);
					Serial.print(F("Finishing Batch - "));
					// go_to_print_position();
					//endingBatch = true;			// Skyp all functions and go to starting point
				}

				// If we receive a restart command while we are inside a block loop (error or setup) we flag it so we know we should go OUT first
				if ((global_status == S_setting_up) && block_loop) {
					do_a_restart = true;
				}
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

					case Cget_autoreset_state: {	// Should we define the seeds per blister? for now 2 modes 1 or 2 (10 or 5 seeds)
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						int receiving_reset_state = atoi(thisChar);
						if (receiving_reset_state == 0) autoreset = false;		// disables autoreset
						if (receiving_reset_state == 1) autoreset = true;		// enables autoreset
					break; }

					case Cget_autoreset_value: {	// Should we define the seeds per blister? for now 2 modes 1 or 2 (10 or 5 seeds)
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned int receiving_reset_value = atoi(thisChar);
						blisters_for_autoreset = receiving_reset_value;
					break; }

					case Cget_server_polling_time: {	
						recevie_data_telnet (received_msg,bufferSize);
						char * thisChar = received_msg;
						unsigned long receiving_Spolling_value = atoi(thisChar);
						polling_server_rate = receiving_Spolling_value;
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

					case Cget_autoreset_state: {
						Serial.print(F(" - Autoreset State: "));
						if (autoreset) {
							Serial.println(F("Enabled"));
						} else {
							Serial.println(F("Disabled"));
						} 
					break; }

					case Cget_autoreset_value: {
						Serial.print(F(" - Autoreset Value: "));
						Serial.println(blisters_for_autoreset);
					break; }

					case Cget_server_polling_time: {	
						Serial.print(F(" - Server polling time Value: "));
						Serial.println(polling_server_rate);
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
		timeout = 0;
		#endif
		if (timeout >= times_to_try) Serial.println(F("*timeout answer*"));
	#endif
	if (receivedK) {
		return true;
	}else{
		return false;
	}
	// DONE!
}



	
void clean_serial_buffer () {
	while (Serial.available () > 0) {
		int null;
		null = Serial.read();
	}
}

void check_serial_answer () {
	if (Serial.available () > 0) {
		char incomming = Serial.read();			// continue, ignore, finish

		if (incomming == '1') {
			server_answer = button_continue;
		}
		if (incomming == '2') {
			server_answer = button_ignore;
		}
		if (incomming == '3') {
			server_answer = button_finish;
		}
	}
}