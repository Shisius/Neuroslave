import RPi.GPIO as gpio
import spidev
import struct
import time
import multiprocessing as mp

GANG_SPIS_MODE = 0
GANG_SPIS_HZ = 4000000

GANG_DRDY_PIN = 25

GANG_CMD_SAMPLE = 0x1
GANG_CMD_START = 0x2
GANG_SAMPLE_SIZE = 24
GANG_SAMPLE_RULE = 'I4iI'

class GanglionSpiComm:

    def __init__(self, spi_bus = 0, spi_device = 0, data_queue = None):
        # SPI
        self.spi = spidev.SpiDev(spi_bus, spi_device)
        self.spi.mode = GANG_SPIS_MODE
        self.spi.max_speed_hz = GANG_SPIS_HZ
        # GPIO
        gpio.setmode(gpio.BCM)
        gpio.setup(GANG_DRDY_PIN, gpio.IN)
        # Interaction
        self.queue = data_queue
        # State
        self.expected_index = 0
        self.is_running = True

    def __del__(self):
        self.spi.close()
        gpio.cleanup()
        print('GPIO cleaned up')

    def start(self):
        self.is_running = True
        gpio.add_event_detect(GANG_DRDY_PIN, gpio.RISING, callback = self.drdy_routine)
        self.spi.xfer([GANG_CMD_START])

    def stop(self):
        self.is_running = False
        gpio.remove_event_detect(GANG_DRDY_PIN)    

    def get_sample(self):
        self.spi.xfer([GANG_CMD_SAMPLE])
        b_data = self.spi.xfer([0x00]*GANG_SAMPLE_SIZE)
        return struct.unpack(GANG_SAMPLE_RULE, bytes(b_data))

    def drdy_routine(self, pin):
        b_data = self.spi.xfer([0x00]*GANG_SAMPLE_SIZE)
        sample_list = struct.unpack(GANG_SAMPLE_RULE, bytes(b_data))
        # check
        if sample_list[0] != self.expected_index:
            print("expected:", self.expected_index, " real: ", sample_list[0])
        self.expected_index = sample_list[0] + 1
        if sample_list[0] % 1000 == 0:
            print(sample_list)
        # store
        if self.queue != None:
            self.queue.put(sample_list)
        # Ask for next
        self.spi.xfer([GANG_CMD_SAMPLE])

    def print_thread(self):
        while self.is_running:
            print(self.queue.get())
            time.sleep(0.5)

        
if __name__ == "__main__":
    context = mp.get_context('spawn')
    data_q = context.Queue()
    gsc = GanglionSpiComm(data_queue = data_q)
