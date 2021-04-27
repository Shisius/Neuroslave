import RPi.GPIO as gpio
import spidev

# SPI
MCP_SPI_HZ = 4000000
MCP_SPI_MODE = 0b00
# GPIO
MCP_DRDY_PIN = 10
MCP_SS_PIN = 11
MCP_RST_PIN = 12
# CMDS
MCP_CMD_DEV_ADDR = 0x40
MCP_CMD_READ = 0x1
MCP_CMD_WRITE = 0x0
MCP_REG_N_BYTES = 3

MCP_CMD_CH_0 = 0x00
MCP_CMD_CH_1 = 0x02
MCP_CMD_CH_2 = 0x04
MCP_CMD_CH_3 = 0x06

MCP_CMD_GAIN = 0x16

class MCP3912:

    def __init__(self, spi_bus = 0, spi_device = 0):
        # SPI
        self.spi = spidev.SpiDev(spi_bus, spi_device)
        self.spi.mode = MCP_SPI_MODE
        self.spi.max_speed_hz = MCP_SPI_HZ
        # SS
        gpio.setup(MCP_SS_PIN, gpio.OUT)
        gpio.output(MCP_SS_PIN, 1)
        # DRDY
        
        
    def sendcmd(self, addr, rw):
        self.spi.xfer(MCP_CMD_DEV_ADDR | addr | rw)

    def read(self, n_bytes = MCP_REG_N_BYTES):
        data = 0
        for i in range(n_bytes):
            data |= self.spi.xfer(0x00)
            data <<= 8
        return data
        
