/*

*/

#include <unistd.h>
#include "iostream"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <thread>
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

struct TcpSample
{
	uint32_t header = 0xACDC0400;
	int32_t eeg_data[4];
};

TcpSample mcp2tcp(McpSample & mcp_sample)
{
	TcpSample tcp_sample;
	memcpy(tcp_sample.eeg_data, mcp_sample.eeg_data, 16);
	if (mcp_sample.state == 30)
		tcp_sample.header |= 1;
	return tcp_sample;
}

const uint32_t n_samples = 100000;
volatile uint32_t i_sample = 0;
McpSample mcp[n_samples + 100];
std::atomic<bool> drdy;

TcpServerStream * msg_srv;
TcpServerStream * data_srv;
std::atomic<bool> is_alive;
std::atomic<bool> is_started;
threadsafe_queue<McpSample> sample_queue;

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

void recv_process()
{
	std::string msg;
	msg_srv = new TcpServerStream(7239);
	msg_srv->start();
	while (true) {
		if (msg_srv->receiveMessage(msg)) {
			if (msg.compare(0, 6, "TurnOn")) {
				is_started.store(true, std::memory_order_relaxed);
			} else if (msg.compare(0, 7, "TurnOff")) {
				is_started.store(false, std::memory_order_relaxed);
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	is_alive = false;
	delete msg_srv;
}

void send_process()
{
	std::string msg;
	msg.resize(sizeof(TcpSample));
	data_srv = new TcpServerStream(8239);
	data_srv->start()
	McpSample mcp_sample;
	TcpSample tcp_sample;

	while (is_alive.load(std::memory_order_relaxed)) {
		if (is_started.load(std::memory_order_relaxed)) {
			sample_queue.wait_and_pop(mcp_sample);
			tcp_sample = mcp2tcp(mcp_sample);
			memcpy(msg.data(), &tcp_sample, sizeof(TcpSample));
			data_srv->sendMessage(msg);
		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	delete data_srv;
}

void spi_process()
{
	bool isr_mode = false;
	uint32_t counter555 = 0;
	uint8_t cmd = 2;
	McpSample mcp_sample;
	drdy.store(false, std::memory_order_relaxed);
	std::cout << "SPI_Setup: " <<  wiringPiSPISetup (0, 10000000) << std::endl;
	std::cout << "ISR setup: " << wiringPiISR (6, INT_EDGE_RISING,  drdy_routine) << std::endl;

	i_sample = 0;
	while (!(is_alive.load(std::memory_order_relaxed))) {
		if (is_started.load(std::memory_order_relaxed)) {
			if (isr_mode && i_sample != 0) {
				while (!(drdy.load(std::memory_order_relaxed))) {usleep(50);}
				drdy.store(false, std::memory_order_relaxed);
			}
			if (i_sample == 0)
				cmd = 2;
			else cmd = 1;
			memset((void*)&mcp_sample, 0, sizeof(McpSample));
			wiringPiSPIDataRW (0, &cmd, sizeof(McpSample)) ;
			wiringPiSPIDataRW (0, (unsigned char*)&mcp_sample, sizeof(McpSample)) ;	
			i_sample++;
			if (mcp_sample.state == 555) {
				counter555++;
				std::this_thread::sleep_for(std::chrono::microseconds(1000));
			} else
				sample_queue.push(mcp_sample);

		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout << "N samples: " << i_sample << " counter555: " << counter555 << std::endl;
}

int main(int argc, char** argv)
{

	is_alive.store(true, std::memory_order_relaxed);;

	wiringPiSetup();	

	scoped_thread recv_thread(std::thread(recv_process));
	scoped_thread send_thread(std::thread(send_process));
	scoped_thread spi_thread(std::thread(spi_process));

	while(!(is_alive.load(std::memory_order_relaxed))) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	printf("Finished!\n");
	
	return 0;
}
