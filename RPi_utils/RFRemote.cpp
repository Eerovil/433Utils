/*
   RFSniffer

Usage: ./RFSniffer [<pulseLength>]
[] = optional

Hacked from http://code.google.com/p/rc-switch/
by @justy to provide a handy RF code sniffer
 */
#include "../rc-switch/RCSwitch.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <curl/curl.h>
#include <unistd.h>


RCSwitch mySwitch;
static const size_t CODECAPACITY = 10;

struct HASwitch {
	int code[CODECAPACITY];
	std::string command;
};



static const size_t CAPACITY = 8;

HASwitch SWITCHES[CAPACITY] =  
{	
	/* BEGIN SILRVERCREST : A ON, A OFF, B ON,  etc.*/
	{{83029,4760496,4532016,5226560,4871280}, "irsend SEND_ONCE PhilipsPFT KEY_POWER"},
	{{83028,5112336,4438416,4315296,4486432}, "irsend SEND_ONCE PhilipsPFT KEY_POWER"},
	{{86101,4871284,5226564,4760500,4532020}, "olohuone_jalkalamppu.on"},
	{{86100,4486436,4438420,4315300,5112340}, ""},
	{{70741,4760508,4871292,5226572,4532028}, ""},
	{{70740,5112348,4486444,4315308,4438428}, ""},
	{{21589,4315298,5112338,4438418,4486434}, "irsend SEND_ONCE PhilipsPFT KEY_VOLUMEUP"},
	{{21588,4871282,4760498,4532018,5226562}, "irsend SEND_ONCE PhilipsPFT KEY_VOLUMEDOWN"}
	/* END SILVERCREST */
};

void handleCommand( std::string command) {
	if (command.size() == 0) {return;}
	if (command.substr(0,6) == "irsend") {
		system(command.c_str());
		return;
	}

	usleep(500000);


	CURL *curl;

	curl = curl_easy_init();
	CURLcode res; 
	struct curl_slist *chunk = NULL;

	chunk = curl_slist_append(chunk, "Accept: application/json");
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	chunk = curl_slist_append(chunk, "charsets: utf-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

	std::string entityid = command.substr(0, command.find("."));
	std::string turn_on = command.substr(command.find(".") + 1);

	std::string json = "{\"entity_id\": \"" + entityid + "\"}";
	printf("json: %s\n",json.c_str());



	curl_easy_setopt(curl, CURLOPT_POSTFIELDS,json.c_str());

	std::stringstream ss;
	ss << "https://REDACTEDorg/api/services/switch/turn_" << turn_on << "?api_password=REDACTED";
	std::string api_url = ss.str();
	curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());

	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	res = curl_easy_perform(curl);
	/* Check for errors */ 
	if(res != CURLE_OK)
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
	printf("Cleaning up...\n");
	curl_easy_cleanup(curl);
}

int main(int argc, char *argv[]) {

	// This pin is not the first pin on the RPi GPIO header!
	// Consult https://projects.drogon.net/raspberry-pi/wiringpi/pins/
	// for more information.
	int PIN = 4; 

	struct timespec start, finish;


	clock_gettime(CLOCK_REALTIME, &start);

	if(wiringPiSetup() == -1) {
		printf("wiringPiSetup failed, exiting...");
		return 0;
	}

	int pulseLength = 0;
	if (argv[1] != NULL) pulseLength = atoi(argv[1]);

	mySwitch = RCSwitch();
	if (pulseLength != 0) mySwitch.setPulseLength(pulseLength);
	mySwitch.enableReceive(PIN);  // Receiver on interrupt 0 => that is pin #2

	printf("listening on pin %i\n", PIN);
	int prevValue = 0;
	int value = 0;
	while(1) {
		usleep(500000);
		
		prevValue = value;
		if (mySwitch.available() || prevValue != 0) {
			value = mySwitch.getReceivedValue();
			bool detected = false;
			if (value == 0 && prevValue == 0) {
				printf("Unknown encoding\n");
			} else {    
				int code = value;
				if (code == 0) {
					printf("Using previous value.\n");
					code = prevValue;
				}
				for (int i=0; i<CAPACITY; i++) {
					for (int k=0; k<CODECAPACITY; k++){
						if (SWITCHES[i].code[k] == code) {
							std::cout << SWITCHES[i].command << "\n"; 
							handleCommand(SWITCHES[i].command);
							detected = true;
						}
					}
				}
			}

			mySwitch.resetAvailable();

			if (!detected) {
				printf("Received %i,protocol %i\n", mySwitch.getReceivedValue(), mySwitch.getReceivedProtocol() );
			} 
		}




	}
	exit(0);


}

