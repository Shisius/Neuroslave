#ifndef _SERVER_W5500_H_
#define _SERVER_W5500_H_

#include "socket.h"
//#include "spi/spi_hal_tools.h"
#include "spi/spi_ll_easy_api.h"

#define W5500_SPI_CHANNEL SPI2
#define W5500_SPI_CS_PIN			LL_GPIO_PIN_12
#define W5500_SPI_CS_PORT			GPIOB
#define W5500_SPI_CS_CLK			LL_AHB1_GRP1_PERIPH_GPIOB

void W5500_WriteBuff(uint8_t* buff, uint16_t len);
void W5500_Select(void);
void W5500_Unselect(void);

typedef struct _server_w5500
{
	// SPI_HandleTypeDef * hspi;
} ServerW5500;

#endif //_SERVER_W5500_H_
