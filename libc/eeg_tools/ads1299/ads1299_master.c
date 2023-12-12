
#include "ads1299_master.h"

void ads1299_spi_cs_default(unsigned char) {}

Ads1299_Master ads1299_create_master_default(void)
{
	Ads1299_Master master;
	// Config
	master.glcfg.daisy_mode = 0;
	master.glcfg.clk_output = 0;
	master.sample_rate = 1;

	master->set_reg = ads1299_set_reg;
	master->get_reg = ads1299_get_reg;

	master->spi_cs = ads1299_spi_cs_default;

	return master;
}

void ads1299_register_spi_cs(Ads1299_Master * master, void(*func)(unsigned char))
{
	master->spi_cs = func;
}
