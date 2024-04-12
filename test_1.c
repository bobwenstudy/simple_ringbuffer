#include <stdio.h>
#include <string.h>

#include "simple_data_ringbuffer.h"

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

#define TEST_USER_DATA_SIZE 0x200
struct test_user_data
{
    uint8_t data[TEST_USER_DATA_SIZE];
};

#define TEST_BUFFER_SIZE 256

static void test_data_work(void)
{
    SUITE_START("test_data_work");

    simple_data_ringbuffer_t test_ringbuf;
    struct test_user_data test_buffer[TEST_BUFFER_SIZE];

    simple_data_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, sizeof(struct test_user_data),
                                test_buffer);

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    int total_size = 0;
    for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
    {
        struct test_user_data data;
        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            data.data[i] = i + loop;
        }

        total_size++;
        ASSERT(simple_data_ringbuffer_put(&test_ringbuf, &data) > 0);
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
    {
        struct test_user_data data;
        int len = simple_data_ringbuffer_get(&test_ringbuf, &data);

        // check read data
        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            ASSERT(data.data[i] == (uint8_t)(i + loop));
        }

        total_size--;
        ASSERT(len > 0);
        ASSERT(len == 1);
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_data_work_full(void)
{
    SUITE_START("test_data_work_full");

    simple_data_ringbuffer_t test_ringbuf;
    struct test_user_data test_buffer[TEST_BUFFER_SIZE];

    simple_data_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE, sizeof(struct test_user_data),
                                test_buffer);

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        int total_size = test_cnt % TEST_BUFFER_SIZE;

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                data.data[i] = i + loop + test_cnt;
            }

            ASSERT(simple_data_ringbuffer_put(&test_ringbuf, &data) == 1);
        }
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            int len = simple_data_ringbuffer_get(&test_ringbuf, &data);

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                ASSERT(data.data[i] == (uint8_t)(i + loop + test_cnt));
            }

            ASSERT(len > 0);
            ASSERT(len == 1);
        }

        total_size = 0;
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_data_work_full_define(void)
{
    SUITE_START("test_data_work_full_define");

    SIMPLE_DATA_RINGBUFFER_DEFINE(test_ringbuf, TEST_BUFFER_SIZE, sizeof(struct test_user_data));

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        int total_size = test_cnt % TEST_BUFFER_SIZE;

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                data.data[i] = i + loop + test_cnt;
            }

            ASSERT(simple_data_ringbuffer_put(&test_ringbuf, &data) == 1);
        }
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            int len = simple_data_ringbuffer_get(&test_ringbuf, &data);

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                ASSERT(data.data[i] == (uint8_t)(i + loop + test_cnt));
            }

            ASSERT(len > 0);
            ASSERT(len == 1);
        }

        total_size = 0;
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_data_work_full_define_enqueue_dequeue(void)
{
    SUITE_START("test_data_work_full_define_enqueue");

    SIMPLE_DATA_RINGBUFFER_DEFINE(test_ringbuf, TEST_BUFFER_SIZE, sizeof(struct test_user_data));

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        int total_size = test_cnt % TEST_BUFFER_SIZE;

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data *data = NULL;
            uint16_t index = simple_data_ringbuffer_enqueue_get(&test_ringbuf, (void **)&data);
            ASSERT(data != NULL);

            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                data->data[i] = i + loop + test_cnt;
            }
            simple_data_ringbuffer_enqueue(&test_ringbuf, index);
        }
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data *data;
            data = simple_data_ringbuffer_dequeue_peek(&test_ringbuf);
            ASSERT(data != NULL);

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                ASSERT(data->data[i] == (uint8_t)(i + loop + test_cnt));
            }

            simple_data_ringbuffer_dequeue(&test_ringbuf);
        }

        total_size = 0;
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

#define TEST_BUFFER_SIZE_ODD 257

static void test_data_work_odd(void)
{
    SUITE_START("test_data_work_odd");

    simple_data_ringbuffer_t test_ringbuf;
    struct test_user_data test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_data_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, sizeof(struct test_user_data),
                                test_buffer);

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    int total_size = 0;
    for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
    {
        struct test_user_data data;
        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            data.data[i] = i + loop;
        }

        total_size++;
        ASSERT(simple_data_ringbuffer_put(&test_ringbuf, &data) > 0);
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) ==
               TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) ==
               (total_size == TEST_BUFFER_SIZE_ODD));
    }

    for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
    {
        struct test_user_data data;
        int len = simple_data_ringbuffer_get(&test_ringbuf, &data);

        // check read data
        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            ASSERT(data.data[i] == (uint8_t)(i + loop));
        }

        total_size--;
        ASSERT(len > 0);
        ASSERT(len == 1);
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) ==
               TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) ==
               (total_size == TEST_BUFFER_SIZE_ODD));
    }

    SUITE_END();
}

static void test_data_work_full_odd(void)
{
    SUITE_START("test_data_work_full_odd");

    simple_data_ringbuffer_t test_ringbuf;
    struct test_user_data test_buffer[TEST_BUFFER_SIZE_ODD];

    simple_data_ringbuffer_init(&test_ringbuf, TEST_BUFFER_SIZE_ODD, sizeof(struct test_user_data),
                                test_buffer);

    ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == 0);
    ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
    ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == 1);
    ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) == 0);

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        int total_size = test_cnt % TEST_BUFFER_SIZE_ODD;

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                data.data[i] = i + loop + test_cnt;
            }

            ASSERT(simple_data_ringbuffer_put(&test_ringbuf, &data) == 1);
        }
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) ==
               TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) ==
               (total_size == TEST_BUFFER_SIZE_ODD));

        for (int loop = 0; loop < total_size; loop++)
        {
            struct test_user_data data;
            int len = simple_data_ringbuffer_get(&test_ringbuf, &data);

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                ASSERT(data.data[i] == (uint8_t)(i + loop + test_cnt));
            }

            ASSERT(len > 0);
            ASSERT(len == 1);
        }

        total_size = 0;
        ASSERT(simple_data_ringbuffer_total_size(&test_ringbuf) == TEST_BUFFER_SIZE_ODD);
        ASSERT(simple_data_ringbuffer_size(&test_ringbuf) == total_size);
        ASSERT(simple_data_ringbuffer_reserve_size(&test_ringbuf) ==
               TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(simple_data_ringbuffer_is_empty(&test_ringbuf) == (total_size == 0));
        ASSERT(simple_data_ringbuffer_is_full(&test_ringbuf) ==
               (total_size == TEST_BUFFER_SIZE_ODD));
    }

    SUITE_END();
}

void test_data_ringbuffer(void)
{
    test_data_work();
    test_data_work_full();
    test_data_work_full_define();
    test_data_work_full_define_enqueue_dequeue();

    test_data_work_odd();
    test_data_work_full_odd();
}
