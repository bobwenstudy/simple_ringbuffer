# 简介

在嵌入式开发中，经常有需要用到RingBuffer的概念，在RingBuffer中经常遇到一个Buffer满和Buffer空的判断的问题，一般的做法是留一个单位的buffer不用，这样做最省事，但是当RingBuffer单位是一个结构体时，这个浪费就不能接受了，市面上大多数解决办法是**镜像指示位**办法，但是具体实现上又有各种设计，但是并不是满足的开发需要，所以有本项目。

本项目地址：[bobwenstudy/easy_timer (github.com)](https://github.com/bobwenstudy/easy_timer)，参考资料有：[环形缓冲器 - 维基百科，自由的百科全书 (wikipedia.org)](https://zh.wikipedia.org/wiki/環形緩衝區#镜像指示位)，[ring buffer，一篇文章讲透它？ - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/534098236)。

对比现有的实现，本项目的特点是。

| 类别                       | simple_ringbuffer | [kfifo(linux)](https://github.com/torvalds/linux/blob/master/lib/kfifo.c) | [ringbuffer (rt-thread)](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device-ipc/ringbuffer/ringbuffer?id=ringbuffer) |
| -------------------------- | ----------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 需要mirror位（多进程风险） | 否                | 否                                                           | 是                                                           |
| 需要个数为2的幂            | 否                | 是                                                           | 否                                                           |
| 支持结构体成员             | 是                | 是（linux5.7）                                               |                                                              |



## mirror位

多了Mirror位就会有一个多进程操作的风险，除非Mirror位和数值同时写入。所以最好不要有Mirror位。



## 个数为2的幂

Linux的kfifo解决了Mirror位的问题，因为其用到了uint32_t回环的特性，需要个数为2个幂。虽然这样大大减少了算法工作量，也可以用位运算来优化取余预算的计算效率。但是使用起来多少不是很舒适，尤其设计到对结构体成员操作时，一不注意又要浪费Buffer。



## 支持结构体成员

其实如果RingBuffer成员的单位为1个字节的话，其实没必要在乎1个字节的损失，只是使用时需要多申请1个字节，多少看起来不是很清爽。

但是当RingBuffer的成员的单位为很大的值时，1个成员的损失才至关重要。

现有的项目考虑灵活性，RingBuffer需要支持各种字节操作，效率并不高，本项目针对结构体操作需要，专门设计了Data_RingBuffer工具来对多字节场景进行处理，操作效率更高，并提供了2种操作接口，以满足不同业务操作的需要。



# 镜像指示位-本项目实现

本项目不想有Mirror的操作问题，也不想有个数2的幂的限制。处理上做了一些特殊的处理，严格区分index和ptr的概念。

index的取值范围为`[0~2n-1]`，并不像Linux取到最大值，解决了个数2的幂的限制。

```
write_index = ringbuf->write_index + len;
if (write_index >= (ringbuf->total_size << 1))
{
    write_index -= (ringbuf->total_size << 1);
}
ringbuf->write_index = write_index;
```

其中ptr的获取，考虑效率使用减法，不使用取余运算。

```
#define RINGBUFFER_INDEX_TO_PTR(_index, _total_size)                                               \
    ((_index >= _total_size) ? (_index - _total_size) : (_index))
uint32_t wptr = RINGBUFFER_INDEX_TO_PTR(ringbuf->write_index, ringbuf->total_size);
```




# 代码结构

代码结构如下所示：

- **simple_ringbuffer**：Ringbuffer实现，包含结构体操作实现`simple_data_ringbuffer`。
- **test_0.c**和**test_1.c**：测试例程。
- **main.c**：测试例程。
- **build.mk**和**Makefile**：Makefile编译环境。
- **README.md**：说明文档

```shell
simple_ringbuffer
 ├── simple_ringbuffer
 │   ├── simple_data_ringbuffer.c
 │   ├── simple_data_ringbuffer.h
 │   ├── simple_ringbuffer.c
 │   └── simple_ringbuffer.h
 ├── build.mk
 ├── code_format.py
 ├── LICENSE
 ├── main.c
 ├── Makefile
 ├── README.md
 ├── test_0.c
 └── test_1.c
```



# 使用说明

具体如何使用直接看例程就行，非常简单，看函数名和变量名即可。

## 单字节操作

使用提供`simple_ringbuffer.h`接口操作即可。

```c
// Define ringbuf.
SIMPLE_RINGBUFFER_DEFINE(test_ringbuf, 0x100);

// Put data to ringbuf.
uint8_t data[0x10];
simple_ringbuffer_put(&test_ringbuf, data, sizeof(data));

// Get data from ringbuf.
uint8_t rdata[0x10];
simple_ringbuffer_get(&test_ringbuf, rdata, sizeof(rdata));
```



## 结构体操作

使用提供`simple_data_ringbuffer.h`接口操作即可。提供了两种接口，按需使用。

```c
struct test_user_data
{
    uint8_t data[0x10];
};

// Define ringbuf.
SIMPLE_DATA_RINGBUFFER_DEFINE(test_ringbuf, 0x100, sizeof(struct test_user_data));


// API1
// Put data to ringbuf.
struct test_user_data data;
simple_ringbuffer_put(&test_ringbuf, &data);

// Get data to ringbuf.
struct test_user_data rdata;
simple_ringbuffer_get(&test_ringbuf, &rdata);


// API2
// Enqueue data to ringbuf.
struct test_user_data *data = NULL;
uint16_t index = simple_data_ringbuffer_enqueue_get(&test_ringbuf, (void **)&data); // enqueue get

simple_data_ringbuffer_enqueue(&test_ringbuf, index); // real enqueue


// Dequeue data from ringbuf.
struct test_user_data *data;
data = simple_data_ringbuffer_dequeue_peek(&test_ringbuf); // dequeue peek

simple_data_ringbuffer_dequeue(&test_ringbuf); // real dequeue
```





# 测试说明

## 环境搭建

目前测试暂时只支持Windows编译，最终生成exe，可以直接在PC上跑。

目前需要安装如下环境：
- GCC环境，笔者用的msys64+mingw，用于编译生成exe，参考这个文章安装即可。[Win7下msys64安装mingw工具链 - Milton - 博客园 (cnblogs.com)](https://www.cnblogs.com/milton/p/11808091.html)。


## 编译说明

本项目都是由makefile组织编译的，编译整个项目只需要执行`make all`即可。


也就是可以通过如下指令来编译工程：

```shell
make all
```

而后运行执行`make run`即可运行例程，例程中实现了上述文档说明的问题和API的基本测试。

```shell
PS D:\workspace\github\simple_ringbuffer> make run
Compiling  : "test_0.c"
Compiling  : "test_1.c"
Linking    : "output/main.exe"
Building   : "output/main.exe"
Start Build Image.
objcopy -v -O binary output/main.exe output/main.bin
copy from `output/main.exe' [pei-i386] to `output/main.bin' [binary]
objdump --source --all-headers --demangle --line-numbers --wide output/main.exe > output/main.lst
Print Size
   text    data     bss     dec     hex filename
 118200  265384    2644  386228   5e4b4 output/main.exe
./output/main.exe
Testing test_work .......................................................... pass
Testing test_work_insuff ................................................... pass
Testing test_work_invalid .................................................. pass
Testing test_work_full ..................................................... pass
Testing test_work_full_define .............................................. pass
Testing test_work_read_index_big_to_write_index ............................ pass
Testing test_work_read_index_big_to_write_index ............................ pass
Testing test_work_odd ...................................................... pass
Testing test_work_insuff_odd ............................................... pass
Testing test_work_invalid_odd .............................................. pass
Testing test_work_full_odd ................................................. pass
Testing test_work_read_index_big_to_write_index_odd ........................ pass
Testing test_data_work ..................................................... pass
Testing test_data_work_full ................................................ pass
Testing test_data_work_full_define ......................................... pass
Testing test_data_work_full_define_enqueue ................................. pass
Testing test_data_work_odd ................................................. pass
Testing test_data_work_full_odd ............................................ pass
Executing 'run: all' complete!
```

可以看到，所有涉及到测试都通过。



