#include "beaver/b.protocol.h"

int main(void)
{
    beaver cli;
 
    if (beaver_init(&cli, IPV4) < 0)
        return 1;
 
    // function pointers need the struct passed explicitly — C has no 'this'
    if (cli.establish(&cli, IPV4, 8080, "127.0.0.1") < 0)
        return 1;
 
    // --- single packet ---
    byte msg[] = "hello beaver";
    cli.send_stream(&cli, msg, sizeof(msg));
 
    // --- full 1 KB stream (will be split into 1 packet since MAX_PAYLOAD=1024) ---
    byte data[1024];
    memset(data, 0xAB, sizeof(data));
    cli.send_stream(&cli, data, sizeof(data));
 
    // --- 3 KB stream → 3 packets ---
    byte big[3 * 1024];
    memset(big, 0xCD, sizeof(big));
    cli.send_stream(&cli, big, sizeof(big));
 
    return 0;
}