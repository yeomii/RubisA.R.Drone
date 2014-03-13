/*
author: Alchan Kim
*/

#ifndef __SERIALTERM_H
#define __SERIALTERM_H
#endif

struct SerialTerm {
    int fd;
    int nonblock;
    int baudrate;
    int closed;
};

/* return zero on success
   or, if error occured, return -1 and print error message to STDERR. */
int open_serialterm(struct SerialTerm *st, const char *devname);
int close_serialterm(struct SerialTerm *st);

/*
   Send len byte(s) of given byte string

   return bytes of sent string
   or return -1 if error occured

   This function will block until all bytes len in buffer sent
*/
int serialterm_sendall(struct SerialTerm *st, char* buf, unsigned len);

/*
   Receive len bytes from serial port
   return 
   or return -1 if error occured

   This function will block until all bytes len recved to buf
*/
int serialterm_recvall(struct SerialTerm *st, char *buf, unsigned len);
