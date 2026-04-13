#include "b.packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int beaver_send(int fd, struct sockaddr_in *dst, uint32_t seq, uint32_t ack, uint8_t flags, const byte *payload, uint16_t plen){
    header_t hdr = {0};
    hdr.seq = seq;
    hdr.ack = ack;
    hdr.flags = flags;
    hdr.len = plen;

    byte buf[MAX_PAYLOAD + HDR_SIZE];
    serialize(&hdr, buf);

    if(payload && plen > 0){
        memcpy(buf + HDR_SIZE, payload, plen);
    }

    uint16_t cks = CHECKSUM(buf, HDR_SIZE + plen);
    uint16_t cks_net = htons(cks);
    memcpy(buf + 11, &cks_net, 2);

    ssize_t sent = sendto(fd, buf, HDR_SIZE+plen, 0, (struct sockaddr *)dst, sizeof(*dst));
    if(sent < 0){perror("sendto"); return -1;}

    printf("[send] seq=%u ack=%u flags=0x%02x len=%u\n",
           seq, ack, flags, plen);
    return 0;
}