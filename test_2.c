#include <stdio.h>
#include <string.h>

#include "simple_pool.h"
#if 1

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

static void test_pool_work(void)
{
    SUITE_START("test_pool_work");

    SIMPLE_POOL_DEFINE(test_pool, TEST_BUFFER_SIZE, TEST_USER_DATA_SIZE);

    SIMPLE_POOL_INIT(test_pool, TEST_BUFFER_SIZE, TEST_USER_DATA_SIZE);

    struct test_user_data *ptr_save[TEST_BUFFER_SIZE];

    int total_size = TEST_BUFFER_SIZE;
    ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
    ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
    ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
    ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE - total_size);
    ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
    ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE));

    for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
    {
        struct test_user_data *data;
        SIMPLE_POOL_DEQUEUE(&test_pool, data);

        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            data->data[i] = i + loop;
        }
        ptr_save[loop] = data;

        total_size--;
        ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
        ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE - total_size);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE));
    }

    for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
    {
        struct test_user_data *data;
        data = ptr_save[loop];

        // check read data
        for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
        {
            ASSERT(data->data[i] == (uint8_t)(i + loop));
        }

        SIMPLE_POOL_ENQUEUE(&test_pool, data);
        total_size++;
        ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
        ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE - total_size);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE));
    }

    SUITE_END();
}

static void test_pool_work_full(void)
{
    SUITE_START("test_pool_work_full");

    SIMPLE_POOL_DEFINE(test_pool, TEST_BUFFER_SIZE, TEST_USER_DATA_SIZE);

    SIMPLE_POOL_INIT(test_pool, TEST_BUFFER_SIZE, TEST_USER_DATA_SIZE);

    struct test_user_data *ptr_save[TEST_BUFFER_SIZE];

    int total_size = TEST_BUFFER_SIZE;
    ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
    ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
    ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
    ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE - total_size);
    ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
    ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE));

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        // clear data first.
        for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
        {
            struct test_user_data *data;
            SIMPLE_POOL_DEQUEUE(&test_pool, data);
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                data->data[i] = 0;
            }
            ptr_save[loop] = data;

            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        }

        for (int loop = 0; loop < TEST_BUFFER_SIZE; loop++)
        {
            struct test_user_data *data;
            data = ptr_save[TEST_BUFFER_SIZE - loop - 1]; // change ptr order

            SIMPLE_POOL_ENQUEUE(&test_pool, data);
            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        }

        // Test work
        int work_cnt = test_cnt % TEST_BUFFER_SIZE;

        for (int loop = 0; loop < work_cnt; loop++)
        {
            struct test_user_data *data;
            SIMPLE_POOL_DEQUEUE(&test_pool, data);
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                // Make sure origin data is zero, avoid data overflow
                ASSERT(data->data[i] == 0);
                data->data[i] = i + loop + test_cnt;
            }
            ptr_save[loop] = data;

            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        }

        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == TEST_BUFFER_SIZE - work_cnt);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == work_cnt);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (work_cnt == TEST_BUFFER_SIZE));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (work_cnt == 0));

        for (int loop = 0; loop < work_cnt; loop++)
        {
            struct test_user_data *data;
            data = ptr_save[work_cnt - loop - 1]; // change ptr order

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE; i++)
            {
                ASSERT(data->data[i] == (uint8_t)(i + (work_cnt - loop - 1) + test_cnt));
            }

            SIMPLE_POOL_ENQUEUE(&test_pool, data);
            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE);
        }

        work_cnt = 0;
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == TEST_BUFFER_SIZE - work_cnt);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == work_cnt);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (work_cnt == TEST_BUFFER_SIZE));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (work_cnt == 0));
    }

    SUITE_END();
}

#define TEST_USER_DATA_SIZE_ODD 0x203
struct test_user_data_odd
{
    uint8_t data[TEST_USER_DATA_SIZE_ODD];
};

#define TEST_BUFFER_SIZE_ODD 257

