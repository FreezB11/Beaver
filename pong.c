#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server, client;

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(fd, (struct sockaddr*)&server, sizeof(server));

    char buff[1024];
    socklen_t len = sizeof(client);

    int n = recvfrom(fd, buff, sizeof(buff), 0,
                     (struct sockaddr*)&client, &len);

    buff[n] = '\0';

    printf("Received: %s\n", buff);
}

///@note we can use accept and listen that is TCP protocol