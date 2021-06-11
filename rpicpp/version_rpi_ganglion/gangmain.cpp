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
	std::string answer_turnon = "EegSession:{\"tag\":\"hep\",\"sample_rate\":1600,\"n_channels\":4,\"gain\":1,\"tcp_decimation\":1}\n\r";
	std::string answer_user = "Users:[\"Antony\"]\n\r";
	std::string answer_user_choosen = "User:Antony\n\r";
	std::cout << "Recv thread\n";
	std::string msg;
	TcpServerStream * msg_srv = new TcpServerStream(7239, 0, 100000);
	msg_srv->start();
	std::cout << "Recv started\n";
	while (true) {
		if (msg_srv->receiveMessage(msg)) {
			std::cout << "Command: " << msg << std::endl;
			if (msg.compare(0, 6, "TurnOn")) {
				msg_srv->sendMessage(answer_turnon);
				is_started.store(true, std::memory_order_relaxed);
			} else if (msg.compare(0, 7, "TurnOff")) {
				is_started.store(false, std::memory_order_relaxed);
				break;
			} else if (msg.compare(0, 5, "User\n")) {
				msg_srv->sendMessage(answer_user);
			} else if (msg.compare(0, 5, "User:")) {
				msg_srv->sendMessage(answer_user_choosen);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	is_alive.store(false, std::memory_order_relaxed);
	delete msg_srv;

	std::cout << "Recv finished\n";
}

void send_process()
{
	std::cout << "Send thread\n";
	std::string msg;
	McpSample mcp_sample;
	TcpSample tcp_sample;
	msg.resize(sizeof(TcpSample));

	TcpServerStream * data_srv = new TcpServerStream(8239, 0, 100000);
	data_srv->start();
	std::cout << "Send started\n";

	while (is_alive.load(std::memory_order_relaxed)) {
		if (is_started.load(std::memory_order_relaxed)) {
			sample_queue.wait_and_pop(mcp_sample);
			tcp_sample = mcp2tcp(mcp_sample);
			memcpy(msg.data(), &tcp_sample, sizeof(TcpSample));
			data_srv->sendMessage(msg);
		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	delete data_srv;
}

void spi_process()
{
	std::cout << "SPI thread\n";
	bool isr_mode = false;
	uint32_t counter555 = 0;
	uint8_t cmd = 2;
	McpSample mcp_sample;
	drdy.store(false, std::memory_order_relaxed);
	std::cout << "SPI_Setup: " <<  wiringPiSPISetup (0, 10000000) << std::endl;
	if (isr_mode)
		std::cout << "ISR setup: " << wiringPiISR (6, INT_EDGE_RISING,  drdy_routine) << std::endl;

	std::cout << "SPI started\n";
	i_sample = 0;
	while (is_alive.load(std::memory_order_relaxed)) {
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

	is_alive.store(true, std::memory_order_relaxed);

	wiringPiSetup();

	std::cout << "Pi setupped\n";	

	std::thread recv_thread(recv_process);
	std::thread send_thread(send_process);
	std::thread spi_thread(spi_process);

	while(is_alive.load(std::memory_order_relaxed)) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	spi_thread.join();
	send_thread.join();
	recv_thread.join();

	printf("Finished!\n");
	
	return 0;
}
