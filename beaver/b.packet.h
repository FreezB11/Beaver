///@file:b.packet.h
#pragma once
#include <stdint.h>
#include <stddef.h>

#include "misc/util.h"

#define MAX_PAYLOAD 1024
#define HDR_SIZE 13

#define SYN  (1 << 0)   // 00000001  open connection
#define ACK  (1 << 1)   // 00000010  this packet has a valid ack field
#define FIN  (1 << 2)   // 00000100  close connection
#define RST  (1 << 3)   // 00001000  hard 1 / error

typedef struct header{
    // header
    u32 seq; // this is the byte offset of where the chunk starts in the full stream
    u32 ack;
    uint8_t flags;
    u16 len;
    u16 checksum;
    //4+4+1+2+2 = 13;
}header_t;

/**
    well the header will be 20bytes
    then we have 1024bytes payload
    which makes it 1044bytes of total packet
*/
typedef struct packet{
    header_t header;
    byte    payload[MAX_PAYLOAD];
}packet_t;

uint16_t CHECKSUM(const byte *payload, size_t len){
    uint32_t sum = 0;

    while(len > 1){
        uint16_t w;
        memcpy(&w, payload, 2);
        sum += w;
        payload += 2;
        len -= 2;
    }
    if(len == 1) sum += *payload;

    while(sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return (uint16_t)(~sum);
}


void serialize(const header_t *hdr, byte *buf){
    uint32_t seq = htonl(hdr->seq);
    uint32_t ack = htonl(hdr->ack);
    uint16_t len = htons(hdr->len);
    uint16_t cks = htons(hdr->checksum);

    memcpy(buf + 0, &seq, 4);
    memcpy(buf + 4, &ack, 4);
    buf[8] = hdr->flags;
    memcpy(buf + 9,  &len, 2);
    memcpy(buf + 11, &cks, 2);
}
void deserialize(const byte *buf, header_t *hdr){
    uint32_t seq, ack;
    uint16_t len, cks;

    memcpy(&seq, buf + 0,  4);
    memcpy(&ack, buf + 4,  4);
    hdr->flags = buf[8];
    memcpy(&len, buf + 9,  2);
    memcpy(&cks, buf + 11, 2);

    hdr->seq        = ntohl(seq);
    hdr->ack        = ntohl(ack);
    hdr->len        = ntohs(len);
    hdr->checksum   = ntohs(cks);
}   