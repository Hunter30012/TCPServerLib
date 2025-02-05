#ifndef _BYTE_CIRCULAR_BUFFER_H
#define _BYTE_CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct _ByteCircularBuffer
{
    unsigned char *buffer;
    uint16_t buffer_size;
    uint16_t front;
    uint16_t rear;
    uint16_t current_size;
} ByteCircularBuffer_t;

ByteCircularBuffer_t* BCBCreateNew(uint16_t size);

void BCBFree(ByteCircularBuffer_t* bcb);

uint16_t BCBWrite(ByteCircularBuffer_t *bcb, unsigned char *data, uint16_t data_size);

uint16_t BCBRead(ByteCircularBuffer_t *bcb, unsigned char *buffer, uint16_t data_size, bool remove_read_bytes);

bool BCBIsFull(ByteCircularBuffer_t *bcb);

uint16_t BCBAvailableSize(ByteCircularBuffer_t *bcb);

void BCBReset(ByteCircularBuffer_t *bcb);


#endif