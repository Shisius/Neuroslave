
#include "server_w5500.h"

void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
	while(LL_SPI_IsActiveFlag_TXE(W5500_SPI_CHANNEL)==RESET);
	uint16_t i_byte = 0;
	while (i_byte < len) {
		LL_SPI_TransmitData8(W5500_SPI_CHANNEL, buff[i_byte]);
		i_byte++;
	}
    // HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

void W5500_Select(void)
{
	LL_GPIO_ResetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
}

void W5500_Unselect(void)
{
	LL_GPIO_SetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
}
