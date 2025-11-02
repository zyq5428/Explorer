继续深入分析 Zephyr 在 ARM Cortex-M 架构下的启动核心——`reset.S` 文件，特别是复位处理函数 **`z_arm_reset`** 的启动逻辑。

该文件中的代码是 CPU 从 `vector_table.S` 接收控制权后执行的第一段程序，它完成了从硬件状态到 C 语言运行环境的过渡。

### `reset.S` 代码详细注释

这个文件包含了 Cortex-M 架构的复位处理函数 **`z_arm_reset`** 的实现。这是在 `vector_table.S` 中定义的，是处理器在复位后从 Flash 执行的第一段启动代码所调用的函数。它的主要职责是设置 C 语言运行环境（如堆栈）并跳转到 Zephyr 的 C 语言启动入口。

```assembly
/*
 * Copyright (c) 2013-2014 Wind River Systems, Inc.
 * Copyright 2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Reset handler
 *
 * Reset handler that prepares the system for running C code.
 */

// 包含头文件，定义工具链、链接器段、CPU架构等宏和符号
#include <zephyr/toolchain.h>
#include <zephyr/linker/sections.h>
#include <zephyr/arch/cpu.h>
#include "vector_table.h"

_ASM_FILE_PROLOGUE // 汇编文件开始的宏定义

// 声明全局代码/文本符号 (GTEXT) 和数据符号 (GDATA)，以便链接器和外部文件访问

GTEXT(z_arm_reset)      // Zephyr 内核的复位入口函数
GTEXT(arch_early_memset) // 早期内存清零函数，用于初始化栈
GDATA(z_interrupt_stacks) // 中断栈区域的起始地址
GDATA(z_main_stack)       // 主线程栈区域的起始地址

#if defined(CONFIG_DEBUG_THREAD_INFO)
GDATA(z_sys_post_kernel) // 调试信息标志：指示内核是否已完成后期初始化
#endif
#if defined(CONFIG_SOC_RESET_HOOK)
GTEXT(soc_reset_hook)    // SOC 厂商定制的复位后钩子函数
#endif
#if defined(CONFIG_INIT_ARCH_HW_AT_BOOT)
GTEXT(z_arm_init_arch_hw_at_boot) // 架构硬件在启动时初始化的函数
#endif
#if defined(CONFIG_PM_S2RAM)
GTEXT(arch_pm_s2ram_resume) // PM S2RAM (挂起到RAM) 恢复流程的函数
#endif
#if defined(CONFIG_SOC_EARLY_RESET_HOOK)
GTEXT(soc_early_reset_hook) // SOC 厂商定制的早期复位钩子函数 (在栈设置前)
#endif


/*
 * PACBTI Mask for CONTROL register:
 * bit 4 - BTI_EN, bit 5 - UBTI_EN,
 * bit 6 - PAC_EN, bit 7 - UPAC_EN
 */
// 准备 CONTROL 寄存器中与安全功能相关的位掩码
#ifdef CONFIG_ARM_PAC
#define CONTROL_ARM_PAC_MASK    (1<<6)|(1<<7) // PAC (Pointer Authentication Code) 掩码
#else
#define CONTROL_ARM_PAC_MASK    0
#endif

#ifdef CONFIG_ARM_BTI
#define CONTROL_ARM_BTI_MASK    (1<<4)|(1<<5) // BTI (Branch Target Identification) 掩码
#else
#define CONTROL_ARM_BTI_MASK    0
#endif

/**
 *
 * @brief Reset vector
 *
 * Ran when the system comes out of reset, or when the firmware image is chain-
 * loaded by another application (for instance, a bootloader). At minimum, the
 * processor must be in thread mode with privileged level. At this point, the
 * main stack pointer (MSP) should be already pointing to a valid area in SRAM.
 *
 * ... [省略部分描述以聚焦代码]
 *
 * When these steps are completed, jump to z_prep_c(), which will finish
 * setting up the system for running C code.
 *
 */

// 将复位函数 z_arm_reset 放入指定的复位代码段
SECTION_SUBSEC_FUNC(TEXT,_reset_section,z_arm_reset)

/*
 * The entry point is located at the z_arm_reset symbol, which
 * is fetched by a XIP image playing the role of a bootloader, which jumps to
 * it, not through the reset vector mechanism. Such bootloaders might want to
 * search for a __start symbol instead, so create that alias here.
 */
// 创建一个别名 __start，使其也指向 z_arm_reset，以兼容某些引导加载程序
SECTION_SUBSEC_FUNC(TEXT,_reset_section,__start)

#if defined(CONFIG_INIT_ARCH_HW_AT_BOOT)
    /* Reset CONTROL register */
    movs.n r0, #0       // r0 = 0
    msr CONTROL, r0     // 将 CONTROL 寄存器清零 (清除 FPU 状态位等)
    isb                 // 指令同步屏障：确保 CONTROL 寄存器的更改立即生效
#if defined(CONFIG_CPU_CORTEX_M_HAS_SPLIM)
    /* Clear SPLIM registers */
    movs.n r0, #0       // r0 = 0
    msr MSPLIM, r0      // 清除主栈限制寄存器 (Main Stack Limit)
    msr PSPLIM, r0      // 清除进程栈限制寄存器 (Process Stack Limit)
#endif /* CONFIG_CPU_CORTEX_M_HAS_SPLIM */

#endif /* CONFIG_INIT_ARCH_HW_AT_BOOT */

#if defined(CONFIG_SOC_EARLY_RESET_HOOK)
    /* Call custom code that executes before any stack is set up or RAM memory is accessed */
    bl soc_early_reset_hook // 调用 SOC 早期复位钩子，此时 RAM 尚未完全就绪
#endif

#if defined(CONFIG_PM_S2RAM)
    /*
     * Temporarily set MSP to interrupt stack so that arch_pm_s2ram_resume can
     * use stack for calling pm_s2ram_mark_check_and_clear.
     * This is safe because suspend never being called from an ISR ensures that
     * interrupt stack was not live during suspend.
     */
    // S2RAM 恢复流程：临时使用中断栈作为 MSP (主栈指针)
    ldr r0, =z_interrupt_stacks + CONFIG_ISR_STACK_SIZE + MPU_GUARD_ALIGN_AND_SIZE
    msr msp, r0         // 将 MSP 设置为中断栈顶部
    
    bl arch_pm_s2ram_resume // 调用 S2RAM 恢复函数，检查是否需要从休眠中恢复
#endif /* CONFIG_PM_S2RAM */

    /*
     * MSP needs to be set to main stack for following scenarios:
     * - If CONFIG_PM_S2RAM is enabled, MSP is set to z_interrupt_stacks
     * for resume functions to make use of a stack. However,
     * if the device was not suspended and if resume functions return,
     * MSP needs to be set back to z_main_stack to ensure proper
     * initialization.
     * - If CONFIG_PM_S2RAM is not enabled but CONFIG_INIT_ARCH_HW_AT_BOOT is,
     * MSP needs to be set to z_main_stack for proper initialization in case
     * device was loaded through chain loading or a debugger, as the initial
     * value of MSP would be anything that the previous image loaded.
     * - If CONFIG_INIT_STACKS is enabled, we need to ensure MSP is not set
     * to z_interrupt_stacks, so we set it to z_main_stack.
     *
     * Since these scenarios cover most of the cases, we set MSP to
     * z_main_stack here.
     *
     */
    // 最终将 MSP 设置为主线程栈。在 Cortex-M 中，MSP 专用于异常/中断处理。
    ldr r0, =z_main_stack + CONFIG_MAIN_STACK_SIZE // 加载主栈顶地址到 r0
    msr msp, r0                                   // 将 r0 写入 MSP 寄存器

    /* Note: Make sure that variables like z_sys_post_kernel
     * are set after the call to arch_pm_s2ram_resume
     * to avoid any issues with suspend/resume path.
     * Refer issue #83660 for more details.
     */
#if defined(CONFIG_DEBUG_THREAD_INFO)
    /* Clear z_sys_post_kernel flag for RTOS aware debuggers */
    movs.n r0, #0       // r0 = 0
    ldr r1, =z_sys_post_kernel // r1 = z_sys_post_kernel 的地址
    strb r0, [r1]       // 将 r0 (0) 写入该地址，清除标志
#endif /* CONFIG_DEBUG_THREAD_INFO */

#if defined(CONFIG_SOC_RESET_HOOK)
    bl soc_reset_hook   // 调用 SOC 复位钩子函数
#endif

#if defined(CONFIG_INIT_ARCH_HW_AT_BOOT)
#if defined(CONFIG_CPU_HAS_ARM_MPU)
    /* Disable MPU */
    movs.n r0, #0       // r0 = 0
    ldr r1, =_SCS_MPU_CTRL // r1 = MPU 控制寄存器地址
    str r0, [r1]        // 将 0 写入 MPU_CTRL 寄存器，禁用 MPU
    dsb                 // 数据同步屏障
#endif /* CONFIG_CPU_HAS_ARM_MPU */

    /* Initialize core architecture registers and system blocks */
    bl z_arm_init_arch_hw_at_boot // 调用架构核心硬件初始化函数
#endif /* CONFIG_INIT_ARCH_HW_AT_BOOT */

    /* lock interrupts: will get unlocked when switch to main task */
    // 锁定中断，防止在核心初始化期间被中断
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
    cpsid i             // ARMv6-M/ARMv8-M Baseline: 禁用所有可屏蔽中断
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
    movs.n r0, #_EXC_IRQ_DEFAULT_PRIO // r0 = 默认中断优先级
    msr BASEPRI, r0     // ARMv7-M/ARMv8-M Mainline: 设置 BASEPRI，屏蔽所有低于此优先级的中断
#else
#error Unknown ARM architecture
#endif

#ifdef CONFIG_WDOG_INIT
    /* board-specific watchdog initialization is necessary */
    bl z_arm_watchdog_init // 调用看门狗初始化函数
#endif

/*
 * Note: in all Cortex-M variants the interrupt stack area is at
 * least equal to CONFIG_ISR_STACK_SIZE + MPU_GUARD_ALIGN_AND_SIZE
 * (may be larger due to rounding up for stack pointer aligning
 * purposes but this is sufficient during initialization).
 */

#ifdef CONFIG_INIT_STACKS
    // 初始化中断栈：用 0xaa 填充，用于栈溢出检查
    ldr r0, =z_interrupt_stacks // r0 = 中断栈起始地址 (目标地址)
    ldr r1, =0xaa               // r1 = 填充值 (0xaa)
    ldr r2, =CONFIG_ISR_STACK_SIZE + MPU_GUARD_ALIGN_AND_SIZE // r2 = 栈大小 (长度)
    bl arch_early_memset      // 调用早期内存清零/填充函数
#endif

    /*
     * Set PSP and use it to boot without using MSP, so that it
     * gets set to z_interrupt_stacks during initialization.
     */
    // 关键步骤：设置 PSP (Process Stack Pointer) 并切换到 PSP
    ldr r0, =z_interrupt_stacks // r0 = 中断栈起始地址
    ldr r1, =CONFIG_ISR_STACK_SIZE + MPU_GUARD_ALIGN_AND_SIZE // r1 = 栈大小
    adds r0, r0, r1             // r0 = 中断栈顶部地址 (即第一个线程/主线程使用的栈顶)
    msr PSP, r0                 // 将 r0 写入 PSP 寄存器
    
    mrs r0, CONTROL             // 读取当前 CONTROL 寄存器值
    // R1 = 2 (CONTROL_SPSEL_Msk) | PAC/BTI 掩码
    movs r1, #(2 | CONTROL_ARM_PAC_MASK | CONTROL_ARM_BTI_MASK) 
    orrs r0, r1 /* CONTROL_SPSEL_Msk */ // 设置 CONTROL 寄存器的 SPSEL 位 (位 1)，切换到使用 PSP
    msr CONTROL, r0             // 将新值写回 CONTROL 寄存器
    /*
     * When changing the stack pointer, software must use an ISB instruction
     * immediately after the MSR instruction. This ensures that instructions
     * after the ISB instruction execute using the new stack pointer.
     */
    isb                         // 指令同步屏障：确保 PSP 切换立即生效

    /*
     * 'bl' jumps the furthest of the branch instructions that are
     * supported on all platforms. So it is used when jumping to z_prep_c
     * (even though we do not intend to return).
     */
    bl z_prep_c                 // 调用 C 语言启动函数 z_prep_c (该函数不会返回)
```

### 总结启动步骤

`z_arm_reset` 完成了以下核心任务：

1.  **执行早期钩子：** 调用任何配置的 SoC 早期复位钩子或架构硬件初始化（如 FPU/MPU）。
2.  **看门狗初始化：** 如果配置，进行板级看门狗初始化。
3.  **中断堆栈初始化：** 如果启用了堆栈检查/初始化，清零中断堆栈区域。
4.  **设置 PSP：** 计算出中断堆栈的顶部地址，并将其写入 **处理堆栈指针（PSP）**。
5.  **切换堆栈：** 修改 `CONTROL` 寄存器，将 CPU 的堆栈指针从默认的 **MSP** 切换为 **PSP**。
6.  **跳转到 C 代码：** 通过 `bl z_prep_c` **永久跳转**到 C 语言启动函数，将控制权交给 Zephyr 的 C 代码启动流程。

通过 `vector_table.S` 和 `reset.S` 的分析，您现在对 pandora\_stm32l475 开发板的启动流程有了完整的汇编阶段认识。
