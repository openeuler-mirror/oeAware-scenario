/******************************************************************************
 * Copyright (c) 2024 Huawei Technologies Co., Ltd. All rights reserved.
 * oeAware is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 ******************************************************************************/
#ifndef __COLLECTOR_H__
#define __COLLECTOR_H__

#include <stdint.h>
#include <sys/types.h>

struct CpuTopology;
struct Stack;
struct PmuDataExt {
    unsigned long pa;               // physical address
    unsigned long va;               // virtual address
    unsigned long event;            // event id
};

struct PmuData {
    struct Stack *stack;           // call stack
    const char *evt;                // event name
    int64_t ts;                     // time stamp
    pid_t pid;                      // process id
    int tid;                        // thread id
    unsigned cpu;                   // cpu id
    struct CpuTopology *cpuTopo;    // cpu topology
    const char *comm;               // process command
    int period;                     // number of Samples
    union {
        uint64_t count;             // event count. Only available for Counting.
        struct PmuDataExt *ext;     // extension. Only available for Spe.
    };
};

#define PMU_CYCLES_COUNTING "PMU_CYCLES_COUNTING"
#define PMU_CYCLES_SAMPLING "PMU_CYCLES_SAMPLING"
#define PMU_UNCORE "PMU_UNCORE"
#define PMU_SPE "PMU_SPE"

#endif