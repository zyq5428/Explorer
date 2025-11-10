# RT-Thread-Studio官方模板应用

## 操作环境

    我的操作环境是
    * Windows 10 专业版
    * 潘多拉 STM32L475 开发板
    * RT-Thread Studio 版本: 2.2.9
    * STM32L4SDK 0.2.4版本(2023-09-07)
    * RT-thread latest版本

目前我实现了lvgl官方示例的使用，现在我希望实现一个简单功能：通过一个线程定时切换led状态，然后lvgl的按钮接收状态，并在一个按钮上显示ON或者OFF
## 创建RT-Thread项目

    使用latest版本的RT-Thread，使用UART1（PA9,PA10）作为控制台串口。
![创建RT-Thread项目](./assets/创建RT-Thread项目.png)

### 在进行对项目进行操作前，配置一下git，以便对项目可以进行管控跟踪

    首先，在github新建一个仓库，注意最好不要进行初始化配置，然后和本地仓库进行关联：
    以下是github官方给的建议操作：
![github仓库对接](./assets/github仓库对接.png)
![github首次commit](./assets/github首次commit.png)

## 配置链接脚本

    MCU的内存映射图：
![MCU的内存映射图](./assets/MCU的内存映射图.png)

    Memory map for STM32L47x/L48x devices：
![STM32L47x_Embedded_SRAM](./assets/STM32L47x_Embedded_SRAM.png)

    lds链接脚本：
![lds链接脚本](./assets/lds链接脚本.png)


## 首次调试

    对项目进行重新构建，会发现官方模板无法正确编译，下面一一进行报错分析及解决。

### FPU报错

> 报错提示：#error "Compiler generates FPU instructions for a device without an FPU (check __FPU_PRESENT)"
> 问题分析：查询STM32WLE5手册，会发现该芯片是无FPU版本：
![STM32WLE5CBU6芯片框图](./assets/STM32WLE5CBU6芯片框图.png)
> 解决方案：在工程设置中关闭FPU功能，即可编译通过：
![工程关闭FPU](./assets/工程关闭FPU.png)

### 终端无输出

> 报错提示：串口终端没有信息打印。
> 问题分析：因为设置的打印串口为UART2，和自带的配置文件对不上，需要重新配置CubeMX。
> 解决方案：在CubeMX中设置好串口，时钟信息：
  > 选择外部晶振，并选择好Voltage scaling range和Flash Wait states (WS)，因为我要配置48MHz,所以根据参考手册中的说明，选择的Range 1和2 WS (3 HCLK cycles);
![Voltage_scaling_range](./assets/Voltage_scaling_range.png)
![Flash_Wait_states](./assets/Flash_Wait_states.png)
![晶振配置](./assets/晶振配置.png)
  > 开启UART，RTC等外设;
![配置多个外设](./assets/配置多个外设.png)
  > 配置时钟;
![时钟配置](./assets/时钟配置.png)
  > 生成代码后仅保留如下文件，并将cubemx添加到包含文件中：
![cubemx有效文件](./assets/cubemx有效文件.png)
  > 在cubemx下配置Kconfig，以便使能相应的芯片功能和外设功。

## LVGL组件

### lvgl软件包配置

    lvgl软件包配置
![lvgl软件包配置](./assets/lvgl软件包配置.png)
    LCD使能lvgl
![LCD使能lvgl](./assets/LCD使能lvgl.png)

### RT_Thread中LVGL的调用逻辑

    在lvgl->env_support->rt-thread中进行初始化，并开始调用lv_user_gui_init()
![lvgl调用逻辑](./assets/lvgl调用逻辑.png)
![lv_user_gui_init](./assets/lv_user_gui_init.png)

    使用lvgl官方的示例：
![使能官方示例](./assets/使能官方示例.png)
![lvgl官方示例使用](./assets/lvgl官方示例使用.png)

``` {.line-numbers highlight=[8]}
#include <lvgl.h>

void lv_user_gui_init(void)
{
    /* display demo; you may replace with your LVGL application at here */
//    extern void lv_demo_calendar(void);
//    lv_demo_calendar();
     extern void lv_example_bar_3(void);
     lv_example_bar_3();
}
```