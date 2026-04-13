// pong.c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server, client;
    socklen_t len = sizeof(client);

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(fd, (struct sockaddr*)&server, sizeof(server));

    int seq;

    while (1) {
        int n = recvfrom(fd, &seq, sizeof(seq), 0,
                         (struct sockaddr*)&client, &len);

        if (n <= 0) continue;

        printf("Received ping: %d\n", seq);

        // send back (pong)
        sendto(fd, &seq, sizeof(seq), 0,
               (struct sockaddr*)&client, len);
    }
}