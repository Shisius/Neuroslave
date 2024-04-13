#ifndef _SERVER_W5500_H_
#define _SERVER_W5500_H_

#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_spi.h>
#include <stm32f4xx_ll_bus.h>

#include "socket.h"
//#include "spi/spi_hal_tools.h"
//#include "spi/spi_ll_easy_api.h"


#define SERVER_W5500_TCP_PORT		23932
#define SERVER_IP_ADDR { 169, 254, 245, 239 }
#define SERVER_IP_MASK { 0xFF, 0xFF, 0x00, 0x00 }
#define SERVER_MAC_ADDR { 0xEF, 0x23, 0x92, 0x39, 0xAB, 0xCD }

unsigned char W5500_SpiXfer(unsigned char byte);
uint8_t W5500_ReadByte(void);
void W5500_WriteByte(uint8_t byte);
void W5500_WriteBuff(uint8_t* buff, uint16_t len);
void W5500_ReadBuff(uint8_t* buff, uint16_t len);
void W5500_Select(void);
void W5500_Unselect(void);

typedef struct ServerW5500 ServerW5500;
struct ServerW5500
{
	uint8_t sock;

	void (*_delay_ms)(uint32_t delay_ms);

	
};

int server_w5500_init(ServerW5500 * srv, void(*cs_select)(void), void(*cs_unselect)(void), 
										 uint8_t(*read_byte)(void), void(*write_byte)(uint8_t),
										 void(*delay_ms)(uint32_t));
int server_w5500_run(ServerW5500 * srv);

#endif //_SERVER_W5500_H_
