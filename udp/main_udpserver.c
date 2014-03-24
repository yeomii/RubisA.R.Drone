#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "udp.h"

static void diep(const char *s)
{
  perror(s);
  exit(1);
}

int main(void)
{
  UDPTerm udp;

  int i;
  int msglen;
  char buf[512];
  
  if(open_udp_server(&udp, 9930, 1))
      diep("udpServer_Init");
  
  for (i=0; i<10; i++) {
    int cnt=0;
    do {
      msglen = udp_recv(&udp, buf, 512, NULL);
      cnt++;
    } while(msglen<=0);
    char ip[100];
    
    udp_recved_ip(&udp, ip, 100);

    printf("Received at cnt=%d -> '%s' from %s\n\n", cnt, buf, ip);
  }
  
  close_udp(&udp);
  return 0;
}
