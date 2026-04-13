// ping.c
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

int main() {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server;
    socklen_t len = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_port = htons(9090);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // timeout (important for loss detection)
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    int sent = 0, received = 0;

    for (int i = 0; i < 1000000; i++) {
        int seq = i;
        sent++;

        sendto(fd, &seq, sizeof(seq), 0,
               (struct sockaddr*)&server, len);

        int reply;
        int n = recvfrom(fd, &reply, sizeof(reply), 0, NULL, NULL);

        if (n > 0) {
            received++;
            // printf("Pong: %d\n", reply);
        } else {
            // printf("Lost: %d\n", seq);
        }

        // usleep(100); // .1ms gap (important)
    }

    printf("\n--- Stats ---\n");
    printf("Sent: %d\n", sent);
    printf("Received: %d\n", received);
    printf("Lost: %d\n", sent - received);
    printf("Loss %%: %.2f%%\n",
           ((sent - received) * 100.0) / sent);
}