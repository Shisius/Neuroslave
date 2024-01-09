#if defined STM32F4
    //#define USE_FULL_LL_DRIVER
    #include <stm32f4xx_ll_gpio.h>
    #include <stm32f4xx_ll_bus.h>
    #include <stm32f4xx_ll_cortex.h>
    #include <stm32f4xx_ll_rcc.h>
    #include <stm32f4xx_ll_spi.h>
    #include <stm32f4xx_ll_utils.h>
    
    // #include "stm32f4xx_hal.h"

    #define LED_PORT                GPIOC
    #define LED_PIN                 LL_GPIO_PIN_13
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

    if ( ( (counter % 500) == 0) && (led_state == LED_STATE_WIN) )
        LL_GPIO_TogglePin(LED_PORT, LED_PIN);
}

void fiasco()
{
    led_state = LED_STATE_FAIL;
}

ServerW5500 srv;

int main(void) 
{

    LED_PORT_CLK_ENABLE();
    LL_Init1msTick(84000000UL);

    LL_GPIO_SetPinMode(LED_PORT, LED_PIN, LL_GPIO_MODE_OUTPUT);
    LL_GPIO_SetPinOutputType(LED_PORT, LED_PIN, LL_GPIO_OUTPUT_PUSHPULL);

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    int result = server_w5500_init(&srv);

    LL_mDelay(500);
    LL_GPIO_TogglePin(LED_PORT, LED_PIN);
    LL_mDelay(500);
    LL_GPIO_TogglePin(LED_PORT, LED_PIN);

    if (socket(srv.sock, Sn_MR_TCP, SERVER_W5500_TCP_PORT, SF_TCP_NODELAY|SF_IO_NONBLOCK) != srv.sock) result = -1;
    // LL_mDelay(500);
    // LL_GPIO_TogglePin(LED_PORT, LED_PIN);
    // LL_mDelay(500);
    // LL_GPIO_TogglePin(LED_PORT, LED_PIN);
    if (result < 0) fiasco();
    else led_state = LED_STATE_WIN;

    LL_mDelay(100);
    fiasco();

    result = listen(srv.sock);

    while (1) 
    {
        //LL_mDelay(100);
        // LL_GPIO_TogglePin(LED_PORT, LED_PIN);
        // LL_mDelay(1000);
        // LL_GPIO_TogglePin(LED_PORT, LED_PIN);
        //if (socket(srv.sock, Sn_MR_TCP, SERVER_W5500_TCP_PORT, 0/*SF_TCP_NODELAY|SF_IO_NONBLOCK*/) == srv.sock)
            
        //if (result < 0) fiasco();
        //else led_state = LED_STATE_WIN;
        uint8_t msg[4] = {'h', 'e', 'p', '\n'};
        if (getSn_SR(srv.sock) == SOCK_ESTABLISHED) {
            led_state = LED_STATE_WIN;
            //while (result >= 0) {
                result = recv(srv.sock, msg, 4);
                if (result >= 4) {
                    //do {
                        result = send(srv.sock, msg, 4);
                        //LL_mDelay(100);
                        //fiasco();
                    //} while (result <= 0);
                } else {
                    //fiasco();
                }
            //}
        } else {
            result = listen(srv.sock);
            fiasco();
        }
        //close(srv.sock);
        
    }


    return 0;
}
