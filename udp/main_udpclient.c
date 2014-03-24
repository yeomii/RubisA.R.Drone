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
  if(open_udp_client(&udp, 1))
      diep("udpClient_Init");
  
  for (i=0; i<10; i++) {
    printf((char*)"Sending packet %d\n", i);
    msglen=sprintf(buf, (char*)"This is packet %d\n", i);
    if (udp_send(&udp, buf, msglen+1, "192.168.1.1", 9930, NULL) == -1)
        diep("send");
    usleep(100000);
  }
  
  close_udp(&udp);
  return 0;
}
