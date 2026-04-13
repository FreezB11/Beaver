#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "beaver/b.packet.h"
#include "beaver/b.send.h"
#include "beaver/b.recv.h"

int main() {
        // RECEIVER MODE
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in local = {0};
    local.sin_family      = AF_INET;
    local.sin_port        = htons(9090);
    local.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr *)&local, sizeof(local));
    printf("[receiver] waiting on port 9090...\n");

    while (1) {
        header_t hdr;
        uint8_t payload[MAX_PAYLOAD];
        struct sockaddr_in src;
        if (beaver_recv(fd, &src, &hdr, payload) == 0) {
            if (hdr.len > 0) {
                payload[hdr.len] = '\0';
                printf("[receiver] payload: \"%s\"\n\n", payload);
            }
        }
    }
    close(fd);
}