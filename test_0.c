#include <stdio.h>
#include <string.h>

#include "simple_ringbuffer.h"

//
// Tests
//
static const char *suite_name;
static char suite_pass;
static int suites_run = 0, suites_failed = 0, suites_empty = 0;
static int tests_in_suite = 0, tests_run = 0, tests_failed = 0;

#define QUOTE(str) #str
#define ASSERT(x)                                                                                  \
    {                                                                                              \
        tests_run++;                                                                               \
        tests_in_suite++;                                                                          \
        if (!(x))                                                                                  \
        {                                                                                          \
            printf("failed assert [%s:%i] %s\n", __FILE__, __LINE__, QUOTE(x));                    \
            suite_pass = 0;                                                                        \
            tests_failed++;                                                                        \
            while (1)                                                                              \
                ;                                                                                  \
        }                                                                                          \
    }

static void SUITE_START(const char *name)
{
    suite_pass = 1;
    suite_name = name;
    suites_run++;
    tests_in_suite = 0;
}

static void SUITE_END(void)
{
    printf("Testing %s ", suite_name);
    size_t suite_i;
    for (suite_i = strlen(suite_name); suite_i < 80 - 8 - 5; suite_i++)
        printf(".");
    printf("%s\n", suite_pass ? " pass" : " fail");
    if (!suite_pass)
        suites_failed++;
    if (!tests_in_suite)
        suites_empty++;
}

#define TEST_BUFFER_SIZE 500

static void test_work(void)
{
    SUITE_START("test_work");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) ==
           TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t rdata[TEST_BUFFER_SIZE / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

static void test_work_insuff(void)
{
    SUITE_START("test_work_insuff");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    uint8_t rdata[TEST_BUFFER_SIZE] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

static void test_work_invalid(void)
{
    SUITE_START("test_work_invalid");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    int total_size = TEST_BUFFER_SIZE;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

    uint8_t rdata[TEST_BUFFER_SIZE / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)i);
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

    // work in second half
    total_size += TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 2;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE) ==
           TEST_BUFFER_SIZE - TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)(i + (TEST_BUFFER_SIZE / 2)));
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

    // Read invalid size
    uint8_t rdata_full[TEST_BUFFER_SIZE] = {0};
    len = simple_ringbuffer_get(&test_ringbuf, rdata_full, sizeof(rdata_full));

    // check read data
    for (int i = 0; i < len; i++)
    {
        // printf("i: %d, rdata_full: %d\n", i, rdata_full[i]);
        ASSERT(rdata_full[i] == (uint8_t)(i));
    }

    ASSERT(len > 0);
    ASSERT(len == total_size);
    total_size -= len;
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

    SUITE_END();
}

static void test_work_full(void)
{
    SUITE_START("test_work_full");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};
    uint8_t rdata[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < 0x10000; i++)
    {
        int total_size = i % TEST_BUFFER_SIZE;

        for (int j = 0; j < total_size; j++)
        {
            data[j] = i;
        }

        ASSERT(simple_ringbuffer_put(&test_ringbuf, data, total_size) == total_size);
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

        int len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));
        // check read data
        for (int j = 0; j < total_size; j++)
        {
            // printf("i: %d, rdata: %d\n", i, rdata[i]);
            ASSERT(rdata[j] == (uint8_t)i);
        }

        ASSERT(len == total_size);
        total_size = 0;
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_work_full_define(void)
{
    SUITE_START("test_work_full_define");

    SIMPLE_RINGBUFFER_DEFINE(test_ringbuf, TEST_BUFFER_SIZE);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};
    uint8_t rdata[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < 0x10000; i++)
    {
        int total_size = i % TEST_BUFFER_SIZE;

        for (int j = 0; j < total_size; j++)
        {
            data[j] = i;
        }

        ASSERT(simple_ringbuffer_put(&test_ringbuf, data, total_size) == total_size);
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

        int len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));
        // check read data
        for (int j = 0; j < total_size; j++)
        {
            // printf("i: %d, rdata: %d\n", i, rdata[i]);
            ASSERT(rdata[j] == (uint8_t)i);
        }

        ASSERT(len == total_size);
        total_size = 0;
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_work_read_index_big_to_write_index(void)
{
    SUITE_START("test_work_read_index_big_to_write_index");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    uint8_t rdata[TEST_BUFFER_SIZE / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    // work in second half
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)(i + TEST_BUFFER_SIZE / 2));
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    // test read_index big to write_index
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == sizeof(rdata));
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == sizeof(rdata));
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

static void test_work_read_index_big_to_write_index_middle(void)
{
    SUITE_START("test_work_read_index_big_to_write_index");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    uint8_t rdata[TEST_BUFFER_SIZE / 4] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE * 3 / 4);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE * 1 / 4);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == i + TEST_BUFFER_SIZE / 4);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    // work in second half
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)(i + TEST_BUFFER_SIZE / 2));
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE * 3 / 4);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE * 1 / 4);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)(i + TEST_BUFFER_SIZE * 3 / 4));
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    // test read_index big to write_index
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE * 3 / 4);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE * 1 / 4);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == i + TEST_BUFFER_SIZE * 1 / 4);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

