#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/sdspi_host.h"

#define spiCLK 18
#define spiMOSI 23
#define spiMISO 19
#define csPin 5

// static const char *TAG = "SPI Driver";
spi_device_handle_t spiHandle = NULL;
void app_main(void)
{ 

    // ========= SPI BUS initialising =============
    void SPI_Settings ()
    {
    spi_bus_config_t bus_config = {};
    bus_config.sclk_io_num = spiCLK;  // CLK
    bus_config.mosi_io_num = spiMOSI; // MOSI
    bus_config.miso_io_num = spiMISO; // MISO
    bus_config.quadwp_io_num = -1;    // Not used
    bus_config.quadhd_io_num = -1;    // Not used
    spi_bus_initialize(SPI3_HOST, &bus_config, 0);
    
    // ========================================
    spi_device_interface_config_t dev_config = {}; // initializes all field to 0
    dev_config.mode = 0;
    dev_config.clock_speed_hz = 1000000; // 163480;
    dev_config.spics_io_num = csPin;
    dev_config.queue_size = 1;
    spi_bus_add_device(SPI3_HOST, &dev_config, &spiHandle);
    }
    // ==================Reading======================
    float CommEnergyIC(int8_t regAddr, int8_t READ)
    {
        int8_t val[4] = {0};
        spi_transaction_t trans_desc = {};
        trans_desc.flags = SPI_TRANS_USE_TXDATA;
        trans_desc.length = (8 * 4); // total data bits
        trans_desc.tx_data[1] = regAddr;
        trans_desc.tx_data[0] = READ;
        trans_desc.rxlength = (8 * 4); // Number of bits NOT number of bytes
        trans_desc.rx_buffer = val;
        spi_device_transmit(spiHandle, &trans_desc);
        vTaskDelay(4 / portTICK_RATE_MS);
        float value = (float)((val[2] << 8) | val[3]) / 100;

        return value;
    }

        printf("Value of phase A :%f \n \r ", CommEnergyIC(0xD9, 0x80));
        vTaskDelay(100/ portTICK_RATE_MS);
        printf("Frequency : %f    \n \r ", CommEnergyIC(0xF8, 0x80));
        vTaskDelay(10 / portTICK_RATE_MS);
    
}