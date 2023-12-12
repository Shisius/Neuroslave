#ifndef _ADS1299_CTL_H_
#define _ADS1299_CTL_H_

#include "ads1299_defs.h"

typedef struct _ads1299_reg
{
	unsigned char addr;
	unsigned char value;
} Ads1299_Reg;

#define ADS1299_SAMPLE_RATE_BASE 250 // (ADS1299_FCLK_DEFAULT >> 13)

typedef struct _adds1299_global_config
{
	// ID
	unsigned char id;			// Trash. Read Only
	unsigned char n_channels;	// Read only!
	// Config 1
	unsigned char daisy_mode;
	unsigned char clk_output;
	unsigned char sample_rate;	// *250 samples per sec
	// Config 2
	unsigned char test_sig_internal; // 0 for external
	unsigned char test_sig_amp; // (value+1) * (-(VREFP-VREFN)/2400)
	unsigned char test_sig_freq; // see enum in defs
	// Config 3
	unsigned char bias_connected; // Read only
	unsigned char bias_sense_en;
	unsigned char bias_buf_en;
	unsigned char bias_sig_internel; // 0 for external
	unsigned char bias_in_meas; // see BIAS_MEAS
	unsigned char ref_buf_en; // Internal Reference buffer
	// Lead off
	unsigned char lead_off_freq; // see enum
	unsigned char lead_off_current; // see enum
	unsigned char lead_off_threshold; // [95, 92.5, 90, 87.5, 85, 80, 75, 70] for positive, [5, 7.5, 10, 12.5, 15, 20, 25, 30] for negative, percents.
	// Config 4
	unsigned char lead_off_comp_en;
	unsigned char single_shot_mode; // 0 for continious conversion mode
	// Misc
	unsigned char srb1_connect;

} Ads1299_GlobalConfig;

#define ADS1299_NUM_GAINS 7
static unsigned char ADS1299_GAINS[ADS1299_NUM_GAINS] = {1,2,4,6,8,12,24};

typedef struct _ads1299_chan_config
{
	unsigned char enable;
	unsigned char gain;		// ADS1299_GAINS values are allowed
	unsigned char srb2;		// Connection to SRB2
	unsigned char mux;		// See enum
} Ads1299_ChanConfig;

// typedef struct _ads1299_regdata
// {
// 	Ads1299_GlobalConfig glcfg;
// 	Ads1299_ChanConfig chcfg[8];
// 	unsigned char bias_senseP;
// 	unsigned char bias_senseN;
// } Ads1299_RegData;

// Write config to ADS
Ads1299_Reg ads1299_get_config1(Ads1299_GlobalConfig * cfg);
Ads1299_Reg ads1299_get_config2(Ads1299_GlobalConfig * cfg);
Ads1299_Reg ads1299_get_config3(Ads1299_GlobalConfig * cfg);
Ads1299_Reg ads1299_get_config4(Ads1299_GlobalConfig * cfg);
Ads1299_Reg ads1299_get_leadoff(Ads1299_GlobalConfig * cfg);
Ads1299_Reg ads1299_get_misc1(Ads1299_GlobalConfig * cfg);

void ads1299_glcfg_update(Ads1299_GlobalConfig * cfg, Ads1299_Reg * reg);

// Special funcitions
Ads1299_Reg ads1299_set_sample_rate(Ads1299_GlobalConfig * cfg, unsigned short sample_rate);
Ads1299_Reg ads1299_set_channel(unsigned char number, Ads1299_ChanConfig * cfg);


#endif //_ADS1299_CTL_H_
