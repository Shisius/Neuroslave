
#include "ads1299_ctl.h"

unsigned char _set_by_mask(unsigned char src, unsigned char value, unsigned char mask)
{
	unsigned char pos = 0;
	if (mask == 0) return src;
	while (((mask >> pos) & 1) == 0) {pos++;}
	src &= ~(mask);
	return src | ( (value << pos) & mask );
}

unsigned char _get_by_mask(unsigned char src, unsigned char mask)
{
	unsigned char pos = 0;
	if (mask == 0) return 0;
	while (((mask >> pos) & 1) == 0) {pos++;}
	return (src >> pos) & mask;
}

Ads1299_Reg ads1299_get_config1(Ads1299_GlobalConfig * cfg)
{
	Ads1299_Reg reg;
	reg.addr = ADS1299_REG_CONFIG1;
	reg.value = 0x90;
	if (cfg->daisy_mode == 0)
		reg.value |= (ADS1299_DIS_DAISY_MODE_MASK);
	if (cfg->clk_output != 0)
		reg.value |= (ADS1299_EN_CLK_OUTPUT_MASK);
	unsigned char data_rate = 0x06;
	if (cfg->sample_rate > 64) cfg->sample_rate = 64;
	while ( (cfg->sample_rate >> (0x07 - data_rate) ) !=0 ) {data_rate--;}
	cfg->sample_rate = 1 << (0x06 - data_rate);
	reg.value |= data_rate & ADS1299_DATA_RATE_MASK;
	return reg;
}

void ads1299_glcfg_update(Ads1299_GlobalConfig * cfg, Ads1299_Reg * reg)
{
	switch (reg.addr)
	{
		case ADS1299_REG_ID:
		cfg->id = _get_by_mask(reg->value, ADS1299_REV_ID_MASK);
		cfg->n_channels = (_get_by_mask(reg->value, ADS1299_NUM_CH_MASK)) * 2 + 4;
		break;

		default:
		break;
	}
}
