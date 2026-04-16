#include "beaver/b.protocol.h"

int main(){

    // int fd = beaver_socket(IPV4); // obvio this udp so yea

    beaver cli;
    beaver_init(cli, IPV4);
    cli.establish(IPV4, 8080, "8.8.8.8");

    byte data[1024];
    cli.send_stream(&data, sizeof(data))
    
    return 0;
}