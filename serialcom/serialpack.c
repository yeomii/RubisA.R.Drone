/*
Author: Alchan Kim
*/

#include "crc8.h"
#include "serialpack.h"


/* XXX: what if two different machine, which communicate each other,
        have different endianess?
*/

static bool verify_sif(struct SIFPacket *packet);
inline static bool big_endian() {
    uint16_t test = 1;

    return *((uint8_t *)&test) == 0;
}

static uint8_t eval_crc(char *buffer, size_t len) {
    return Crc8(buffer, len);
}


static int ERROR_MSG(const char *str) {
    fprintf(stderr, "[serialpack]%s\n", str);
}

int init_sif(struct SIFPacket *packet, sif_mtd_type method, char *buffer, size_t len) {
    packet->start_magic = SIF_START_MAGIC;
    packet->end_magic = SIF_END_MAGIC;
    packet->method = method;

    if(len > SIF_BUFFER_SIZE) {
        ERROR_MSG("(init_sif) sif buffer has been overflowed!");
        return -1;
    }
    memset(packet->buffer, 0, SIF_BUFFER_SIZE);
    memcpy(packet->buffer, buffer, len);

    packet->crc_chksum = eval_crc(&packet->method, sizeof(sif_mtd_type) + SIF_BUFFER_SIZE);

    return 0;
}

static bool verify_sif(struct SIFPacket *packet) {
    return (packet->start_magic == SIF_START_MAGIC &&
            packet->end_magic == SIF_END_MAGIC &&
            packet->crc_chksum == eval_crc(&packet->method, sizeof(sif_mtd_type) + SIF_BUFFER_SIZE));
}

#define WARN_MSG(msg) ERROR_MSG(msg)
/* blocking recv */
int recv_sif(struct SerialTerm *serial, struct SIFPacket *packet) {
    uint8_t stream_buf[SIF_PACKET_SIZE];
    int size;
    int recved;

    size = 0;
    while(1) {
        if(size >= SIF_PACKET_SIZE) {
            if(verify_sif((struct SIFPacket *)stream_buf)) {
                memcpy(packet, stream_buf, SIF_PACKET_SIZE);
                break;
            }
            else {
                /* Verifycation failed */
                WARN_MSG("packet verification(MAGIC, CRC and so on) failed! shifting stream buffer");
                memmove(stream_buf, stream_buf + 1, SIF_PACKET_SIZE - 1);
                size--;
            }
        }
        else {
            recved =  serialterm_recvall(serial, stream_buf + size, 1);
            if(recved < 0) {
                ERROR_MSG("(recv_sif) failed to recv one byte");
                return -1;
            }
            size += recved;
        }
    }
    return size;
}

int send_sif(struct SerialTerm *serial, struct SIFPacket *packet) {
    return serialterm_sendall(serial, (char *)packet, sizeof(struct SIFPacket));
}

size_t sif_len(struct SIFPacket *packet) {
    return SIF_PACKET_SIZE;
}

void* sif_buf(struct SIFPacket *packet) {
    return packet->buffer;
}

void sif_buf_copy(struct SIFPacket *packet, void *dst, size_t bound) {
    size_t len;

    len = sif_len(packet);
    memcpy(dst, packet->buffer, (bound < len)? bound : len);
}
