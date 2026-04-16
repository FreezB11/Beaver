///@file: b.protocol.h
#pragma once

#include <stdbool.h>

#include "b.packet.h"
#include "b.recv.h"
#include "b.send.h"

#define IPV4 AF_INET
#define IPV6 AF_INET6

typedef struct beaver{

    //properties;

}beaver;

int beaver_init(beaver* beav, int __domain){

}

int beaver_socket(int __domain){
    return socket(__domain, SOCK_DGRAM, 0);
}

int beaver_set_peer(int __domain, int __port, char* __ip){
    struct sockaddr_in _internal;
    socklen_t len = sizeof(_internal);

    _internal.sin_family = __domain;
    _internal.sin_port = htons(__port);
    _internal.sin_addr.s_addr = inet_addr(__ip);
}

// the send should be doing a send like of the total list 
void beaver_send_stream(int fd, struct sockaddr_in *dst,
                        const byte *stream, uint16_t stream_length)
{
    // we chunk the data here

}

void beaver_recv_stream(int fd, struct sockaddr_in *src,
                        byte *buffer, uint16_t buffer_length)
{

}