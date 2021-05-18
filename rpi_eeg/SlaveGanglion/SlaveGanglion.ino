
#include "Ganglion_Slave.h"

GanglionSlave ganglion; 

volatile int spis_cnt = 0;

void setup() {
  Serial.begin(115200);
	ganglion.init();
	attachPinInterrupt(MCP_DRDY, MCP_ISR, LOW);
  Serial.println("Neuroslave 2.0");
} 

void loop() {
	while (1) {
		if (ganglion.mcp_data_ready) {
			ganglion.mcp_process_data();
			ganglion.mcp_data_ready = false;
		} 
	}

}

void SPIS_onReceive(int bytes_received, bool overflow)
{
	if (bytes_received <= 0)
		return;
  Serial.println("spis routine");
	ganglion.spis_routine(bytes_received, overflow);
  
}

int MCP_ISR(uint32_t unused) { 

	ganglion.mcp_data_ready = true;
	ganglion.mcp_sample_counter++;

	return 0; 
}
