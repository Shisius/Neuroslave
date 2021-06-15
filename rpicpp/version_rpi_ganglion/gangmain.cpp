/*

*/

#include <unistd.h>
#include "iostream"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <atomic>
#include <thread>
#include <algorithm>
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

enum class NeuroslaveSampleState : uint8_t {
    GOOD = 0,
    INDEX_ERROR = 1
};

struct TcpHeader{
    uint32_t Label : 16;
    uint32_t payload_length : 8;
    uint32_t state : 8;
};// __attribute__((__packed__));

struct TcpSample
{
	TcpHeader header;
	int32_t eeg_data[4];

	void to_char(char* msg) {
		memcpy(msg, &(header), 2);
		std::reverse(msg, msg+2);
		memcpy(msg + 2, (char*)(&(header))+2, 2);
		for (unsigned int i = 0; i < 4; i++) {
			memcpy(msg + 4 + i*4, &(eeg_data[0]) + i, 4);
			std::reverse(msg + 4 + i*4, msg + 4 + i*4 + 4);
			//std::reverse(msg + 4 + i*4 + 2, msg + 4 + i*4 + 4);
		}
	}
};// __attribute__((__packed__));

// TcpSample mcp2tcp(McpSample & mcp_sample)
// {
// 	TcpSample tcp_sample;
// 	tcp_sample.header = 0xACDC0400;
// 	memcpy(tcp_sample.eeg_data, mcp_sample.eeg_data, 16);
// 	if (mcp_sample.state == 30)
// 		tcp_sample.header |= 1;
// 	return tcp_sample;
// }

void mcp2tcp(McpSample & mcp_sample, TcpSample & tcp_sample)
{
	tcp_sample.header.Label = 0xACDC;
	tcp_sample.header.payload_length = 4;
	tcp_sample.header.state = 0;

	//tcp_sample.eeg_data[0] = 0x00010000;
	//tcp_sample.eeg_data[1] = 0x00010000;
	//tcp_sample.eeg_data[2] = 0x00010000;
	//tcp_sample.eeg_data[3] = 0x00010000;
	memcpy(tcp_sample.eeg_data, mcp_sample.eeg_data, 16);
	if (mcp_sample.state == 30)
		tcp_sample.header.state = 0x01;
}

void mcp2tcp(McpSample & mcp_sample, TcpSample * tcp_sample)
{
	tcp_sample->header.Label = 0xACDC;
	tcp_sample->header.payload_length = 4;
	tcp_sample->header.state = 0;

	tcp_sample->eeg_data[0] = 0x00010000;
	tcp_sample->eeg_data[1] = 0x00010000;
	tcp_sample->eeg_data[2] = 0x00010000;
	tcp_sample->eeg_data[3] = 0x00010000;
	//memcpy(tcp_sample.eeg_data, mcp_sample.eeg_data, 16);
	if (mcp_sample.state == 30)
		tcp_sample->header.state = 0x01;
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
	std::cout << answer_turnon;
	std::string answer_turnon_accepted = "TurnOn:Accepted\n\r";
	std::string answer_user = "Users:[\"Antony\"]\n\r";
	std::cout << answer_user;
	std::string answer_user_choosen = "User:Antony\n\r";
	std::string answer_user_accepted = "User:Accepted\n\r";
	std::cout << "Recv thread\n";
	std::string msg = "";
	TcpServerStream msg_srv(7239, 0, 100);
	msg_srv.start();
	std::cout << "Recv started\n";
	while (is_alive.load(std::memory_order_relaxed)) {
		if (msg_srv.receiveMessage(msg)) {
			std::cout << "Command: " << msg << std::endl;
			if (msg.compare(0, 6, "TurnOn") == 0) {
				//msg_srv->sendMessage(answer_turnon_accepted);
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				msg_srv.sendMessage(answer_turnon);
				is_started.store(true, std::memory_order_relaxed);
			} else if (msg.compare(0, 6, "TurnOf") == 0) {
				is_started.store(false, std::memory_order_relaxed);
				break;
			} else if (msg.compare(0, 6, "User\n\r") == 0) {
				//msg_srv->sendMessage(answer_user_accepted);
				//std::this_thread::sleep_for(std::chrono::milliseconds(100));
				msg_srv.sendMessage(answer_user);
			} else if (msg.compare(0, 5, "User:") == 0) {
				msg_srv.sendMessage(answer_user_accepted);
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	is_alive.store(false, std::memory_order_relaxed);
	msg_srv.stop();

	std::cout << "Recv finished\n";
}

std::string tcp_sample2msg(TcpSample * tcp_sample) {
	std::string msg;
	msg.resize(sizeof(TcpSample));
	msg.copy((char*)tcp_sample, sizeof(TcpSample));
	//std::reverse(msg.begin(), msg.end());
	return msg;
}

void send_process()
{
	std::cout << "Send thread\n";
	std::string msg;
	int s_per_msg = 100;
	McpSample mcp_sample;
	msg.resize(sizeof(TcpSample)*s_per_msg);
	//TcpSample * tcp_sample = new TcpSample();
	TcpSample tcp_sample;
	char * tcp_chr = new char[20];
	//char chr_msg[sizeof(TcpSample)];
	//uint16_t text[10] = {0xdcac, 0x0004, 0xacdc, 0x0400, 0x0004, 0xdcac, 0x0400, 0xacdc, 0x1234, 0x0239};//{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t'};

	TcpServerStream data_srv(8239, 0, 10);
	data_srv.start();
	std::cout << "Send started\n";
	uint32_t i_tcp_pack = 1;
	while (is_alive.load(std::memory_order_relaxed)) {
		if (is_started.load(std::memory_order_relaxed)) {
			sample_queue.wait_and_pop(mcp_sample);
			mcp2tcp(mcp_sample, tcp_sample);
			//tcp_chr = (char*)&tcp_sample;
			tcp_sample.to_char(tcp_chr);
			
			//msg.copy(tcp_chr, sizeof(TcpSample));
			memcpy(msg.data() + (sizeof(TcpSample)) * ((i_tcp_pack - 1) % s_per_msg), tcp_chr, sizeof(TcpSample));
			if (i_tcp_pack % s_per_msg == 0) {
				//std::cout << tcp_sample.header.Label << " " << sizeof(TcpSample) << " " << std::to_string(static_cast<int>(msg[0] << 8) + (static_cast<int>(msg[1]))) << "\n";
				//std::cout << mcp_sample.eeg_data[0] << " " << std::to_string(static_cast<int>(tcp_chr[4])) << " " << std::to_string(static_cast<int>(msg[5])) << 
				//	" " << std::to_string(static_cast<int>(msg[6])) << " " << std::to_string(static_cast<int>(msg[7])) << std::endl;
				data_srv.sendMessage(msg);
			}
			//data_srv.sendMessage(tcp_sample2msg(tcp_sample));
			//data_srv.sendMessage((char*)tcp_sample, 20);
			i_tcp_pack++;
		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	data_srv.stop();

	//delete tcp_sample;
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
			if (mcp_sample.state == 30) {
				std::cout << "30!\n";
			}
			if (mcp_sample.state == 555) {
				counter555++;
				std::this_thread::sleep_for(std::chrono::microseconds(500));
			} else {
				i_sample++;
				sample_queue.push(mcp_sample);
			}

		} else
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	std::cout << "N samples: " << i_sample << " counter555: " << counter555 << std::endl;
}

int main(int argc, char** argv)
{

	is_alive.store(true, std::memory_order_relaxed);

	wiringPiSetup();

	//TcpSample tcpexample;
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
