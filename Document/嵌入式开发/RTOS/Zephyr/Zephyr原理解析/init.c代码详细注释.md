init.c文件是 Zephyr 内核 C 语言初始化阶段的核心，负责定义内核对象、初始化线程、并按预定级别运行所有的初始化函数。

### `init.c` 核心功能总结

#### 1. 关键数据结构和线程定义

* **内核实例 (`_kernel`)**: 文件中定义了唯一的 `struct z_kernel _kernel` 实例，它是 Zephyr 内核全局状态的持有者。
* **主线程和栈**: 定义了用于启动和运行应用程序 `main()` 函数的**主线程** (`z_main_thread`) 及其固定栈空间 (`z_main_stack`)。
* **中断栈**: 定义了中断服务例程使用的中断栈空间数组 (`z_interrupt_stacks`)。
* **空闲线程**: 在多线程配置 (`CONFIG_MULTITHREADING`) 下，为每个 CPU 核心定义了空闲线程 (`z_idle_threads`) 及其栈空间。

#### 2. 初始化流程入口：`z_cstart` 和 `bg_thread_main`

系统在完成汇编级别的初始化（如清零 `.bss` 段）后，会进入 `z_cstart()`（通常在 `init.c` 或相关文件中调用）来启动 C 语言初始化流程。

`bg_thread_main()` 是**主线程**的入口函数，它负责执行后续所有的内核和应用初始化，直至调用用户定义的 `main()` 函数。

**`bg_thread_main` 的主要步骤包括：**

1.  **内存管理初始化**: 如果启用了 MMU，则调用 `z_mem_manage_init()`。
2.  **设置状态**: 设置 `z_sys_post_kernel = true`，表示已进入内核后初始化阶段。
3.  **运行 POST_KERNEL 级别**: 调用 `z_sys_init_run_level(INIT_LEVEL_POST_KERNEL)`，初始化大部分驱动和内核服务。
4.  **硬件/板级钩子**: 调用 `soc_late_init_hook()` 和 `board_late_init_hook()` 进行板级最终初始化。
5.  **启动横幅**: 调用 `boot_banner()` 显示启动信息。
6.  **运行 APPLICATION 级别**: 调用 `z_sys_init_run_level(INIT_LEVEL_APPLICATION)`，运行应用程序级别的初始化代码。
7.  **启动静态线程**: 调用 `z_init_static_threads()` 启动所有在编译时定义的静态线程。
8.  **调用 `main()`**: 最后，调用用户应用程序的 `main()` 函数。
9.  **结束**: `main()` 返回后，清除主线程的 essential 标志，并处理代码覆盖率数据转储。

#### 3. 分级初始化机制

文件定义了一个初始化级别枚举 `enum init_level`，确保系统组件以正确的顺序初始化：

| 级别 | 宏定义起点 | 描述 |
| :--- | :--- | :--- |
| **EARLY** | `__init_EARLY_start` | 最早的 C 语言初始化，例如基本的 SOC/硬件设置。
| **PRE_KERNEL_1** | `__init_PRE_KERNEL_1_start` | 在内核调度器启动前的第一阶段初始化。
| **PRE_KERNEL_2** | `__init_PRE_KERNEL_2_start` | 在内核调度器启动前的第二阶段初始化。
| **POST_KERNEL** | `__init_POST_KERNEL_start` | 在主线程启动后运行，是大多数驱动和 OS 服务的初始化阶段。
| **APPLICATION** | `__init_APPLICATION_start` | 应用程序逻辑之前的最终初始化阶段。
| **SMP** | `__init_SMP_start` | 多核（SMP）系统专用的初始化。

**核心函数 `z_sys_init_run_level()`** 通过遍历链接器脚本定义的 `init_entry` 数组，依次调用属于当前级别的初始化函数。对于设备初始化，它会调用 `do_device_init()`（除非设备被标记为延迟初始化）。

#### 4. 静态线程初始化

`z_init_static_threads()` 函数（在 `CONFIG_MULTITHREADING` 下启用）负责：

