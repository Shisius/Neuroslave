
/// UDP client
// sockaddr dist_addr
// socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)
// send, recvfrom

/// TCP server
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "esp_event.h"
#include "nvs_flash.h"

#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM 10
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 32
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE 1

class WiFiStation
{
private:

	wifi_init_config_t * wifi_init_config;
	wifi_config_t * wifi_config;

public:

	WiFiStation(); 

	static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

	int setup();

	~WiFiStation();

};

//WiFiStation * hep() {WiFiStation * wifistation = new WiFiStation(); return wifistation;}

// void app_main()
// {
//     //Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//       ESP_ERROR_CHECK(nvs_flash_erase());
//       ret = nvs_flash_init();
//     }
//     ESP_ERROR_CHECK(ret);
//     //wifi_init_softap();
// }