static void test_pool_work_odd(void)
{
    SUITE_START("test_pool_work_odd");

    SIMPLE_POOL_DEFINE(test_pool, TEST_BUFFER_SIZE_ODD, TEST_USER_DATA_SIZE_ODD);

    SIMPLE_POOL_INIT(test_pool, TEST_BUFFER_SIZE_ODD, TEST_USER_DATA_SIZE_ODD);

    struct test_user_data_odd *ptr_save[TEST_BUFFER_SIZE_ODD];

    int total_size = TEST_BUFFER_SIZE_ODD;
    ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
    ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
    ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
    ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
    ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE_ODD));

    for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
    {
        struct test_user_data_odd *data;
        SIMPLE_POOL_DEQUEUE(&test_pool, data);
        for (int i = 0; i < TEST_USER_DATA_SIZE_ODD; i++)
        {
            data->data[i] = i + loop;
        }
        ptr_save[loop] = data;

        total_size--;
        ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
        ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE_ODD));
    }

    for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
    {
        struct test_user_data_odd *data;
        data = ptr_save[loop];

        // check read data
        for (int i = 0; i < TEST_USER_DATA_SIZE_ODD; i++)
        {
            ASSERT(data->data[i] == (uint8_t)(i + loop));
        }

        SIMPLE_POOL_ENQUEUE(&test_pool, data);
        total_size++;
        ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
        ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - total_size);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE_ODD));
    }

    SUITE_END();
}

static void test_pool_work_full_odd(void)
{
    SUITE_START("test_pool_work_full_odd");

    SIMPLE_POOL_DEFINE(test_pool, TEST_BUFFER_SIZE_ODD, TEST_USER_DATA_SIZE_ODD);

    SIMPLE_POOL_INIT(test_pool, TEST_BUFFER_SIZE_ODD, TEST_USER_DATA_SIZE_ODD);

    struct test_user_data_odd *ptr_save[TEST_BUFFER_SIZE_ODD];

    int total_size = TEST_BUFFER_SIZE_ODD;
    ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
    ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
    ASSERT(SIMPLE_POOL_SIZE(&test_pool) == total_size);
    ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - total_size);
    ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (total_size == 0));
    ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (total_size == TEST_BUFFER_SIZE_ODD));

    for (int test_cnt = 0; test_cnt < 0x1000; test_cnt++)
    {
        // clear data first.
        for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
        {
            struct test_user_data_odd *data;
            SIMPLE_POOL_DEQUEUE(&test_pool, data);
            for (int i = 0; i < TEST_USER_DATA_SIZE_ODD; i++)
            {
                data->data[i] = 0;
            }
            ptr_save[loop] = data;

            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        }

        for (int loop = 0; loop < TEST_BUFFER_SIZE_ODD; loop++)
        {
            struct test_user_data_odd *data;
            data = ptr_save[TEST_BUFFER_SIZE_ODD - loop - 1]; // change ptr order

            SIMPLE_POOL_ENQUEUE(&test_pool, data);
            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        }

        int work_cnt = test_cnt % TEST_BUFFER_SIZE_ODD;

        for (int loop = 0; loop < work_cnt; loop++)
        {
            struct test_user_data_odd *data;
            SIMPLE_POOL_DEQUEUE(&test_pool, data);
            for (int i = 0; i < TEST_USER_DATA_SIZE_ODD; i++)
            {
                // Make sure origin data is zero, avoid data overflow
                ASSERT(data->data[i] == 0);
                data->data[i] = i + loop + test_cnt;
            }
            ptr_save[loop] = data;

            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        }

        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - work_cnt);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == work_cnt);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (work_cnt == TEST_BUFFER_SIZE_ODD));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (work_cnt == 0));

        for (int loop = 0; loop < work_cnt; loop++)
        {
            struct test_user_data_odd *data;
            data = ptr_save[work_cnt - loop - 1]; // change ptr order

            // check read data
            for (int i = 0; i < TEST_USER_DATA_SIZE_ODD; i++)
            {
                ASSERT(data->data[i] == (uint8_t)(i + (work_cnt - loop - 1) + test_cnt));
            }

            SIMPLE_POOL_ENQUEUE(&test_pool, data);
            ASSERT(SIMPLE_POOL_TOTAL_CNT(&test_pool) == TEST_BUFFER_SIZE_ODD);
            ASSERT(SIMPLE_POOL_ITEM_SIZE(&test_pool) == TEST_USER_DATA_SIZE_ODD);
        }

        work_cnt = 0;
        ASSERT(SIMPLE_POOL_SIZE(&test_pool) == TEST_BUFFER_SIZE_ODD - work_cnt);
        ASSERT(SIMPLE_POOL_RESERVE_SIZE(&test_pool) == work_cnt);
        ASSERT(SIMPLE_POOL_IS_EMPTY(&test_pool) == (work_cnt == TEST_BUFFER_SIZE_ODD));
        ASSERT(SIMPLE_POOL_IS_FULL(&test_pool) == (work_cnt == 0));
    }

    SUITE_END();
}

void test_pool_ringbuffer(void)
{
    test_pool_work();
    test_pool_work_full();

    test_pool_work_odd();
    test_pool_work_full_odd();
}
#endif