#define TEST_BUFFER_SIZE_ODD 257
static void test_work_odd(void)
{
    SUITE_START("test_work_odd");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE_ODD] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE_ODD; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD / 2);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) ==
           TEST_BUFFER_SIZE_ODD - TEST_BUFFER_SIZE_ODD / 2);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t rdata[TEST_BUFFER_SIZE_ODD / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

static void test_work_insuff_odd(void)
{
    SUITE_START("test_work_insuff_odd");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE_ODD] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE_ODD; i++)
    {
        data[i] = i;
    }

    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 1);

    uint8_t rdata[TEST_BUFFER_SIZE_ODD] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        ASSERT(rdata[i] == (uint8_t)i);
    }

    ASSERT(len > 0);
    ASSERT(len == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    SUITE_END();
}

static void test_work_invalid_odd(void)
{
    SUITE_START("test_work_invalid_odd");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE_ODD] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE_ODD; i++)
    {
        data[i] = i;
    }

    int total_size = TEST_BUFFER_SIZE_ODD;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    uint8_t rdata[TEST_BUFFER_SIZE_ODD / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)i);
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    // work in second half
    total_size += TEST_BUFFER_SIZE_ODD / 2;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD) ==
           TEST_BUFFER_SIZE_ODD / 2);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)(i + (TEST_BUFFER_SIZE_ODD / 2)));
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    // Read invalid size
    uint8_t rdata_full[TEST_BUFFER_SIZE_ODD] = {0};
    len = simple_ringbuffer_get(&test_ringbuf, rdata_full, sizeof(rdata_full));

    // check read data
    for (int i = 0; i < len; i++)
    {
        // printf("i: %d, rdata_full: %d\n", i, rdata_full[i]);
        if (i == 0)
        {
            ASSERT(rdata_full[i] == (uint8_t)0);
        }
        else
        {
            ASSERT(rdata_full[i] == (uint8_t)(i - 1));
        }
    }

    ASSERT(len > 0);
    ASSERT(len == total_size);
    total_size -= len;
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    SUITE_END();
}

static void test_work_full_odd(void)
{
    SUITE_START("test_work_full_odd");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, test_buffer);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE_ODD] = {0};
    uint8_t rdata[TEST_BUFFER_SIZE_ODD] = {0};

    for (int i = 0; i < 0x10000; i++)
    {
        int total_size = i % TEST_BUFFER_SIZE_ODD;

        for (int j = 0; j < total_size; j++)
        {
            data[j] = i;
        }

        ASSERT(simple_ringbuffer_put(&test_ringbuf, data, total_size) == total_size);
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

        int len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));
        // check read data
        for (int j = 0; j < total_size; j++)
        {
            // printf("i: %d, rdata: %d\n", i, rdata[i]);
            ASSERT(rdata[j] == (uint8_t)i);
        }

        ASSERT(len == total_size);
        total_size = 0;
        ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));
    }

    SUITE_END();
}

static void test_work_read_index_big_to_write_index_odd(void)
{
    SUITE_START("test_work_read_index_big_to_write_index_odd");

    simple_ringbuffer_t test_ringbuf;
    uint8_t test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, test_buffer);

    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == 0);

    uint8_t data[TEST_BUFFER_SIZE_ODD] = {0};

    for (int i = 0; i < TEST_BUFFER_SIZE_ODD; i++)
    {
        data[i] = i;
    }

    int total_size = TEST_BUFFER_SIZE_ODD;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    uint8_t rdata[TEST_BUFFER_SIZE_ODD / 2] = {0};
    uint32_t len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == (uint8_t)i);
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    // work in second half
    total_size += TEST_BUFFER_SIZE_ODD / 2;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        ASSERT(rdata[i] == i + (TEST_BUFFER_SIZE_ODD / 2));
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    // test read_index big to write_index
    total_size += TEST_BUFFER_SIZE_ODD / 2;
    ASSERT(simple_ringbuffer_put(&test_ringbuf, data, TEST_BUFFER_SIZE_ODD / 2) > 0);
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    len = simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));

    // check read data
    for (int i = 0; i < sizeof(rdata); i++)
    {
        // printf("i: %d, rdata: %d\n", i, rdata[i]);
        if (i == 0)
        {
            ASSERT(rdata[i] == 0);
        }
        else
        {
            ASSERT(rdata[i] == (i - 1));
        }
    }

    total_size -= sizeof(rdata);
    ASSERT(len > 0);
    ASSERT(len == sizeof(rdata));
    ASSERT(simple_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_ringbuffer_size(&test_ringbuf) == total_size);
    ASSERT(simple_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(simple_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
    ASSERT(simple_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE_ODD));

    SUITE_END();
}

void test_ringbuffer(void)
{
    test_work();
    test_work_insuff();
    test_work_invalid();
    test_work_full();
    test_work_full_define();
    test_work_read_index_big_to_write_index();
    test_work_read_index_big_to_write_index_middle();

    test_work_odd();
    test_work_insuff_odd();
    test_work_invalid_odd();
    test_work_full_odd();
    test_work_read_index_big_to_write_index_odd();
}
