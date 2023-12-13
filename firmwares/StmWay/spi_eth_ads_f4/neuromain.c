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

typedef enum {
    LED_STATE_NONE = 0,
    LED_STATE_FAIL = 1,
    LED_STATE_WIN = 2
} LedStates;

volatile int led_state = LED_STATE_NONE;

void SysTick_Handler(void)
{
    static int counter = 0;
    counter++;

    // 1 Hz blinking
    if ( ( (counter % 100) == 0) && (led_state == LED_STATE_FAIL) )
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);

    if ( ( (counter % 1000) == 0) && (led_state == LED_STATE_WIN) )
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

void fiasco()
{
    led_state = LED_STATE_FAIL;
}

ServerW5500 srv;

int main(void) 
{

    LL_GPIO_SetPinMode(LED_PORT, LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED_PORT, LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    int result = server_w5500_init(&srv);

    while (1) 
    {
        result = server_w5500_accept(&srv);
        if (result < 0) fiasco();
        else led_state = LED_STATE_WIN;
        uint8_t msg[4];
        result = recv(srv.sock, msg, 4);
        result = send(srv.sock, msg, 4);
        close(srv.sock);
    }


    return 0;
}
