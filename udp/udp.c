#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "udp.h"

int open_udp_client(UDPTerm *udp, int blocking) {
    if ((udp->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        return 1;

    if(blocking==0) { 
        int x;
        x = fcntl(udp->fd, F_GETFL, 0);
        fcntl(udp->fd, F_SETFL, x | O_NONBLOCK);
    }
    return 0;
}

int open_udp_server(UDPTerm *udp, unsigned int port, int blocking) {
    if ((udp->fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        return 1;

    if(blocking==0) { 
        int x;
        x = fcntl(udp->fd, F_GETFL, 0);
        fcntl(udp->fd, F_SETFL, x | O_NONBLOCK);
    }

    //Bind the socket  
    memset((char *) &udp->si_server, 0, sizeof(udp->si_server));
    udp->si_server.sin_family = AF_INET;
    udp->si_server.sin_port = htons(port);
    udp->si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(udp->fd, (const struct sockaddr*)&udp->si_server, sizeof(udp->si_server))==-1) return 3;

    return 0;
}

static int init_send_si(struct sockaddr_in *si, const char* host, int port)
{
    memset((char *) si, 0, sizeof(struct sockaddr_in));
    si->sin_family = AF_INET;
    si->sin_port = htons(port);
    return inet_aton(host, &si->sin_addr);
}

int udp_send(UDPTerm *udp, char* buf, unsigned int size, const char* host, unsigned int port, int *wblock_out) {
    struct sockaddr_in si;
    int res;
    if(!init_send_si(&si, host, port)) {
        fprintf(stderr, "[udp]unrecognized host:%s\n", host);
        return -1;
    }
    res = sendto(udp->fd, buf, size, 0, (const struct sockaddr*)&si, sizeof(si));

    if(res == -1 && wblock_out) {
        if(errno == EWOULDBLOCK)
            *wblock_out = 1;
        else
            *wblock_out = 0;
    }
    return res;
}

int udp_recv(UDPTerm *udp, char* buf, unsigned int size, int *wblock_out) {
    int res;
    res = recvfrom(udp->fd, buf, size, 0,
                   (struct sockaddr*)&udp->si_last_recved,
                   &udp->recved_si_len);

    if(res == -1 && wblock_out) {
        if(errno == EWOULDBLOCK)
            *wblock_out = 1;
        else
            *wblock_out = 0;
    }
    return res;
}

void udp_recved_ip(UDPTerm *udp, char *host, unsigned int max_host_size) {
    char* addr_str;
    size_t addr_str_size;
    addr_str = inet_ntoa(udp->si_last_recved.sin_addr);
    addr_str_size = strlen(addr_str) + 1;

    if(max_host_size >= addr_str_size)
        strcpy(host, addr_str);
    else
        strncpy(host, addr_str, max_host_size - 1);
} 

void close_udp(UDPTerm *udp)
{  
    close(udp->fd);
}
