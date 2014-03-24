#ifndef __UDP_H
#define __UDP_H
#include <netinet/in.h>

typedef struct _UDPTerm {
  int fd;
  
  socklen_t recved_si_len;
  struct sockaddr_in si_server;
  struct sockaddr_in si_last_recved; // XXX: may have synchronization issue
} UDPTerm;

int open_udp_server(UDPTerm *udp, unsigned int port, int blocking);
int open_udp_client(UDPTerm *udp, int blocking);

int udp_send(UDPTerm *udp, char* buf, unsigned int size, const char *host, unsigned int port, int *wblock_out);
int udp_recv(UDPTerm *udp, char *buf, unsigned int size, int *wblock_out);

void udp_recved_ip(UDPTerm *udp, char *host, unsigned int max_host_size);

void close_udp(UDPTerm *udp);

#endif
