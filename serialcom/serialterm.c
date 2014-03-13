/*
   Author: Alchan Kim
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <termios.h>

#include "serialterm.h"

static int ERROR_MSG(const char *str) {
    fprintf(stderr, "[serialterm]%s\n", str);
}

int open_serialterm(struct SerialTerm *st, const char *devname) {
    int fd;
    struct termios config;

    fd = open(devname, O_RDWR | O_NOCTTY /*| O_NDELAY */);

    if(fd == -1) {
        ERROR_MSG("fail to open device");
        return -1;
    }

    if(!isatty(fd)) {
        ERROR_MSG("device to open is not a tty device");
        return -1;
    }

    if(tcgetattr(fd, &config) < 0) {
        ERROR_MSG("fail to get tty configuration");
        return -1;
    }

    config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
                        INLCR | PARMRK | INPCK |
                        ISTRIP | IXON);

    config.c_oflag = 0;
    config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

    config.c_cflag &= ~(CSIZE | PARENB);
    config.c_cflag |= CS8;

    config.c_cc[VMIN] = 1;
    config.c_cc[VTIME] = 0;


    /*
        FIXME: With RPI GPIO,
        When baudrate is set to 115200,
        useless charactor \xF8 always starts with intended string...
        To circumvent this bug, baudrate is set to 9600 at last.
     */
    if(cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0 ) {
        ERROR_MSG("fail to set baudrate to 9600");
        return -1;
    }

    if(tcsetattr(fd, TCSAFLUSH, &config) < 0) {
        ERROR_MSG("fail to set configure structure");
        return -1;
    }

    st->fd = fd;
    st->nonblock = 0;
    st->closed = 0;
    st->baudrate = 9600;

    return 0;
}

int close_serialterm(struct SerialTerm *st) {
    st->closed = 1;
    return close(st->fd);
}

int serialterm_sendall(struct SerialTerm *st, char* buf, unsigned len) {
    int sent;
    int cur;

    sent = 0;
    while(sent < len) {
        cur = write(st->fd, buf + sent, len - sent);

        if(cur < 0) {
            return -1;
        }
        sent += cur;
    }
    return sent;
}

int serialterm_recvall(struct SerialTerm *st, char *buf, unsigned len) {
    int recved;
    int cur;

    recved = 0;
    while(recved < len) {
        cur = read(st->fd, buf + recved, len - recved);
        if(cur < 0) {
            return -1;
        }

        recved += cur;
    }

    return recved;
}

#if 0
int main() {
    struct SerialTerm st;
    int res;
    char buf[1524] = {0,};

    open_serialterm(&st, "/dev/ttyAMA0");
    res = serialterm_recvall(&st, buf, 1000);

    printf("%s", buf);
    close_serialterm(&st);
    return 0;
}
#endif
