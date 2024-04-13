
#include "server_w5500.h"

// unsigned char W5500_SpiXfer(unsigned char byte)
// {
// 	while(LL_SPI_IsActiveFlag_TXE(W5500_SPI_CHANNEL) == RESET) {}

// 	LL_SPI_TransmitData8(W5500_SPI_CHANNEL, byte);
	
// 	while(LL_SPI_IsActiveFlag_RXNE(W5500_SPI_CHANNEL) == RESET) {}

// 	return LL_SPI_ReceiveData8(W5500_SPI_CHANNEL);
// }

// void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
// 	while(LL_SPI_IsActiveFlag_TXE(W5500_SPI_CHANNEL)==RESET);
// 	uint16_t i_byte = 0;
// 	while (i_byte < len) {
// 		LL_SPI_TransmitData8(W5500_SPI_CHANNEL, buff[i_byte]);
// 		i_byte++;
// 	}
// }

// void W5500_ReadBuff(uint8_t* buff, uint16_t len)
// {
// 	uint16_t i_byte = 0;
// 	while (i_byte < len) {
// 		buff[i_byte] = W5500_ReadByte();
// 		i_byte++;
// 	}
// }

// void W5500_Select(void)
// {
// 	LL_GPIO_ResetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
// }

// void W5500_Unselect(void)
// {
// 	LL_GPIO_SetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
// }

// uint8_t W5500_ReadByte(void) {
//     uint8_t byte = 0;
//     byte = W5500_SpiXfer(byte);
//     return byte;
// }

// void W5500_WriteByte(uint8_t byte) {
//     W5500_SpiXfer(byte);
// }

int server_w5500_init(ServerW5500 * srv, void(*cs_select)(void), void(*cs_unselect)(void), 
										 uint8_t(*read_byte)(void), void(*write_byte)(uint8_t),
										 void(*delay_ms)(uint32_t))
{
	// register
	reg_wizchip_cs_cbfunc(cs_select, cs_unselect);
	reg_wizchip_spi_cbfunc(read_byte, write_byte);
	// reg_wizchip_spiburst_cbfunc(read_buf, write_buf);
	srv->_delay_ms = delay_ms;
	// Init
	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);
	// Addr
	uint8_t mask[4] = SERVER_IP_MASK;
	setSUBR(mask);
	uint8_t mac[6] = SERVER_MAC_ADDR;
	setSHAR(mac);
	uint8_t addr[4] = SERVER_IP_ADDR;
	setSIPR(addr);
	// Socket
	srv->sock = 1;
	// if (socket(srv->sock, Sn_MR_TCP, SERVER_W5500_TCP_PORT, SF_TCP_NODELAY|SF_IO_NONBLOCK) != srv->sock) return -1;
	return 0;
}

int server_w5500_run(ServerW5500 * srv)
{
	int ret;
	while (1)
	{
		switch (getSn_SR(srv->sock))
		{
			case SOCK_CLOSED:
				if ((ret = socket(srv->sock, Sn_MR_TCP, SERVER_W5500_TCP_PORT, SF_TCP_NODELAY|SF_IO_NONBLOCK)) != srv->sock) return ret;
				break;
			case SOCK_INIT:
				if ((ret = listen(srv->sock)) != SOCK_OK) return ret;
				break;
			case SOCK_CLOSE_WAIT:
				if ((ret = disconnect(srv->sock)) != SOCK_OK) return ret;
				break;
			case SOCK_ESTABLISHED:
				if (getSn_IR(srv->sock) & Sn_IR_CON) {
					setSn_IR(srv->sock, Sn_IR_CON);
				}
				//if ()
				break;
			default:
				break;
		}
	}
	return 0;
}
