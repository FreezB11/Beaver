#include "../beaver/b.protocol.h"

int main(void)
{
    beaver client;
 
    if (beaver_init(&client, IPV4) < 0)
        return 1;
 
    // Record who to send to (no bind needed on the sender side)
    if (client.establish(&client, IPV4, 8080, "127.0.0.1") < 0)
        return 1;
 
    byte msg[] = "hello from beaver client";
    if (client.send_stream(&client, msg, sizeof(msg)) < 0) {
        fprintf(stderr, "[sender] send_stream failed\n");
        return 1;
    }
 
    return 0;
}