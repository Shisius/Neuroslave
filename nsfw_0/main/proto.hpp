// UDP and TCP clients and servers
#ifndef __PROTO_H__
#define __PROTO_H__

#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "tcpip_adapter.h"

//#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

// UPD Client
class UDPClient
{
private:

    sockaddr_in * remote_addr;

    int self_sock;

public:

    UDPClient();

    int setup(const char * remote_ip, const uint16_t port);

    // send message to remote
    int sendmesssage(const char * text, size_t text_size);
    
    // recive message from remote
    // int recvmessage();

    ~UDPClient();

};


#endif