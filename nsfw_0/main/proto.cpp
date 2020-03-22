// UDP and TCP clients and servers
#include "proto.hpp"

UDPClient::UDPClient()
{
    // Remote address initialization
    struct sockaddr_in empty_addr = {};
    remote_addr = new sockaddr_in(empty_addr);
    remote_addr->sin_family = AF_INET;
}

int UDPClient::setup(const char * remote_ip, const uint16_t port)
{
    remote_addr->sin_addr.s_addr = inet_addr(remote_ip);
    remote_addr->sin_port = htons(port);

    // Create socket
    self_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (self_sock < 0) printf("Socket failed!!!\n");

    return 0;
}

int UDPClient::sendmesssage(const char * some_text, size_t text_size)
{
    return sendto(self_sock, some_text, text_size, 0, (struct sockaddr *)remote_addr, sizeof(*remote_addr));
}

UDPClient::~UDPClient()
{
    shutdown(self_sock, 0);
    close(self_sock);
}