### `vector_table.S` 代码详细注释

这个文件定义了 ARM Cortex-M 处理器上电启动和处理异常所必须的**中断向量表**。这个表位于固件（Flash）的起始地址，是处理器硬件寻找启动入口和异常处理函数的地方。

```assembly
/*
 * Copyright (c) 2013-2015 Wind River Systems, Inc.
 * Copyright (c) 2020 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Populated vector table in ROM
 *
 * Vector table at the beginning of the image for starting system. The reset
 * vector is the system entry point, ie. the first instruction executed.
 *
 * The table is populated with all the system exception handlers. The NMI vector
 * must be populated with a valid handler since it can happen at any time. The
 * rest should not be triggered until the kernel is ready to handle them.
 */

// 包含头文件，定义工具链、链接器段等宏和符号
#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>
#include "vector_table.h"

_ASM_FILE_PROLOGUE // 汇编文件开始的宏定义

/*
 * Tell armclang that stack alignment are ensured.
 */
.eabi_attribute Tag_ABI_align_preserved, 1 // 告知 armclang 编译器栈对齐已由汇编代码保证

GDATA(z_main_stack) // 声明主线程栈的起始地址

// 将向量表放入 exc_vector_table 链接段，这是 Zephyr 默认的向量表位置
SECTION_SUBSEC_FUNC(exc_vector_table,_vector_table_section,_vector_table)

    /*
     * setting the _very_ early boot on the main stack allows to use memset
     * on the interrupt stack when CONFIG_INIT_STACKS is enabled before
     * switching to the interrupt stack for the rest of the early boot
     */
    // [0] 栈顶指针 (Initial SP Value)
    // 这是系统复位后 Main Stack Pointer (MSP) 的初始值。
    .word z_main_stack + CONFIG_MAIN_STACK_SIZE 

    // [1] 复位向量 (Reset Vector)
    // 系统复位后的第一个执行指令地址。
    .word z_arm_reset 

    // [2] 不可屏蔽中断 (NMI)
    .word z_arm_nmi 

    // [3] 硬件错误 (Hard Fault)
    .word z_arm_hard_fault

// 根据 ARM 架构版本（Baseline 或 Mainline）填充后续的系统异常向量

#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE) // ARMv6-M 或 ARMv8-M Baseline (如 Cortex-M0/M0+/M23)
    // [4] MPU Fault (保留，使用 HardFault)
    .word 0 
    // [5] Bus Fault (保留，使用 HardFault)
    .word 0 
    // [6] Usage Fault (保留，使用 HardFault)
    .word 0 
    // [7-10] 保留
    .word 0
    .word 0
    .word 0
    .word 0
    // [11] SVC (系统服务调用)
    .word z_arm_svc
    // [12] Debug Monitor (保留)
    .word 0 
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE) // ARMv7-M 或 ARMv8-M Mainline (如 Cortex-M3/M4/M33)
    // [4] 内存保护单元错误 (MPU Fault)
    .word z_arm_mpu_fault
    // [5] 总线错误 (Bus Fault)
    .word z_arm_bus_fault
    // [6] 使用错误 (Usage Fault)
    .word z_arm_usage_fault
#if defined(CONFIG_ARMV8_M_SE)
#if defined(CONFIG_ARM_SECURE_FIRMWARE)
    // [7] 安全错误 (Secure Fault) - 仅限 ARMv8-M Security Extension (SE) 安全固件
    .word z_arm_secure_fault
#else
    // [7] 伪异常 (Spurious Exception) - 非安全固件下的默认处理
    .word z_arm_exc_spurious 
#endif /* CONFIG_ARM_SECURE_FIRMWARE */
#else
    // [7] 保留
    .word 0 
#endif /* CONFIG_ARMV8_M_SE */
    // [8] 保留
    .word 0
    // [9] 保留
    .word 0
    // [10] 保留
    .word 0
    // [11] SVC (系统服务调用)
    .word z_arm_svc
    // [12] 调试监控 (Debug Monitor)
    .word z_arm_debug_monitor
#else
#error Unknown ARM architecture // 未知 ARM 架构，停止编译
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */
    
    // [13] 保留
    .word 0

#if defined(CONFIG_MULTITHREADING)
    // [14] PendSV (可悬起系统调用) - 用于上下文切换
    .word z_arm_pendsv
#else
    // [14] 无多线程时，使用伪异常处理
    .word z_arm_exc_spurious
#endif

#if defined(CONFIG_CPU_CORTEX_M_HAS_SYSTICK) // 如果 CPU 有 SysTick 定时器
#if defined(CONFIG_SYS_CLOCK_EXISTS) && \
    defined(CONFIG_CORTEX_M_SYSTICK_INSTALL_ISR)
    // [15] SysTick (系统滴答定时器) - 如果配置为系统时钟源，使用时钟中断服务例程
    .word sys_clock_isr 
#else
    // [15] SysTick - 如果未配置为系统时钟或未安装 ISR，使用伪异常处理
    .word z_arm_exc_spurious
#endif /* CONFIG_SYS_CLOCK_EXISTS && CONFIG_CORTEX_M_SYSTICK_INSTALL_ISR */
#else
    // [15] SysTick - CPU 不支持 SysTick 时，使用保留值 0
    .word 0
#endif /* CONFIG_CPU_CORTEX_M_HAS_SYSTICK */

// 注释结束：向量表在此处结束系统异常部分，后续为芯片（IRQ）中断向量，但未在此文件中列出。
```

### 关键启动点总结

1.  **初始堆栈指针 (MSP):** 在 `[0x00]` 处定义为 `z_main_stack + CONFIG_MAIN_STACK_SIZE`。
2.  **复位处理函数 (Reset Handler):** 在 `[0x04]` 处定义为 **`z_arm_reset`**。这是 CPU 在上电后执行的**第一条指令**的地址。
3.  **启动代码的衔接：** `vector_table.S` 将控制权转交给 `z_arm_reset`，而 `z_arm_reset` 则在 **`reset.S`** 中定义，负责执行底层的汇编初始化，最终跳转到 C 语言启动函数 `__start`（或 `z_cstart`）。
