#if defined STM32F4
    #include <stm32f4xx_ll_gpio.h>
    #include <stm32f4xx_ll_cortex.h>
    #include <stm32f4xx_ll_rcc.h>
    // #include "stm32f4xx_hal.h"

    #define LED_PORT                GPIOC
    #define LED_PIN                 LL_GPIO_PIN_12
    #define LED_PORT_CLK_ENABLE()   { RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; }
#endif

#include "eth/server_w5500.h"

int main(void) 
{

    //HAL_Init();

    while (1) 
    {
        ;
    }


    return 0;
}
