#ifndef _SPI_LL_EASY_API_H_
#define _SPI_LL_EASY_API_H_

//#define SPI1
//#define SPI2
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_bus.h>

#define NS_SPI2_CS_PIN			LL_GPIO_PIN_12
#define NS_SPI2_CS_PORT			GPIOB
#define NS_SPI2_CLK 			LL_APB1_GRP1_PERIPH_SPI2
#define NS_SPI2_CS_CLK			LL_AHB1_GRP1_PERIPH_GPIOB
#define NS_SPI2_GPIO				(LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15)
#define NS_SPI2_GPIO_PORT			GPIOB

void ns_init_spi2_ll(void);

uint8_t ns_xfer_spi2_ll(uint8_t byte);
void ns_select_spi2_ll(void);
void ns_unselect_spi2_ll(void);
uint8_t ns_readbyte_spi2_ll(void);
void ns_writebyte_spi2_ll(uint8_t byte);
void ns_writebuf_spi2_ll(uint8_t* buff, uint16_t len);
void ns_readbuf_spi2_ll(uint8_t* buff, uint16_t len);

#endif //_SPI_LL_EASY_API_H_
