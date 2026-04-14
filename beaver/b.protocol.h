///@file: b.protocol.h
#pragma once

#include "b.packet.h"
#include "b.recv.h"
#include "b.send.h"

#define IPV4 AF_INET
#define IPV6 AF_INET6

typedef struct list
{
    packet_t packet;
    struct list *next;
}list;

int beaver_socket(int __domain){
    return socket(__domain, SOCK_DGRAM, 0);
}

// the send should be doing a send like of the total list 
void beaver_send_stream(int fd, struct sockaddr_in *dst, 
                        uint32_t seq, uint32_t ack, uint8_t flags, 
                        const byte *stream, uint16_t stream_length)
{
    header_t hdr = {0};
    hdr.seq = seq;
    hdr.ack = ack;
    hdr.flags = flags;
    hdr.len = stream_length;

    byte buf[MAX_PAYLOAD + HDR_SIZE];
    
}