* 遍历所有静态线程数据 (`_static_thread_data`)，调用 `z_setup_new_thread()` 进行线程设置。
* 如果启用用户空间 (`CONFIG_USERSPACE`)，它会遍历 `k_object_assignment` 部分，为线程授予内核对象的访问权限。
* 在调度器锁定的情况下，遍历并调度所有非 `K_FOREVER` 延迟的静态线程开始运行。

### `init.c` - Zephyr 内核初始化模块详细注释

```c
/*
 * Copyright (c) 2010-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Kernel initialization module
 *
 * This module contains routines that are used to initialize the kernel.
 */

// 包含各种必要的头文件，涉及内核结构、调度、设备、日志、追踪、内存等
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <offsets_short.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/debug/stack.h>
#include <zephyr/random/random.h>
#include <zephyr/linker/sections.h>
#include <zephyr/toolchain.h>
#include <zephyr/kernel_structs.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/linker/linker-defs.h>
#include <zephyr/platform/hooks.h> // 平台/SoC 钩子函数
#include <ksched.h>
#include <kthread.h>
#include <ipi.h>
#include <zephyr/sys/dlist.h>
#include <kernel_internal.h>
#include <zephyr/drivers/entropy.h>
#include <zephyr/logging/log_ctrl.h>
#include <zephyr/tracing/tracing.h>
#include <zephyr/debug/gcov.h>
#include <kswap.h>
#include <zephyr/timing/timing.h>
#include <zephyr/logging/log.h>
#include <zephyr/internal/syscall_handler.h>
#include <zephyr/arch/common/init.h>

// 注册内核日志模块，使用配置的日志级别
LOG_MODULE_REGISTER(os, CONFIG_KERNEL_LOG_LEVEL);

/* the only struct z_kernel instance */
// 1. 内核核心结构体：整个 Zephyr 内核的唯一运行时数据结构。
//    __pinned_bss 宏将其放置在固定的、清零的 BSS 内存区域。
__pinned_bss
struct z_kernel _kernel;

#ifdef CONFIG_PM
// 2. 如果启用了电源管理 (PM)，记录当前活跃的 CPU 数量。
__pinned_bss atomic_t _cpus_active;
#endif

/* init/main and idle threads */
// 3. 定义主线程的堆栈空间。
K_THREAD_PINNED_STACK_DEFINE(z_main_stack, CONFIG_MAIN_STACK_SIZE);
// 4. 定义主线程的 k_thread 结构体实例。
struct k_thread z_main_thread;

#ifdef CONFIG_MULTITHREADING
// 5. 如果启用多线程，定义多核环境下的闲置线程结构体数组（每核心一个）。
__pinned_bss
struct k_thread z_idle_threads[CONFIG_MP_MAX_NUM_CPUS];

// 6. 定义多核环境下的闲置线程堆栈数组。
static K_KERNEL_PINNED_STACK_ARRAY_DEFINE(z_idle_stacks,
                                          CONFIG_MP_MAX_NUM_CPUS,
                                          CONFIG_IDLE_STACK_SIZE);

/**
 * @brief 初始化所有静态定义的线程
 *
 * 遍历链接器脚本中定义的静态线程数据段，对每个线程进行设置和启动。
 */
static void z_init_static_threads(void)
{
    // 遍历链接器在 _static_thread_data 段中收集的所有静态线程数据。
    STRUCT_SECTION_FOREACH(_static_thread_data, thread_data) {
        // 7. 调用内核 API 设置新线程的属性（结构体、堆栈、入口、参数、优先级、选项、名称）。
        z_setup_new_thread(
            thread_data->init_thread,
            thread_data->init_stack,
            thread_data->init_stack_size,
            thread_data->init_entry,
            thread_data->init_p1,
            thread_data->init_p2,
            thread_data->init_p3,
            thread_data->init_prio,
            thread_data->init_options,
            thread_data->init_name);

        // 8. 将线程数据结构体的地址存入线程自身的 init_data 字段。
        thread_data->init_thread->init_data = thread_data;
    }

#ifdef CONFIG_USERSPACE
    // 9. 如果启用用户空间，遍历 k_object_assignment 段，为线程分配对内核对象的访问权限。
    STRUCT_SECTION_FOREACH(k_object_assignment, pos) {
        for (int i = 0; pos->objects[i] != NULL; i++) {
            k_object_access_grant(pos->objects[i],
                                  pos->thread);
        }
    }
#endif /* CONFIG_USERSPACE */

    /*
     * Non-legacy static threads may be started immediately or
     * after a previously specified delay. ...
     * Take a sched lock to prevent them from running
     * until they are all started.
     */
    // 10. 锁定调度器，防止任何线程在启动流程完成前被调度运行。
    k_sched_lock();
    // 再次遍历所有静态线程数据。
    STRUCT_SECTION_FOREACH(_static_thread_data, thread_data) {
        k_timeout_t init_delay = Z_THREAD_INIT_DELAY(thread_data);

        // 11. 如果线程的启动延迟不为 K_FOREVER，则将其加入就绪队列/定时器。
        if (!K_TIMEOUT_EQ(init_delay, K_FOREVER)) {
            thread_schedule_new(thread_data->init_thread,
                                init_delay);
        }
    }
    // 12. 解锁调度器。此时，除了被延迟的线程外，其他所有线程已准备就绪，但在当前函数返回前仍不会运行。
    k_sched_unlock();
}
#else
// 13. 如果未启用多线程，z_init_static_threads 被定义为空宏。
#define z_init_static_threads() do { } while (false)
#endif /* CONFIG_MULTITHREADING */

// 14. 外部声明（来自链接器脚本）：初始化函数入口点的起始和结束符号。
//    这些符号定义了 Zephyr 的**分级初始化**机制。
extern const struct init_entry __init_start[];
extern const struct init_entry __init_EARLY_start[];
extern const struct init_entry __init_PRE_KERNEL_1_start[];
extern const struct init_entry __init_PRE_KERNEL_2_start[];
extern const struct init_entry __init_POST_KERNEL_start[];
extern const struct init_entry __init_APPLICATION_start[];
extern const struct init_entry __init_end[];

// 15. 初始化级别枚举：定义了初始化函数运行的先后顺序。
enum init_level {
    INIT_LEVEL_EARLY = 0,        // 最早期的初始化，通常在 C 运行时初始化后立即运行。
    INIT_LEVEL_PRE_KERNEL_1,     // 内核前阶段 1：基础硬件初始化、中断控制器。
    INIT_LEVEL_PRE_KERNEL_2,     // 内核前阶段 2：大部分驱动和设备初始化。
    INIT_LEVEL_POST_KERNEL,      // 内核后阶段：依赖于完整调度器的服务（如文件系统、网络）。
    INIT_LEVEL_APPLICATION,      // 应用程序阶段：用户应用程序相关的初始化。
#ifdef CONFIG_SMP
    INIT_LEVEL_SMP,              // 对称多处理 (SMP) 阶段：启动次级 CPU。
#endif /* CONFIG_SMP */
};

#ifdef CONFIG_SMP
extern const struct init_entry __init_SMP_start[];
#endif /* CONFIG_SMP */

/*
 * storage space for the interrupt stack
 *
 * Note: This area is used as the system stack during kernel initialization,
 * since the kernel hasn't yet set up its own stack areas. ...
 */
// 16. 定义中断堆栈数组：用于中断处理，但在内核初始化早期也被用作**临时系统堆栈**。
K_KERNEL_PINNED_STACK_ARRAY_DEFINE(z_interrupt_stacks,
                                   CONFIG_MP_MAX_NUM_CPUS,
                                   CONFIG_ISR_STACK_SIZE);

// 17. 外部声明闲置线程的入口函数（在 kthread.c 中定义）。
extern void idle(void *unused1, void *unused2, void *unused3);

// ... [省略 CONFIG_OBJ_CORE_SYSTEM 相关的对象核心和统计信息结构体定义] ...

__pinned_bss
// 18. 标志位：指示系统是否已进入 POST_KERNEL 阶段（即调度器已启动）。
bool z_sys_post_kernel;

// 19. 外部声明设备初始化函数（在 device.c 中定义）。
extern int do_device_init(const struct device *dev);

/**
 * @brief 初始化所有静态设备的状态对象
 *
 * 遍历所有设备实例，并调用 k_object_init 对其进行初始化（通常是零初始化和核心链接）。
 */
static void z_device_state_init(void)
{
    STRUCT_SECTION_FOREACH(device, dev) {
        k_object_init(dev);
    }
}

/**
 * @brief 执行给定初始化级别中的所有初始化函数
 *
 * @param level 要运行的初始化级别。
 */
static void z_sys_init_run_level(enum init_level level)
{
    // 20. 静态数组：存储所有初始化级别的链接器起始符号地址。
    static const struct init_entry *levels[] = {
        __init_EARLY_start,
        __init_PRE_KERNEL_1_start,
        __init_PRE_KERNEL_2_start,
        __init_POST_KERNEL_start,
        __init_APPLICATION_start,
#ifdef CONFIG_SMP
        __init_SMP_start,
#endif /* CONFIG_SMP */
        /* End marker */
        __init_end,
    };
    const struct init_entry *entry;

    // 21. 遍历当前级别起始地址 (levels[level]) 到下一个级别起始地址 (levels[level+1]) 之间的所有条目。
    for (entry = levels[level]; entry < levels[level+1]; entry++) {
        const struct device *dev = entry->dev;
        int result = 0;

        sys_trace_sys_init_enter(entry, level); // 追踪：进入初始化函数
        if (dev != NULL) {
            // 22. 如果是设备（`dev != NULL`）且未标记为延迟初始化 (`DEVICE_FLAG_INIT_DEFERRED`)
            if ((dev->flags & DEVICE_FLAG_INIT_DEFERRED) == 0U) {
                result = do_device_init(dev); // 调用设备初始化函数
            }
        } else {
            // 23. 否则（普通系统初始化函数），直接调用函数指针。
            result = entry->init_fn();
        }
        sys_trace_sys_init_exit(entry, level, result); // 追踪：退出初始化函数
    }
}

/* defined in banner.c */
extern void boot_banner(void); // 外部声明：打印系统启动信息。

// ... [省略 CONFIG_STATIC_INIT_GNU 相关的 z_static_init_gnu 函数定义] ...

/**
 * @brief 内核背景（主）线程的主线函数
 *
 * 该例程完成剩余的内核初始化，然后调用应用程序的 main() 函数。
 * **这是内核启动后第一个被调度的线程。**
 */
__boot_func
static void bg_thread_main(void *unused1, void *unused2, void *unused3)
{
    ARG_UNUSED(unused1);
    ARG_UNUSED(unused2);
    ARG_UNUSED(unused3);

#ifdef CONFIG_MMU
    // 24. 如果启用了 MMU，初始化内存管理子系统（必须在多线程环境中进行）。
    z_mem_manage_init();
#endif /* CONFIG_MMU */
    // 25. 标记内核已完成核心启动阶段，进入 POST_KERNEL 阶段。
    z_sys_post_kernel = true;

#if CONFIG_IRQ_OFFLOAD
    arch_irq_offload_init(); // 架构特定的中断卸载初始化。
#endif
    // 26. 运行 POST_KERNEL 级别的初始化，此时调度器已启动。
    z_sys_init_run_level(INIT_LEVEL_POST_KERNEL);

    // 27. 调用 SoC 和板级后期的初始化钩子。
    soc_late_init_hook();

    board_late_init_hook();

// ... [省略 CONFIG_STACK_POINTER_RANDOM 相关的代码] ...
    // 28. 打印 Zephyr 启动 Banner。
    boot_banner();

#ifdef CONFIG_STATIC_INIT_GNU
    z_static_init_gnu(); // 运行额外的 C++ 静态构造函数初始化（如果启用）。
#endif /* CONFIG_STATIC_INIT_GNU */

    /* Final init level before app starts */
    // 29. 运行 APPLICATION 级别的初始化，这是用户 main() 之前最后执行的初始化。
    z_sys_init_run_level(INIT_LEVEL_APPLICATION);

    // 30. 初始化并启动所有 K_THREAD_DEFINE 定义的静态线程。
    z_init_static_threads();

// ... [省略 CONFIG_KERNEL_COHERENCE 相关的代码] ...

#ifdef CONFIG_SMP
    if (!IS_ENABLED(CONFIG_SMP_BOOT_DELAY)) {
        z_smp_init(); // 启动对称多处理（SMP）次级 CPU。
    }
    // 31. 运行 SMP 级别的初始化。
    z_sys_init_run_level(INIT_LEVEL_SMP);
#endif /* CONFIG_SMP */

#ifdef CONFIG_MMU
    z_mem_manage_boot_finish(); // MMU 启动完成后的收尾工作。
#endif /* CONFIG_MMU */

    // 32. 调用用户应用程序的 main() 函数。
#ifdef CONFIG_BOOTARGS
    extern int main(int, char **);
    extern char **prepare_main_args(int *argc);

    int argc = 0;
    char **argv = prepare_main_args(&argc);
    (void)main(argc, argv);
#else
    extern int main(void);
    (void)main(); // 无参数调用 main()。
#endif /* CONFIG_BOOTARGS */

    /* Mark non-essential since main() has no more work to do */
    // 33. main() 返回后，清除主线程的 K_ESSENTIAL 标志，允许其被终止或进入休眠（如果系统配置）。
    z_thread_essential_clear(&z_main_thread);

#ifdef CONFIG_COVERAGE_DUMP
    /* Dump coverage data once the main() has exited. */
    gcov_coverage_dump(); // 运行覆盖率数据转储。
#elif defined(CONFIG_COVERAGE_SEMIHOST)
    gcov_coverage_semihost(); // 通过 Semihosting 转储覆盖率数据。
#endif /* CONFIG_COVERAGE_DUMP */
} /* LCOV_EXCL_LINE ... because we just dumped final coverage data */

#if defined(CONFIG_MULTITHREADING)
__boot_func
/**
 * @brief 初始化单个 CPU 的闲置线程
 *
 * @param i CPU ID。
 */
static void init_idle_thread(int i)
{
    // 34. 设置闲置线程的 k_thread 结构体、堆栈、入口函数 (idle)、优先级 (K_IDLE_PRIO)。
    z_setup_new_thread(thread, stack,
                       stack_size, idle, &_kernel.cpus[i],
                       NULL, NULL, K_IDLE_PRIO, K_ESSENTIAL,
                       tname);
    // 闲置线程永远不会处于休眠状态。
    z_mark_thread_as_not_sleeping(thread);

#ifdef CONFIG_SMP
    // 35. 在 SMP 配置中，标记该线程为闲置线程。
    thread->base.is_idle = 1U;
#endif /* CONFIG_SMP */
}

/**
 * @brief 初始化单个 CPU 的内核结构体
 *
 * @param id CPU ID。
 */
void z_init_cpu(int id)
{
    // 36. 初始化该 CPU 的闲置线程。
    init_idle_thread(id);
    // 37. 设置 CPU 结构体中的闲置线程指针、ID 和中断堆栈指针。
    _kernel.cpus[id].idle_thread = &z_idle_threads[id];
    _kernel.cpus[id].id = id;
    _kernel.cpus[id].irq_stack =
        (K_KERNEL_STACK_BUFFER(z_interrupt_stacks[id]) +
         K_KERNEL_STACK_SIZEOF(z_interrupt_stacks[id]));
// ... [省略 CONFIG_SCHED_THREAD_USAGE_ALL、CONFIG_PM、CONFIG_OBJ_CORE_SYSTEM 相关的代码] ...
#ifdef CONFIG_SCHED_IPI_SUPPORTED
    sys_dlist_init(&_kernel.cpus[id].ipi_workq); // 初始化 CPU 间中断 (IPI) 工作队列。
#endif
}

/**
 * @brief 准备多线程环境
 *
 * 初始化调度器，设置主线程和第一个 CPU 的闲置线程。
 *
 * @return 返回主线程堆栈指针。
 */
__boot_func
static char *prepare_multithreading(void)
{
    char *stack_ptr;

    /* _kernel.ready_q is all zeroes */
    // 38. 初始化调度器就绪队列。
    z_sched_init();

#ifndef CONFIG_SMP
    /*
     * prime the cache with the main thread since:
     * - the cache can never be NULL
     * - the main thread will be the one to run first
     * ...
     */
    // 39. 在单核模式下，将主线程设置为就绪队列的缓存，因为它将是第一个运行的线程。
    _kernel.ready_q.cache = &z_main_thread;
#endif /* CONFIG_SMP */
    // 40. 设置主线程的属性，使其入口函数为 bg_thread_main。
    stack_ptr = z_setup_new_thread(&z_main_thread, z_main_stack,
                                   K_THREAD_STACK_SIZEOF(z_main_stack),
                                   bg_thread_main, // 主线程入口
                                   NULL, NULL, NULL,
                                   CONFIG_MAIN_THREAD_PRIORITY,
                                   K_ESSENTIAL, "main");
    // 41. 标记主线程为非休眠状态。
    z_mark_thread_as_not_sleeping(&z_main_thread);
    // 42. 将主线程加入就绪队列。
    z_ready_thread(&z_main_thread);

    // 43. 初始化 CPU 0（包括其闲置线程和 CPU 结构体）。
    z_init_cpu(0);

    return stack_ptr;
}

__boot_func
/**
 * @brief 切换到主线程（第一次上下文切换）
 *
 * 该函数执行第一次上下文切换，从临时的 C 启动环境切换到主线程的上下文。
 *
 * @param stack_ptr 主线程的堆栈指针。
 */
static FUNC_NORETURN void switch_to_main_thread(char *stack_ptr)
{
#ifdef CONFIG_ARCH_HAS_CUSTOM_SWAP_TO_MAIN
    // 44. 如果架构有自定义的切换函数，调用它。
    arch_switch_to_main_thread(&z_main_thread, stack_ptr, bg_thread_main);
#else
    ARG_UNUSED(stack_ptr);
    /*
     * Context switch to main task (entry function is _main()): the
     * current fake thread is not on a wait queue or ready queue, so it
     * will never be rescheduled in.
     */
    // 45. 否则，调用通用的 z_swap_unlocked() 执行上下文切换。
    //     当前的执行环境（"假线程"）会被切换出，主线程 (`z_main_thread`) 会被切换入。
    z_swap_unlocked();
#endif /* CONFIG_ARCH_HAS_CUSTOM_SWAP_TO_MAIN */
    // 46. 标记为代码不可达，因为一旦切换成功，程序将永远不会返回此函数。
    CODE_UNREACHABLE; /* LCOV_EXCL_LINE */
}
#endif /* CONFIG_MULTITHREADING */

// ... [省略 z_early_rand_get 随机数生成函数] ...

/**
 * @brief 初始化内核的入口函数
 *
 * 该例程在系统准备好运行 C 代码时被调用（即 BSS 已清零，.data 已复制）。
 * **它是从 `prep_c.c` 调用的 Zephyr C 启动起点。**
 *
 * @return 不返回（通过上下文切换进入多线程环境）。
 */
__boot_func
FUNC_NO_STACK_PROTECTOR
FUNC_NORETURN void z_cstart(void)
{
    /* gcov hook needed to get the coverage report.*/
    gcov_static_init(); // 覆盖率工具的静态初始化。

    /* initialize early init calls */
    // 47. 运行 EARLY 级别初始化。
    z_sys_init_run_level(INIT_LEVEL_EARLY);

    /* perform any architecture-specific initialization */
    arch_kernel_init(); // 架构特定的内核初始化（如中断控制器）。

    LOG_CORE_INIT(); // 初始化日志核心。

#if defined(CONFIG_MULTITHREADING)
    z_dummy_thread_init(&_thread_dummy); // 初始化用于同步的虚假线程结构。
#endif /* CONFIG_MULTITHREADING */
    /* do any necessary initialization of static devices */
    // 48. 初始化所有静态设备的 k_obj_core 状态。
    z_device_state_init();

    // 49. 调用 SoC 和板级早期的初始化钩子。
    soc_early_init_hook();

    board_early_init_hook();

    /* perform basic hardware initialization */
    // 50. 运行 PRE_KERNEL_1 级别初始化。
    z_sys_init_run_level(INIT_LEVEL_PRE_KERNEL_1);
#if defined(CONFIG_SMP)
    arch_smp_init(); // SMP 架构初始化。
#endif
    // 51. 运行 PRE_KERNEL_2 级别初始化。
    z_sys_init_run_level(INIT_LEVEL_PRE_KERNEL_2);

#ifdef CONFIG_REQUIRES_STACK_CANARIES
    // 52. 堆栈金丝雀（Stack Canary）初始化：使用早期随机数生成器设置堆栈保护值。
    uintptr_t stack_guard;

    z_early_rand_get((uint8_t *)&stack_guard, sizeof(stack_guard));
    __stack_chk_guard = stack_guard;
    __stack_chk_guard <<= 8; // 额外的位移操作，增加随机性/避免全零。
#endif  /* CONFIG_REQUIRES_STACK_CANARIES */

// ... [省略 CONFIG_TIMING_FUNCTIONS_NEED_AT_BOOT 相关的代码] ...

#ifdef CONFIG_MULTITHREADING
    // 53. **多线程核心启动**：准备多线程环境并切换到主线程。
    switch_to_main_thread(prepare_multithreading()); // 该函数永不返回。
#else
// ... [省略非多线程环境下的启动代码] ...
    // 54. 如果没有多线程，直接调用 bg_thread_main()，完成后进入死循环。
    bg_thread_main(NULL, NULL, NULL);

    /* LCOV_EXCL_START ... */
    irq_lock();
    while (true) {
    }
    /* LCOV_EXCL_STOP */
#endif /* CONFIG_MULTITHREADING */

    /*
     * Compiler can't tell that the above routines won't return and issues
     * a warning unless we explicitly tell it that control never gets this
     * far.
     */

    // 55. 标记为代码不可达。
    CODE_UNREACHABLE; /* LCOV_EXCL_LINE */
}

#ifdef CONFIG_OBJ_CORE_SYSTEM
/**
 * @brief 初始化 CPU 内核对象类型
 *
 * 将 CPU 结构体 (`struct _cpu`) 注册为一种内核对象类型，以便进行访问控制和统计。
 */
static int init_cpu_obj_core_list(void)
{
    /* Initialize CPU object type */
    // 56. 初始化 CPU 对象类型结构体。
    z_obj_type_init(&obj_type_cpu, K_OBJ_TYPE_CPU_ID,
                    offsetof(struct _cpu, obj_core));

#ifdef CONFIG_OBJ_CORE_STATS_SYSTEM
    k_obj_type_stats_init(&obj_type_cpu, &cpu_stats_desc);
#endif /* CONFIG_OBJ_CORE_STATS_SYSTEM */

    return 0;
}

/**
 * @brief 初始化内核核心对象类型
 *
 * 将主内核结构体 (`struct z_kernel`) 注册为一种内核对象类型。
 */
static int init_kernel_obj_core_list(void)
{
    /* Initialize kernel object type */
    // 57. 初始化内核对象类型结构体。
    z_obj_type_init(&obj_type_kernel, K_OBJ_TYPE_KERNEL_ID,
                    offsetof(struct z_kernel, obj_core));

#ifdef CONFIG_OBJ_CORE_STATS_SYSTEM
    k_obj_type_stats_init(&obj_type_kernel, &kernel_stats_desc);
#endif /* CONFIG_OBJ_CORE_STATS_SYSTEM */

    // 58. 初始化并链接 _kernel 实例到内核对象核心列表中。
    k_obj_core_init_and_link(K_OBJ_CORE(&_kernel), &obj_type_kernel);
// ... [省略 CONFIG_OBJ_CORE_STATS_SYSTEM 相关的代码] ...
    return 0;
}

// 59. 使用 SYS_INIT 宏注册这两个初始化函数，确保它们在 PRE_KERNEL_1 级别运行。
SYS_INIT(init_cpu_obj_core_list, PRE_KERNEL_1,
         CONFIG_KERNEL_INIT_PRIORITY_OBJECTS);

SYS_INIT(init_kernel_obj_core_list, PRE_KERNEL_1,
         CONFIG_KERNEL_INIT_PRIORITY_OBJECTS);
#endif /* CONFIG_OBJ_CORE_SYSTEM */
```