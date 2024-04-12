#include <stdio.h>
#include <string.h>

extern void test_ringbuffer(void);
extern void test_data_ringbuffer(void);
extern void test_pool_ringbuffer(void);

/**
 * @brief  Main program.
 * @param  None.
 * @return None.
 */
int main(void)
{
    test_ringbuffer();
    test_data_ringbuffer();
    test_pool_ringbuffer();
}
