# 基于Lora(Sub-GHz)实现UART的无线透传

## 基本原理

* 硬件部分
  * STM32WLE5CBU6内含一个最高频率可到48 MHz的Arm Cortex-M4 32-bit RISC core，搭配一个22dBm的semtech SX1262 Lora芯片。
  * 使用两个串口（一个用户系统打印，一个用于数据收发）。
* 软件部分
  * 采用RT-thread实现系统级调度
  * 通过环形缓冲区（ring buffer）实现数据的转发，以实现串口和无线的速率匹配。
  * 通过消息队列通知每次接收到的数据（数据来源，数据大小）。

---

## 实现方式

### 定义使用的数据结构

* 定义两个ringbuffer：
  * 这两个环形缓冲区一个是lora的发送区，一个是uart的发送区，缓冲区长度可以根据情况进行设置。
``` C {.line-numbers highlight=[1, 2]}
struct rt_ringbuffer *lora_rx_rb;
struct rt_ringbuffer *uart_tx_rb;
#define RING_BUFFER_LEN         256
```
* 定义两个消息队列，分别用于告知（lora/uart）每次需要发送的数据类型，数据大小：
``` C {.line-numbers highlight=[1, 2]}
struct rt_messagequeue lora_tx_mq;
struct rt_messagequeue uart_tx_mq;
```

* 定义发送消息数据结构，包含以下部分：
  * 设备结构体指针: 数据来源于哪个设备，暂不使用，建议为RT_NULL;
  * 消息的类型：发送的是数据还是配置指令；
  * 消息实体：数据大小或者指令；
``` C {.line-numbers highlight=[8, 14, 21]}
#define MSG_TYPE_DATA   0x00
#define MSG_TYPE_CFG    0x01

/* 发送消息数据结构*/
struct send_msg
{
    /* What device does the data come from? */
    rt_device_t dev;
    /* 
     * message type
     * data
     * configuration instructions 
     */
    rt_uint8_t  type;
    /* 
     * valid information
     * data size
     * or
     * instruction
     */
    rt_size_t size;
};
```

* 定义lora数据包，主要包含以下部分：
  * 目标地址
  * 发送地址
  * 中转地址：不中转为0x00，故不能使用地址0x00；
  * 数据类型：数据,命令,ACK；
  * 本轮发送的数据总长度：一般为发送消息数据的长度；
  * 数据长度：
  * 有效数据：当为配置包时，为键值对组合，必定为偶数；
  * 数据校验码：通讯自带CRC校验，故不使用

| 说明 | 目标地址 | 发送地址 | 中转地址 | 数据类型 | 数据总长度 | 数据长度 | 有效数据 |
| :--: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: | :-----: |
| 类型 | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t * N |
| 示例 | 0xA5 | 0xA5 | 0x00 | 0x00 | 0x20 | 0x20 | 0x01 * N |

``` C {.line-numbers highlight=[10, 12, 14, 19, 21, 23]}
#define LORA_TYPE_DATA   0x00
#define LORA_TYPE_CFG    0x01
#define LORA_TYPE_ACK    0x02
#define LORA_PAYLOAD_LEN 0x20

/* lora data packet */
struct lora_data_pkt
{
    /* destination address */
    rt_uint8_t dst_addr;
    /* source address */
    rt_uint8_t src_addr;
    /* transit address */
    rt_uint8_t mid_addr;
    /* type of data
     * data
     * configuration instructions
     * ACK
     */
    rt_uint8_t type;
    /* data total length */
    rt_uint8_t total_len;
    /* data actual length */
    rt_uint8_t len;
    /* type of data */
    rt_uint8_t payload[LORA_PAYLOAD_LEN];
};
```

* 定义lora ACK包，主要包含以下部分：
  * 目标地址
  * 发送地址
  * 中转地址：不中转为0x00，故不能使用地址0x00；
  * 数据类型：数据,命令,ACK；
  * ACK
  * 数据校验码：通讯自带CRC校验，故不使用

| 说明 | 目标地址 | 发送地址 | 中转地址 | 数据类型 | ACK |
| :--: | :-----: | :-----: | :-----: | :-----: | :-----: |
| 类型 | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t | rt_uint8_t * N |
| 示例 | 0xA5 | 0xA5 | 0x00 | 0x02 | 0x00 |

``` C {.line-numbers highlight=[10, 12, 14, 19, 21, 23]}
#define LORA_TYPE_DATA   0x00
#define LORA_TYPE_CFG    0x01
#define LORA_TYPE_ACK    0x02
#define LORA_PAYLOAD_LEN 0x20

/* lora ack packet */
struct lora_ack_pkt
{
    /* destination address */
    rt_uint8_t dst_addr;
    /* source address */
    rt_uint8_t src_addr;
    /* transit address */
    rt_uint8_t mid_addr;
    /* type of data */
    rt_uint8_t type;
    /* data length */
    rt_uint8_t ack;
};
```

---

### 基本逻辑

