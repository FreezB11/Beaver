#include "../beaver/b.protocol.h"

int main(void)
{
    beaver server;
 
    if (beaver_init(&server, IPV4) < 0)
        return 1;
 
    // Bind to the local port so the OS delivers packets to us
    if (server.listen_on(&server, IPV4, 8080) < 0)
        return 1;
 
    byte msg[4096];   // big enough for whatever the sender throws at us
    int  received = server.recv_stream(&server, msg, sizeof(msg));
    if (received < 0) {
        fprintf(stderr, "[receiver] recv_stream failed\n");
        return 1;
    }
 
    // Only print the bytes we actually received, not garbage past them
    printf("[receiver] message (%d bytes): %.*s\n", received, received, msg);
 
    return 0;
}