#include "b.packet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int beaver_recv(int fd, struct sockaddr_in *src,
                       header_t *hdr, byte *payload) {

    uint8_t buf[HDR_SIZE + MAX_PAYLOAD];
    socklen_t src_len = sizeof(*src);

    // 1. receive raw bytes
    ssize_t n = recvfrom(fd, buf, sizeof(buf), 0,
                         (struct sockaddr *)src, &src_len);
    if (n < 0) { perror("recvfrom"); return -1; }
    if (n < HDR_SIZE) {
        printf("[recv] packet too small (%zd bytes), dropping\n", n);
        return -1;
    }

    // 2. validate checksum BEFORE deserializing
    //    save checksum, zero that field, recompute, compare
    uint16_t received_cks;
    memcpy(&received_cks, buf + 11, 2);
    memset(buf + 11, 0, 2);                         // zero checksum field
    uint16_t computed_cks = CHECKSUM(buf, n);
    if (computed_cks != ntohs(received_cks)) {
        printf("[recv] checksum FAIL — dropping corrupted packet\n");
        return -1;
    }
    memcpy(buf + 11, &received_cks, 2);             // restore

    // 3. deserialize header
    deserialize(buf, hdr);

    // 4. copy payload out
    uint16_t plen = hdr->len;
    if (plen > 0 && payload)
        memcpy(payload, buf + HDR_SIZE, plen);

    printf("[recv] seq=%u ack=%u flags=0x%02x len=%u  ",
           hdr->seq, hdr->ack, hdr->flags, hdr->len);

    // 5. decode flags for debug
    printf("flags=[ ");
    if (hdr->flags & SYN) printf("SYN ");
    if (hdr->flags & ACK) printf("ACK ");
    if (hdr->flags & FIN) printf("FIN ");
    if (hdr->flags & RST) printf("RST ");
    printf("]\n");

    return 0;
}