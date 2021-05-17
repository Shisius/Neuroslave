#include "Ganglion_Slave.h"

GanglionSlave::GanglionSlave() {
	;
}

void GanglionSlave::init()
{
	init_led();
	led_off();
	init_spi();
	init_spis();
	init_mcp();
	mcp_config(GAIN_1, SAMPLE_RATE_1600);
	mcp_turn_on_channels();
	led_on();
}

// SPI
void GanglionSlave::init_spi()
{
	SPI.begin();
	SPI.setFrequency(MCP_SPI_KHZ);
	SPI.setDataMode(SPI_MODE0);
}

// SPIS

void GanglionSlave::init_spis()
{
	SPIS.begin(spis_rx, spis_tx, sizeof(spis_rx));
	SPIS.setDataMode(SPI_MODE0);
    SPIS.setBitOrder(MSBFIRST);
    pinMode(SPIS_DATA_READY, OUTPUT);
}

void GanglionSlave::spis_routine(int bytes_received, bool overflow)
{
	if (bytes_received > 0) {
		switch (spis_rx[0]) {
			case SPIS_ANSWER:
				digitalWrite(SPIS_DATA_READY, 0);
				return;
			case SPIS_START:
				mcp_sample_queue.pop_new((McpSample*)spis_tx);
				break;
			case SPIS_MCP_SAMPLE:
				mcp_sample_queue.pop_wait((McpSample*)spis_tx);
				break;

		}
		digitalWrite(SPIS_DATA_READY, 1);
	}
}

// LED

void GanglionSlave::init_led() {
	pinMode(LED, OUTPUT);
}
void GanglionSlave::led_on() {
	digitalWrite(LED, 1);
}
void GanglionSlave::led_off() {
	digitalWrite(LED, 0);
}

// MCP3912

void GanglionSlave::init_mcp()
{
	pinMode(MCP_DRDY,INPUT);
	pinMode(MCP_SS,OUTPUT); 
	digitalWrite(MCP_SS,HIGH);
	pinMode(MCP_RST,OUTPUT); 
	// RESET MCP
	digitalWrite(MCP_RST,LOW);
	delay(100); 
	digitalWrite(MCP_RST,HIGH);
}

void GanglionSlave::mcp_send_cmd(uint8_t address, uint8_t rw)
{
  	SPI.transfer(DEV_ADD | address | rw);
}

int32_t GanglionSlave::mcp_read_register()
{
	int32_t reg = SPI.transfer(0x00);
  	reg <<= 8;
  	reg |= SPI.transfer(0x00);
  	reg <<= 8;
  	reg |= SPI.transfer(0x00);

  	return reg;
}

void GanglionSlave::mcp_write_register(uint32_t settings)
{
  SPI.transfer((settings & 0x00FF0000) >> 16);
  SPI.transfer((settings & 0x0000FF00) >> 8);
  SPI.transfer(settings & 0x000000FF);
}

void GanglionSlave::mcp_process_data()
{
	McpSample new_sample;
//  int32_t tmp_sample;
	// Read sample from MCP3912
	digitalWrite(MCP_SS, LOW);
	mcp_send_cmd(CHAN_0, MCP_READ);
	for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
//    tmp_sample = mcp_read_register();
//    tmp_sample = conv24to32(tmp_sample);
//		memcpy(new_sample.eeg_data + i, &tmp_sample, sizeof(int32_t));
    new_sample.eeg_data[i] = conv24to32(mcp_read_register());
	}
	digitalWrite(MCP_SS, HIGH);
	new_sample.sample_index = mcp_sample_counter;

	// Store sample in buffer
	mcp_sample_queue.push(new_sample);
}

void GanglionSlave::mcp_config(uint32_t gain, SAMPLE_RATE sample_rate)
{
	digitalWrite(MCP_SS, LOW);
	mcp_send_cmd(GAIN, MCP_WRITE);
	mcp_write_register(gain);          // GAIN_1, _2, _4, _8, _16, _32
	mcp_write_register(0x00B9000F);    // STATUSCOM auto increment TYPES DR in HIZ
	mcp_write_register(0x003C0050 | (sample_rate << 13));    // CONFIG_0: dither on max, boost 2x, OSR 4096, 0x003CE050 | sample rate: 50, 100, 200, 400
	mcp_write_register(0x000F0000);    // CONFIG_1:  put the ADCs in reset, external oscillator
	digitalWrite(MCP_SS, HIGH);
}

void GanglionSlave::mcp_turn_on_channels() {
	digitalWrite(MCP_SS, LOW);
	mcp_send_cmd(CONFIG_1, MCP_WRITE);
	mcp_write_register(0x00000000);
	digitalWrite(MCP_SS, HIGH);
}

int32_t conv24to32(int32_t value24)
{
	if ((value24 & 0x00800000) > 0) 
    	return value24 | 0xFF000000;
    else 
      	return value24 & 0x00FFFFFF;
}
