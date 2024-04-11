#include <string.h>

#include "simple_data_ringbuffer.h"

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define DATA_RINGBUFFER_INDEX_TO_PTR(_index, _total_size)                                          \
    ((_index >= _total_size) ? (_index - _total_size) : (_index))

int simple_data_ringbuffer_put(simple_data_ringbuffer_t *ringbuf, void *buffer)
{
    uint16_t write_index;
    uint16_t wptr;
    // printf("reserve: %d\n", simple_data_ringbuffer_reserve_size(ringbuf));
    if (simple_data_ringbuffer_reserve_size(ringbuf) == 0)
    {
        return 0;
    }

    wptr = DATA_RINGBUFFER_INDEX_TO_PTR(ringbuf->write_index, ringbuf->total_size);
    // printf("ringbuf->write_index: %d, wptr: %d\n", ringbuf->write_index, wptr);
    memcpy(ringbuf->buffer + wptr * ringbuf->item_size, buffer, ringbuf->item_size);

    // printf("write_index0: %d\n", ringbuf->write_index);
    write_index = ringbuf->write_index + 1;
    if (write_index >= (ringbuf->total_size << 1))
    {
        write_index -= (ringbuf->total_size << 1);
    }
    ringbuf->write_index = write_index;
    // printf("write_index1: %d\n", ringbuf->write_index);

    return 1;
}

int simple_data_ringbuffer_get(simple_data_ringbuffer_t *ringbuf, void *buffer)
{
    uint16_t read_index;
    uint16_t rptr;
    // printf("len0: %d\n", simple_data_ringbuffer_size(ringbuf));
    if (simple_data_ringbuffer_size(ringbuf) == 0)
    {
        return 0;
    }
    // printf("len1: %d\n", simple_data_ringbuffer_size(ringbuf) - 1);

    if (buffer != NULL)
    {
        rptr = DATA_RINGBUFFER_INDEX_TO_PTR(ringbuf->read_index, ringbuf->total_size);
        memcpy(buffer, ringbuf->buffer + rptr * ringbuf->item_size, ringbuf->item_size);
    }

    // printf("read_index0: %d\n", ringbuf->read_index);
    read_index = ringbuf->read_index + 1;
    if (read_index >= (ringbuf->total_size << 1))
    {
        read_index -= (ringbuf->total_size << 1);
    }
    ringbuf->read_index = read_index;
    // printf("read_index1: %d\n", ringbuf->read_index);

    return 1;
}

int simple_data_ringbuffer_enqueue_get(simple_data_ringbuffer_t *ringbuf, void **mem)
{
    uint16_t wptr = DATA_RINGBUFFER_INDEX_TO_PTR(ringbuf->write_index, ringbuf->total_size);

    // printf("reserve: %d\n", simple_data_ringbuffer_reserve_size(ringbuf));
    if (simple_data_ringbuffer_reserve_size(ringbuf) == 0)
    {
        /* Buffer could not be allocated */
        *mem = NULL; /* Signal the failure */
        return 0;    // full
    }

    /* We keep idx as the always-one-free, so we return preceding
     * buffer (last). Recall that last has not been updated,
     * so idx != last
     */
    *mem = ringbuf->buffer + wptr * ringbuf->item_size; /* preceding buffer */

    // printf("write_index0: %d\n", ringbuf->write_index);
    uint16_t write_index = ringbuf->write_index + 1;
    if (write_index >= (ringbuf->total_size << 1))
    {
        write_index -= (ringbuf->total_size << 1);
    }
    // printf("write_index1: %d\n", write_index);

    return write_index;
}

void simple_data_ringbuffer_enqueue(simple_data_ringbuffer_t *ringbuf, uint16_t write_index)
{
    ringbuf->write_index = write_index; /* Commit: Update write index */
}

void *simple_data_ringbuffer_dequeue_peek(simple_data_ringbuffer_t *ringbuf)
{
    uint16_t read_index;
    uint16_t rptr;
    // printf("len0: %d\n", simple_data_ringbuffer_size(ringbuf));
    if (simple_data_ringbuffer_size(ringbuf) == 0)
    {
        return NULL;
    }
    // printf("len1: %d\n", simple_data_ringbuffer_size(ringbuf) - 1);

    rptr = DATA_RINGBUFFER_INDEX_TO_PTR(ringbuf->read_index, ringbuf->total_size);
    return ringbuf->buffer + rptr * ringbuf->item_size;
}

void simple_data_ringbuffer_dequeue(simple_data_ringbuffer_t *ringbuf)
{
    simple_data_ringbuffer_get(ringbuf, NULL);
}
