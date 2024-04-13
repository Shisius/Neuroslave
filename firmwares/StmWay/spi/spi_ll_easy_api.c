
#include "spi_ll_easy_api.h"

void ns_init_spi2_ll(void)
{
	LL_SPI_InitTypeDef spi_config = {0};
	LL_APB1_GRP1_EnableClock(NS_SPI2_CLK);	
	LL_AHB1_GRP1_EnableClock(NS_SPI2_CS_CLK);
	// Init SPI GPIO
	LL_GPIO_InitTypeDef gpio_config = {0};
	gpio_config.Pin = NS_SPI2_GPIO;
	gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_config.Pull = LL_GPIO_PULL_NO;
	gpio_config.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(NS_SPI2_GPIO_PORT, &gpio_config);
	gpio_config.Pin = NS_SPI2_CS_PIN;
	gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
	gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_config.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(NS_SPI2_CS_PORT, &gpio_config);
	// Init SPI
	spi_config.TransferDirection = LL_SPI_FULL_DUPLEX;
	spi_config.Mode = LL_SPI_MODE_MASTER;
	spi_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	spi_config.ClockPolarity = LL_SPI_POLARITY_HIGH;
	spi_config.ClockPhase = LL_SPI_PHASE_2EDGE;
	spi_config.NSS = LL_SPI_NSS_SOFT;
	spi_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	spi_config.BitOrder = LL_SPI_MSB_FIRST;
	spi_config.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	spi_config.CRCPoly = 10;
	LL_SPI_Init(SPI2, &(spi_config));
	LL_SPI_SetStandard(SPI2, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_Enable(SPI2);
}

uint8_t ns_xfer_spi2_ll(uint8_t byte)
{
	while(LL_SPI_IsActiveFlag_TXE(SPI2) == RESET) {}

	LL_SPI_TransmitData8(SPI2, byte);
	
	while(LL_SPI_IsActiveFlag_RXNE(SPI2) == RESET) {}

	return LL_SPI_ReceiveData8(SPI2);
}

void ns_select_spi2_ll(void)
{
	LL_GPIO_ResetOutputPin(NS_SPI2_CS_PORT, NS_SPI2_CS_PIN);
}

void ns_unselect_spi2_ll(void)
{
	LL_GPIO_SetOutputPin(NS_SPI2_CS_PORT, NS_SPI2_CS_PIN);
}

uint8_t ns_readbyte_spi2_ll(void) {
	// while(LL_SPI_IsActiveFlag_RXNE(SPI2) == RESET) {}

	// return LL_SPI_ReceiveData8(SPI2);
    return ns_xfer_spi2_ll(0);
}

void ns_writebyte_spi2_ll(uint8_t byte) {
	// while(LL_SPI_IsActiveFlag_TXE(SPI2) == RESET) {}

	// LL_SPI_TransmitData8(SPI2, byte);
    ns_xfer_spi2_ll(byte);
}

void ns_writebuf_spi2_ll(uint8_t* buff, uint16_t len) {
	while(LL_SPI_IsActiveFlag_TXE(SPI2)==RESET);
	uint16_t i_byte = 0;
	while (i_byte < len) {
		LL_SPI_TransmitData8(SPI2, buff[i_byte]);
		i_byte++;
	}
}

void ns_readbuf_spi2_ll(uint8_t* buff, uint16_t len)
{
	while(LL_SPI_IsActiveFlag_RXNE(SPI2) == RESET);
	uint16_t i_byte = 0;
	while (i_byte < len) {
		buff[i_byte] = LL_SPI_ReceiveData8(SPI2);
		i_byte++;
	}
}
