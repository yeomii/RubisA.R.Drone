#include <stdio.h>
#include <stdint.h>
#include "serialpack.h"

int main() {
    struct SerialTerm serial;
    struct SIFPacket packet;

    // init_sif(&packet, 1, "HelloWorld!", 11);
    open_serialterm(&serial, "/dev/ttyAMA0");
    
    printf("\n%d\n", recv_sif(&serial, &packet));

    close_serialterm(&serial);
    return 0;
}
