#include "wifiobj.hpp"

WiFiStation::WiFiStation() {
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    this->wifi_init_config = new wifi_init_config_t(wifi_init_config);
    wifi_config_t empty_wifi_config = {
        .sta = {
            .ssid = {},
            .password = {}
        },
    };
    this->wifi_config = new wifi_config_t(empty_wifi_config);
}

void WiFiStation::wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    switch (event_id)
    {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            printf("disconnected!!!\n");
            esp_wifi_connect();
            break;
    } 
}

void WiFiStation::ip_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    switch (event_id)
    {
        case IP_EVENT_STA_GOT_IP:
            printf("GOT IP event\n");
            break;
    }
}

int WiFiStation::setup(const char ssid[], const char * password) {
    // tcpip init
    tcpip_adapter_init();
    // wifi init
    esp_event_loop_create_default();
    esp_wifi_init(wifi_init_config);
    // handler
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &(WiFiStation::wifi_event_handler), NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &(WiFiStation::ip_event_handler), NULL);
    // set mode
    esp_wifi_set_mode(WIFI_MODE_STA);
    // Set ssid and password
    memcpy(wifi_config->sta.ssid, ssid, sizeof(wifi_config->sta.ssid));
    memcpy(wifi_config->sta.password, password, sizeof(wifi_config->sta.password));
    // set config
    esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config);
    // start
    esp_wifi_start();
    return 0;
}

WiFiStation::~WiFiStation() {
    esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &(WiFiStation::wifi_event_handler));
    //esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &(WiFiStation::ip_event_handler), NULL);
}
