#include <stdio.h>
#include <stdint.h>

#include "serialcom/serialpack.h"

int main() {
    struct SerialTerm serial;
    struct SIFPacket packet_in;
    struct SIFPacket packet_out;
    
    char buffer[128];

    int i = 0;

    if (open_serialterm(&serial, "/dev/usb1") == -1) {
        return 0;
    }
    

    while (1){
        sprintf(buffer, "TestPacket : %d", i);
        init_sif(packet_out, 1, &buffer, 128);
        send_sif(&serial, &packet_out);

        sleep(3000);
    }

    return 0;

}