* 串口接收：采用DMA加空闲中断的方式，当DMA半满及全满，或者传输线空闲时，都会触发中断，以便及时的完成数据接收。在UART_Start_Receive_DMA函数中，会注册DMA半满和全满调用函数。
  * 串口空闲中断时，会调用中断处理函数，主要是获取数据长度；
  * DMA半满中断会调用UART_DMARxHalfCplt，进而调用HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize / 2U);
  * DMA全满中断会调用UART_DMAReceiveCplt，进而调用HAL_UARTEx_RxEventCallback(huart, huart->RxXferSize);
  * 将数据放入lora_tx_rb环形缓冲区，对于是否强制覆盖，如果想保证实时性，建议执行，同步调整缓冲区大小，防止数据丢失。
  * 通过lora_tx_mq发送消息给lora发送程序（**一般配置指令不由串口传入，由其他配置程序进行操作，保证透传的简洁性**）。
``` C {.line-numbers highlight=[6, 19, 20, 25, 59, 67]}
void USART2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    uart_isr(&(uart_obj[UART2_INDEX].serial));

    /* leave interrupt */
    rt_interrupt_leave();
}

static void uart_isr(struct rt_serial_device *serial)
{
    else if ((uart->uart_dma_flag) && (__HAL_UART_GET_FLAG(&(uart->handle), UART_FLAG_IDLE) != RESET)
             && (__HAL_UART_GET_IT_SOURCE(&(uart->handle), UART_IT_IDLE) != RESET))
    {
        level = rt_hw_interrupt_disable();
        recv_total_index = serial->config.bufsz - __HAL_DMA_GET_COUNTER(&(uart->dma_rx.handle));
        recv_len = recv_total_index - uart->dma_rx.last_index;
        uart->dma_rx.last_index = recv_total_index;
        rt_hw_interrupt_enable(level);

        if (recv_len)
        {
            rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_DMADONE | (recv_len << 8));
        }
        __HAL_UART_CLEAR_IDLEFLAG(&uart->handle);
    }
}

void rt_hw_serial_isr(struct rt_serial_device *serial, int event)
{
    switch (event & 0xff)
    {
        case RT_SERIAL_EVENT_RX_DMADONE:
        {
            int length;
            rt_base_t level;

            /* get DMA rx length */
            length = (event & (~0xff)) >> 8;

            if (serial->config.bufsz == 0)
            {
                struct rt_serial_rx_dma* rx_dma;

                rx_dma = (struct rt_serial_rx_dma*) serial->serial_rx;
                RT_ASSERT(rx_dma != RT_NULL);

                RT_ASSERT(serial->parent.rx_indicate != RT_NULL);
                serial->parent.rx_indicate(&(serial->parent), length);
                rx_dma->activated = RT_FALSE;
            }
            else
            {
                /* disable interrupt */
                level = rt_hw_interrupt_disable();
                /* update fifo put index */
                rt_dma_recv_update_put_index(serial, length);
                /* calculate received total length */
                length = rt_dma_calc_recved_len(serial);
                /* enable interrupt */
                rt_hw_interrupt_enable(level);
                /* invoke callback */
                if (serial->parent.rx_indicate != RT_NULL)
                {
                    serial->parent.rx_indicate(&(serial->parent), length);
                }
            }
            break;
        }
    }
}
```

* 串口发送：从uart_tx_mq消息队列中获取本次消息的类型，如果是数据，则从uart_tx_rb取对应长度的数据，是配置指令，则转入对应的配置操作。
  * 使用DMA的方式进行数据发送，需要注意发送的数组需要用**static修饰**，放置临时变量被销毁了，导致数据发送错误。
  * 一般串口发送的优先级要低于接收，通过缓冲区来避免接收丢包。

* Lora发送：从lora_tx_mq消息队列中获取本次消息的类型，如果是数据，则从uart_tx_rb取对应长度的数据，是配置指令，则转入对应的配置操作，一般会需要将配置发送给lora接收端，对于是否发送后发送端就开始配置，还是得到响应后再配置，进行二次确认确认，后面详细讲解。
  * 定义一个静态变量wait_send_len保存等待发送的数据长度，用于将一个大数据包分次发送，并需要实现错误时的数据包；
  * 在Lora 接收超时和一个数据包接收完成时，进行数据的发送，由于只有一根天线，所以发送时丢失一些接收信息是必然的，可以采用协商机制或者发送前的检测机制，后续详细深入了解；
  * 如果接收端回复的不是接收OK，则进行数据重发，尝试3次后要进行错误提示。

* Lora接收：

**重点加粗**

*斜体*

~~删除线~~

---

列表:

* 无序列表
  * 嵌套无序列表
  * 嵌套无序列表
* 无序列表
* 无序列表

1. 有序列表 1
   1. 嵌套有序列表 1
   2. 嵌套有序列表 2
2. 有序列表 2
3. 有序列表 3

---

引用文本:

> 引用别人说的话
> 就这样写
> By. OrangeX4

---

这是 `行内代码` 语法.

代码块语法:

''' python
print("Hello, World!")
'''

请将 ' 替换成 `.

---

[超链接名称](链接地址)

![图片提示语](图片地址)

---

表格:

| 表头 | 表头 |
| ---- | ---- |
| 内容 | 内容 |
| 内容 | 内容 |

---

注释:

<!-- 你看不见我 -->