#ifndef _ADS1299_DEFS_H_
#define _ADS1299_DEFS_H_

#define ADS1299_FCLK_DEFAULT 2048000UL
#define ADS1299_SETUP_DELAY_US 150000UL
#define ADS1299_CS_DELAY_US 5
#define ADS1299_SPI_FREQ_MAX 20000000UL

typedef enum
{
	ADS1299_WAKEUP 		= 0x02,
	ADS1299_STANDBY 	= 0x04,
	ADS1299_RESET 		= 0x06,
	ADS1299_START 		= 0x08,
	ADS1299_STOP 		= 0x0A,

	ADS1299_RDATAC 		= 0x10, // Enable continuous mode
	ADS1299_SDATAC 		= 0x11,	// Stop continuous mode
	ADS1299_RDATAC		= 0x12, // Read data by command

	ADS1299_RREG_MASK	= 0x3F, // Read register. 5 bits for addr
	ADS1299_WREG_MASK	= 0x5F  // Write register. 5 bits for addr
} Ads1299_Cmds;

typedef enum
{
	ADS1299_REG_ID 			= 0x00,
	ADS1299_REG_CONFIG1		= 0x01,
	ADS1299_REG_CONFIG2		= 0x02,
	ADS1299_REG_CONFIG3		= 0x03,
	ADS1299_REG_LOFF		= 0x04,

	ADS1299_REG_CH1SET		= 0x05,
	ADS1299_REG_CH2SET		= 0x06,
	ADS1299_REG_CH3SET		= 0x07,
	ADS1299_REG_CH4SET		= 0x08,
	ADS1299_REG_CH5SET		= 0x09,
	ADS1299_REG_CH6SET		= 0x0A,
	ADS1299_REG_CH7SET		= 0x0B,
	ADS1299_REG_CH8SET		= 0x0C,

	ADS1299_REG_BIAS_SENSP	= 0x0D,
	ADS1299_REG_BIAS_SENSN	= 0x0E,
	ADS1299_REG_LOFF_SENSP	= 0x0F,
	ADS1299_REG_LOFF_SENSN	= 0x10,
	ADS1299_REG_LOFF_FLIP	= 0x11,

	ADS1299_REG_LOFF_STATP	= 0x12,
	ADS1299_REG_LOFF_STATN	= 0x13,

	ADS1299_REG_GPIO		= 0x14,
	ADS1299_REG_MISC1		= 0x15,
	ADS1299_REG_MISC2		= 0x16,
	ADS1299_REG_CONFIG4		= 0x17
} Ads1299_RegAddr;

// ID
#define ADS1299_DEV_ID 0x03
typedef enum 
{
	ADS1299_NUM_CH_MASK = 0x03,
	ADS1299_DEV_ID_MASK = 0x0C,
	ADS1299_REV_ID_MASK = 0xE0
} Ads1299_ID_Masks;

// Config 1
typedef enum 
{
	ADS1299_DATA_RATE_MASK			= 0x07,
	ADS1299_EN_CLK_OUTPUT_MASK		= 0x20,
	ADS1299_DIS_DAISY_MODE_MASK 	= 0x40 // 0 - Daisy Mode (Multiple devices), 1 - Multiple readback
} Ads1299_Config1_Masks;

// Config 2
typedef enum
{
	ADS1299_TEST_FREQ_21 = 0, // Freq = fCLK/2^21
	ADS1299_TEST_FREQ_20 = 1,
	ADS1299_TEST_FREQ_NO = 2,
	ADS1299_TEST_FREQ_DC = 3
} Ads1299_TestSigFreq;
typedef enum
{
	ADS1299_TEST_FREQ_MASK 		= 0x03,
	ADS1299_TEST_AMP_MASK		= 0x04,
	ADS1299_TEST_SOURCE_MASK	= 0x10
} Ads1299_Config2_Masks;

// Config 3
typedef enum
{
	ADS1299_BIAS_STAT_MASK 			= 0x01,
	ADS1299_BIAS_LOFF_SENSE_MASK	= 0x02,
	ADS1299_BIAS_BUF_EN_MASK		= 0x04,
	ADS1299_BIAS_REF_INT_MASK		= 0x08,
	ADS1299_BIAS_MEAS_MASK 			= 0x10,
	ADS1299_REF_BUF_EN_MASK			= 0x80	
} Ads1299_Config3_Masks;

// Lead off
typedef enum
{
	ADS1299_LOFF_FREQ_DC	= 0x00,
	ADS1299_LOFF_FREQ_7_8	= 0x01, // AC freq 7.8 Hz = fCLK/2^18
	ADS1299_LOFF_FREQ_31_2	= 0x02, // AC freq 31.2 Hz = fCLK/2^16
	ADS1299_LOFF_FREQ_DR_4	= 0x03  // AC freq fDR/4
} Ads1299_LeadOffFreq;
typedef enum
{
	ADS1299_LOFF_CUR_6nA	= 0x00,
	ADS1299_LOFF_CUR_24nA	= 0x01,
	ADS1299_LOFF_CUR_6uA	= 0x02,
	ADS1299_LOFF_CUR_24uA	= 0x03
} Ads1299_LeadOffCurrent;
typedef enum
{
	ADS1299_FLEAD_OFF_MASK 			= 0x03,
	ADS1299_ILEAD_OFF_MASK			= 0x0C,
	ADS1299_LEAD_OFF_COMP_TH_MASK	= 0xC0
} Ads1299_LOFF_Masks;

// Channel settings
typedef enum
{
	ADS1299_CHAN_MUX_MORM		= 0x00,
	ADS1299_CHAN_MUX_SHORT		= 0x01,
	ADS1299_CHAN_MUX_BIAS_MEAS	= 0x02,
	ADS1299_CHAN_MUX_MVDD		= 0x03,
	ADS1299_CHAN_MUX_TEMP		= 0x04,
	ADS1299_CHAN_MUX_TEST		= 0x05,
	ADS1299_CHAN_MUX_BIAS_DRP	= 0x06,
	ADS1299_CHAN_MUX_BIAS_DRN	= 0x07
} Ads1299_ChanInput;
typedef enum
{
	ADS1299_CHAN_MUX_MASK		= 0x07,
	ADS1299_CHAN_SRB2_MASK		= 0x08,
	ADS1299_CHAN_GAIN_MASK		= 0x70,
	ADS1299_CHAN_EN_MASK		= 0x80
} Ads1299_ChanSet_Masks;

typedef struct _ads1299_regmap
{
	unsigned char id;

	unsigned char
} Ads1299_RegMap;

#endif //_ADS1299_DEFS_H_
