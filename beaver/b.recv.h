/// @file: b.recv.h
#pragma once

#include "b.packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

static int beaver_recv(int fd, struct sockaddr_in *src,
                       header_t *hdr, byte *payload)
{
    uint8_t   buf[HDR_SIZE + MAX_PAYLOAD];
    socklen_t src_len = sizeof(*src);

    // 1. receive raw bytes
    ssize_t n = recvfrom(fd, buf, sizeof(buf), 0,
                         (struct sockaddr *)src, &src_len);
    if (n < 0)        { perror("recvfrom"); return -1; }
    if (n < HDR_SIZE) {
        printf("[recv] packet too small (%zd bytes), dropping\n", n);
        return -1;
    }

    // 2. validate checksum BEFORE deserialising
    //    - read the on-wire checksum (network byte order)
    //    - zero that field in the buffer
    //    - recompute over the zeroed buffer
    //    - compare both in host byte order
    uint16_t wire_cks_net;
    memcpy(&wire_cks_net, buf + 11, 2);
    uint16_t wire_cks = ntohs(wire_cks_net);   // host byte order

    memset(buf + 11, 0, 2);                    // zero checksum field
    uint16_t computed_cks = CHECKSUM(buf, (size_t)n);
    memcpy(buf + 11, &wire_cks_net, 2);        // restore original bytes

    if (computed_cks != wire_cks) {
        printf("[recv] checksum FAIL (got 0x%04x, expected 0x%04x) — dropping\n",
               computed_cks, wire_cks);
        return -1;
    }

    // 3. deserialise header
    deserialize(buf, hdr);

    // 4. copy payload out
    if (hdr->len > 0 && payload)
        memcpy(payload, buf + HDR_SIZE, hdr->len);

    // 5. debug print
    printf("[recv] seq=%u ack=%u flags=0x%02x len=%u  flags=[ ",
           hdr->seq, hdr->ack, hdr->flags, hdr->len);
    if (hdr->flags & SYN) printf("SYN ");
    if (hdr->flags & ACK) printf("ACK ");
    if (hdr->flags & FIN) printf("FIN ");
    if (hdr->flags & RST) printf("RST ");
    printf("]\n");

    return 0;
}