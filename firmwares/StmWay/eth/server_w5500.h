#ifndef _SERVER_W5500_H_
#define _SERVER_W5500_H_

#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_bus.h>

#include "socket.h"
//#include "spi/spi_hal_tools.h"
//#include "spi/spi_ll_easy_api.h"

#define W5500_SPI_CHANNEL SPI2
#define W5500_SPI_CS_PIN			LL_GPIO_PIN_12
#define W5500_SPI_CS_PORT			GPIOB
#define W5500_SPI_CLK 				LL_APB1_GRP1_PERIPH_SPI2
#define W5500_SPI_CS_CLK			LL_AHB1_GRP1_PERIPH_GPIOB
#define W5500_SPI_GPIO				(LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15)
#define W5500_SPI_GPIO_PORT			GPIOB

#define SERVER_W5500_TCP_PORT		23932

unsigned char W5500_SpiXfer(unsigned char byte);
uint8_t W5500_ReadByte(void);
void W5500_WriteByte(uint8_t byte);
void W5500_WriteBuff(uint8_t* buff, uint16_t len);
void W5500_ReadBuff(uint8_t* buff, uint16_t len);
void W5500_Select(void);
void W5500_Unselect(void);

typedef struct _server_w5500
{
	//LL_SPI_InitTypeDef spi_config;
	//LL_GPIO_InitTypeDef gpio_config;
	uint8_t sock;
} ServerW5500;

int server_w5500_init(ServerW5500 * srv);
int server_w5500_accept(ServerW5500 * srv);

#endif //_SERVER_W5500_H_
