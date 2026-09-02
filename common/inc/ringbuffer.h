#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    uint8_t *pStart;
    uint8_t *pEnd;

    volatile uint8_t *pRead;
    volatile uint8_t *pWrite;
} rb_t;

rb_t rb_init(uint8_t *pBuffer, unsigned size);
void rb_write(rb_t *pRb, const uint8_t *pData, unsigned len);
void rb_read(rb_t *pRb, uint8_t *pData, unsigned len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif