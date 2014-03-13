/*
Author: Alchan Kim
*/

#ifndef __SERIALPACK_H
#define __SERIALPACK_H
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "serialterm.h"

#define MTD_SETPOINT 1

#define SIF_START_MAGIC 0x02
#define SIF_BUFFER_SIZE 128
#define SIF_END_MAGIC 0x03

typedef uint8_t sif_mtd_type;

struct SIFPacket {
    uint8_t start_magic; /* 0x02 */
    sif_mtd_type method;
    char buffer[SIF_BUFFER_SIZE]; 
    uint8_t crc_chksum; /* CRC8 checksum of method plus  buffer */
    uint8_t end_magic; /*  0x03 */
};
#define SIF_PACKET_SIZE (sizeof(struct SIFPacket))

int init_sif(struct SIFPacket *packet, sif_mtd_type method, char *buffer, size_t len);
int recv_sif(struct SerialTerm *serial, struct SIFPacket *packet);
int send_sif(struct SerialTerm *serial, struct SIFPacket *packet);

size_t sif_len(struct SIFPacket *packet);
void* sif_buf(struct SIFPacket *packet);
void sif_buf_copy(struct SIFPacket *packet, void *dst, size_t bound);
