/*

*/

#include <SimbleeBLE.h>
#define MCP_SS  7   // MCP Slave Select on Simblee pin 13
#define MCP_DRDY 8   // MCP DataReady on Simblee pin 8
#define MCP_RST 20   // MCP Reset: active low

#define NEW_MCP_DRDY 5
#define NEW_MCP_SS 3
#define NEW_MCP_RST 2

void setup() {
	// DRDY
	pinMode(MCP_DRDY, INPUT);
	pinMode(NEW_MCP_DRDY, OUTPUT);
	digitalWrite(NEW_MCP_DRDY, digitalRead(MCP_DRDY));
    attachPinInterrupt(MCP_DRDY, MCP_DRDY_ISR, LOW);
    // SS
    pinMode(MCP_SS, OUTPUT);
    digitalWrite(MCP_SS, LOW);
    //pinMode(NEW_MCP_SS, INPUT);
    //digitalWrite(MCP_SS, digitalRead(NEW_MCP_SS));
    //attachPinInterrupt(NEW_MCP_SS, MCP_SS_ISR, LOW);
    // RST
    pinMode(MCP_RST, OUTPUT); 
    digitalWrite(MCP_RST, LOW);
  	delay(100); 
  	digitalWrite(MCP_RST, HIGH);
}

void loop() {
  ;
}

int MCP_DRDY_ISR(uint32_t unused) { 

	digitalWrite(NEW_MCP_DRDY, digitalRead(MCP_DRDY));
	attachPinInterrupt(MCP_DRDY, MCP_DRDY_ISR, ~digitalRead(MCP_DRDY));

	return 0; 
}

int MCP_SS_ISR(uint32_t unused) { 

	digitalWrite(MCP_SS, digitalRead(NEW_MCP_SS));
	attachPinInterrupt(NEW_MCP_SS, MCP_SS_ISR, ~digitalRead(NEW_MCP_SS));

	return 0; 
}
