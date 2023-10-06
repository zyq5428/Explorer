# STM32WL配置RTC

## STM32WL配置软件模拟RTC

* 组件设置中开启软件模拟RTC
![组件开启软件模拟RTC](./assets/组件开启软件模拟RTC.png)

* 启用RTC设备示例
![启用RTC设备示例](./assets/启用RTC设备示例.png)
``` c {.line-numbers highlight=[1]}
#include <rtthread.h>
#include <rtdevice.h>

static int rtc_sample(int argc, char *argv[])
{
    rt_err_t ret = RT_EOK;
    time_t now;

    /* 设置日期 */
    ret = set_date(2018, 12, 3);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC date failed\n");
        return ret;
    }

    /* 设置时间 */
    ret = set_time(11, 15, 50);
    if (ret != RT_EOK)
    {
        rt_kprintf("set RTC time failed\n");
        return ret;
    }

    /* 延时3秒 */
    rt_thread_mdelay(3000);

    /* 获取时间 */
    now = time(RT_NULL);
    rt_kprintf("%s\n", ctime(&now));

    return ret;
}

msh >rtc_sample
Mon Dec  3 11:15:53 2018
```

## 开启ulog日志
![开启ulog日志C](./assets/开启ulog日志.png)
``` {.line-numbers highlight=[5]}
 \ | /
- RT -     Thread Operating System
 / | \     4.1.1 build Oct  6 2023 23:27:12
 2006 - 2022 Copyright by RT-Thread team
msh >01-01 08:00:00 D/drv.usart: uart1 dma config start
01-01 08:00:00 D/drv.usart: uart1 dma RX instance: 40020058
01-01 08:00:00 D/drv.usart: uart1 dma config done
01-01 08:00:00 D/drv.usart: uart1 dma config start
01-01 08:00:00 D/drv.usart: uart1 dma TX instance: 40020044
01-01 08:00:00 D/drv.usart: uart1 dma config done
```