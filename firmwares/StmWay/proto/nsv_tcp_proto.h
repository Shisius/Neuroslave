#ifndef _NSV_TCP_PROTO_H_
#define _NSV_TCP_PROTO_H_

/**
 * magic - defines protocol version
 * title - request or answer code
 * arg - subtitle
 * size - size of data after the header
 */
typedef struct _nsv_tcp_msg_header
{
	unsigned char magic;
	unsigned char title;
	unsigned char arg;
	unsigned char size;
} NsvTcpMsgHeader;

#endif // _NSV_TCP_PROTO_H_
