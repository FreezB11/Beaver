#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "beaver/b.packet.h"
#include "beaver/b.send.h"
#include "beaver/b.recv.h"

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in dst = {0};
    dst.sin_family = AF_INET;
    dst.sin_port   = htons(9090);
    inet_pton(AF_INET, "127.0.0.1", &dst.sin_addr);
    const char *msg = "hello from beaver";
    uint32_t seq = 1000;   // would be random in T6
    beaver_send(fd, &dst, seq, 0, 0,
                       (const uint8_t *)msg, strlen(msg));
    close(fd);
}