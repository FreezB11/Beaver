///@file: b.protocol.h
#pragma once
 
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
 
#include "b.packet.h"
#include "b.recv.h"
#include "b.send.h"
 
typedef enum {
    IPV4 = 4,
    IPV6 = 6,
} ip_version_t;
 
typedef enum {
    statefull,
    stateless
} conn_state_t;
 
// forward declare
typedef struct beaver beaver;
 
struct beaver {
    // --- internal state (__ prefix = don't touch directly) ---
    ip_version_t       __version;
    conn_state_t       __state;
    int                __fd;
    struct sockaddr_in __peer;
 
    // --- API surface ---
    // sender: set peer address (who to send to)
    int (*establish)   (beaver *self, ip_version_t ver, int port, const char *ip);
    // receiver: bind to a local port (who to listen on)
    int (*listen_on)   (beaver *self, ip_version_t ver, int port);
    // data transfer
    int (*send_stream) (beaver *self, const byte *data, size_t len);
    int (*recv_stream) (beaver *self, byte *buf,        size_t buf_len);
};
 
// ─── implementation ────────────────────────────────────────────────────────
 
// Used by the SENDER — just records where to send packets.
static int _beaver_establish(beaver *self, ip_version_t ver, int port, const char *ip)
{
    self->__peer.sin_family = (ver == IPV4) ? AF_INET : AF_INET6;
    self->__peer.sin_port   = htons((uint16_t)port);
 
    if (inet_pton(AF_INET, ip, &self->__peer.sin_addr) <= 0) {
        perror("inet_pton");
        return -1;
    }
 
    self->__state = statefull;
    printf("[beaver] peer set → %s:%d\n", ip, port);
    return 0;
}
 
// Used by the RECEIVER — binds the socket so the OS delivers packets to us.
static int _beaver_listen_on(beaver *self, ip_version_t ver, int port)
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family      = (ver == IPV4) ? AF_INET : AF_INET6;
    local.sin_port        = htons((uint16_t)port);
    local.sin_addr.s_addr = INADDR_ANY;
 
    if (bind(self->__fd, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("bind");
        return -1;
    }
 
    self->__state = statefull;
    printf("[beaver] listening on port %d\n", port);
    return 0;
}
 
// Chunks a byte stream into MAX_PAYLOAD-sized packets and sends them.
// seq = byte offset of each chunk in the full stream.
static int _beaver_send_stream(beaver *self, const byte *data, size_t len)
{
    uint32_t seq     = 0;
    size_t   offset  = 0;
    unsigned packets = 0;
 
    while (offset < len) {
        size_t   remaining = len - offset;
        uint16_t chunk     = (uint16_t)(remaining > MAX_PAYLOAD ? MAX_PAYLOAD : remaining);
 
        if (beaver_send(self->__fd, &self->__peer,
                        seq, 0, 0x00,
                        data + offset, chunk) < 0)
            return -1;
 
        seq    += chunk;
        offset += chunk;
        packets++;
    }
 
    printf("[beaver] sent %zu byte(s) across %u packet(s)\n", len, packets);
    return 0;
}
 
// Receives packets until a chunk smaller than MAX_PAYLOAD arrives
// (signals the final chunk of the stream).
// Returns total bytes received, or -1 on error.
static int _beaver_recv_stream(beaver *self, byte *buf, size_t buf_len)
{
    header_t hdr;
    byte     payload[MAX_PAYLOAD];
    size_t   total = 0;
 
    while (1) {
        memset(payload, 0, sizeof(payload));
 
        if (beaver_recv(self->__fd, &self->__peer, &hdr, payload) < 0)
            return -1;
 
        if (total + hdr.len > buf_len) {
            fprintf(stderr, "[beaver] recv buffer overflow\n");
            return -1;
        }
 
        memcpy(buf + total, payload, hdr.len);
        total += hdr.len;
 
        if (hdr.len < MAX_PAYLOAD)   // last (or only) chunk
            break;
    }
 
    printf("[beaver] received %zu byte(s)\n", total);
    return (int)total;
}
 
// ─── init ──────────────────────────────────────────────────────────────────
 
static int beaver_init(beaver *self, ip_version_t ver)
{
    memset(self, 0, sizeof(*self));
 
    self->__version = ver;
    self->__state   = stateless;
 
    int domain = (ver == IPV4) ? AF_INET : AF_INET6;
    self->__fd = socket(domain, SOCK_DGRAM, 0);
    if (self->__fd < 0) { perror("socket"); return -1; }
 
    // wire up the API
    self->establish   = _beaver_establish;
    self->listen_on   = _beaver_listen_on;
    self->send_stream = _beaver_send_stream;
    self->recv_stream = _beaver_recv_stream;
 
    printf("[beaver] init OK  fd=%d\n", self->__fd);
    return 0;
}