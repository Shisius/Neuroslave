#ifndef _SPI_LL_EASY_API_H_
#define _SPI_LL_EASY_API_H_

#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_gpio.h>

void init_spi_ll(unsigned char spi_num, unsigned int max_speed_hz);

#endif //_SPI_LL_EASY_API_H_
