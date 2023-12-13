
#include "server_w5500.h"

unsigned char W5500_SpiXfer(unsigned char byte)
{
	while(LL_SPI_IsActiveFlag_TXE(W5500_SPI_CHANNEL) == RESET) {}

	LL_SPI_TransmitData8(W5500_SPI_CHANNEL, byte);
	
	while(LL_SPI_IsActiveFlag_RXNE(W5500_SPI_CHANNEL) == RESET) {}

	return LL_SPI_ReceiveData8(W5500_SPI_CHANNEL);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
	while(LL_SPI_IsActiveFlag_TXE(W5500_SPI_CHANNEL)==RESET);
	uint16_t i_byte = 0;
	while (i_byte < len) {
		LL_SPI_TransmitData8(W5500_SPI_CHANNEL, buff[i_byte]);
		i_byte++;
	}
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len)
{
	uint16_t i_byte = 0;
	while (i_byte < len) {
		buff[i_byte] = W5500_ReadByte();
		i_byte++;
	}
}

void W5500_Select(void)
{
	LL_GPIO_ResetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
}

void W5500_Unselect(void)
{
	LL_GPIO_SetOutputPin(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);
}

uint8_t W5500_ReadByte(void) {
    uint8_t byte = 0;
    byte = W5500_SpiXfer(byte);
    return byte;
}

void W5500_WriteByte(uint8_t byte) {
    W5500_SpiXfer(byte);
}

int server_w5500_init(ServerW5500 * srv)
{
	LL_SPI_InitTypeDef spi_config = {0};
	LL_APB1_GRP1_EnableClock(W5500_SPI_CLK);	
	LL_AHB1_GRP1_EnableClock(W5500_SPI_CS_CLK);
	// Init SPI GPIO
	LL_GPIO_InitTypeDef gpio_config = {0};
	gpio_config.Pin = W5500_SPI_GPIO;
	gpio_config.Mode = LL_GPIO_MODE_ALTERNATE;
	gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_config.Pull = LL_GPIO_PULL_NO;
	gpio_config.Alternate = LL_GPIO_AF_5;
	LL_GPIO_Init(W5500_SPI_GPIO_PORT, &gpio_config);
	gpio_config.Pin = W5500_SPI_CS_PIN;
	gpio_config.Mode = LL_GPIO_MODE_OUTPUT;
	gpio_config.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	gpio_config.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpio_config.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(W5500_SPI_CS_PORT, &gpio_config);
	// Init SPI
	spi_config.TransferDirection = LL_SPI_FULL_DUPLEX;
	spi_config.Mode = LL_SPI_MODE_MASTER;
	spi_config.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	spi_config.ClockPolarity = LL_SPI_POLARITY_HIGH;
	spi_config.ClockPhase = LL_SPI_PHASE_2EDGE;
	spi_config.NSS = LL_SPI_NSS_SOFT;
	spi_config.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2;
	spi_config.BitOrder = LL_SPI_MSB_FIRST;
	spi_config.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	spi_config.CRCPoly = 10;
	LL_SPI_Init(W5500_SPI_CHANNEL, &(spi_config));
	LL_SPI_SetStandard(W5500_SPI_CHANNEL, LL_SPI_PROTOCOL_MOTOROLA);
	LL_SPI_Enable(W5500_SPI_CHANNEL);
	// register
	reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
	reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
	reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);
	// Init
	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);
	// Addr
	uint8_t mask[4] = { 0xFF, 0xFF, 0xFF, 0x00 };
	setSUBR(mask);
	uint8_t mac[6] = { 0xEF, 0x23, 0x92, 0x39, 0xAB, 0xCD };
	setSHAR(mac);
	uint8_t addr[4] = { 192, 168, 0, 239 };
	setSIPR(addr);
	// Socket
	srv->sock = 1;
	if (socket(srv->sock, IPPROTO_TCP, SERVER_W5500_TCP_PORT, 0) != srv->sock) return -1;
	return 0;
}

int server_w5500_accept(ServerW5500 * srv)
{
	if (listen(srv->sock) == SOCK_OK) return 0;
	else return -1;
}
