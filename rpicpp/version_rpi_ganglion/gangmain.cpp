/*

*/

#include <unistd.h>
#include "iostream"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
extern "C" {
	#include <wiringPiSPI.h>
	#include <wiringPi.h>
}

#include "tcpserverstream.h"

struct McpSample
{
	uint32_t sample_index = 0;
	int32_t eeg_data[4];
	uint32_t state = 0;
};

const uint32_t n_samples = 100000;
volatile uint32_t i_sample = 0;
McpSample mcp[n_samples + 100];
uint8_t cmd_start = 2;
std::atomic<bool> drdy;

void drdy_routine(void) {
	// if (i_sample >= n_samples) {
	// 	i_sample++;
	// 	return;
	// }
	// cmd_start = 1;
	// memset((void*)&mcp[i_sample], 0, sizeof(McpSample));
	// wiringPiSPIDataRW (0, &cmd_start, sizeof(cmd_start)) ;
	// wiringPiSPIDataRW (0, (unsigned char*)&mcp[i_sample], sizeof(McpSample)) ;
	// i_sample++;
	drdy.store(true, std::memory_order_relaxed);
}

int main(int argc, char** argv)
{

	
	
	wiringPiSetup();
	uint32_t s555 = 0;
	drdy.store(false, std::memory_order_relaxed);
	printf("Hep\n");
	std::cout << wiringPiSPISetup (0, 10000000) << std::endl;


	std::cout << wiringPiISR (6, INT_EDGE_RISING,  drdy_routine) << std::endl;

	memset((void*)&mcp[0], 0, sizeof(McpSample));
	wiringPiSPIDataRW (0, &cmd_start, sizeof(McpSample)) ;
	wiringPiSPIDataRW (0, (unsigned char*)&mcp[0], sizeof(McpSample)) ;
	i_sample++;
	for (uint32_t i = 1; i < n_samples; i++) {
		while (!(drdy.load(std::memory_order_relaxed))) {usleep(50);}
		drdy.store(false, std::memory_order_relaxed);
		cmd_start = 1;
		memset((void*)&mcp[i], 0, sizeof(McpSample));
		wiringPiSPIDataRW (0, &cmd_start, sizeof(McpSample)) ;
		wiringPiSPIDataRW (0, (unsigned char*)&mcp[i], sizeof(McpSample)) ;		
	}
	printf("Finished\n");
	
	usleep(1000);
	for (uint32_t i = 0; i < n_samples; i++) {
		if ((mcp[i].state != 239)) {
			if (mcp[i].state == 555)
				s555++;
			else
				printf("%d %i %d\n", mcp[i].sample_index, mcp[i].eeg_data[0], mcp[i].state);
		}
	}
	printf("first %d\n", mcp[0].sample_index);
	printf("last %d\n", mcp[n_samples-1].sample_index);
	printf("555: %d\n", s555);
	
	return 0;
}