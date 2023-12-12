#if defined STM32F4
    //#define USE_FULL_LL_DRIVER
    #include <stm32f4xx_ll_gpio.h>
    #include <stm32f4xx_ll_bus.h>
    #include <stm32f4xx_ll_cortex.h>
    #include <stm32f4xx_ll_rcc.h>
    #include <stm32f4xx_ll_spi.h>
    
    // #include "stm32f4xx_hal.h"

    #define LED_PORT                GPIOC
    #define LED_PIN                 LL_GPIO_PIN_12
    #define LED_PORT_CLK_ENABLE()   { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; }
#endif

#include "eth/server_w5500.h"

ServerW5500 srv;

int main(void) 
{

    srv = server_w5500_create();

    int result = server_w5500_init(&srv);

    while (1) 
    {
        ;
    }


    return 0;
}
