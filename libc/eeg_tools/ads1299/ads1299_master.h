#ifndef _ADS1299_MASTER_H_
#define _ADS1299_MASTER_H_

#include "ads1299_ctl.h"

typedef unsigned int[8] ads1299_sample_t;

typedef struct Ads1299_Master Ads1299_Master;
struct Ads1299_Master
{
	Ads1299_GlobalConfig glcfg;
	Ads1299_ChanConfig chcfg[8];

	// Methods
	void (*send_cmd)(Ads1299_Master * master, unsigned char cmd);
	void (*set_reg)(Ads1299_Master * master, unsigned char addr, unsigned char value);
	unsigned char (*get_reg)(Ads1299_Master * master, unsigned char addr);

	void (*push_config)(Ads1299_Master * master);
	void (*pull_config)(Ads1299_Master * master);

	void (*start)(Ads1299_Master * master);
	// hardware functions
	void (*spi_cs)(unsigned char value);
	void (*spi_read_buf)(unsigned char * buf, unsigned char len);
	void (*spi_write_buf)(unsigned char * buf, unsigned char len);
	void (*delay_us)(unsigned int us);

};

Ads1299_Master ads1299_create_master_default(void);

void ads1299_register_spi_cs(Ads1299_Master * master, void(*func)(unsigned char));


void ads1299_set_reg(Ads1299_Master * master, unsigned char addr, unsigned char value);
unsigned char ads1299_get_reg(Ads1299_Master * master, unsigned char addr);

#endif //_ADS1299_MASTER_H_
