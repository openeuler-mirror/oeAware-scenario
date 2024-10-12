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
#include <numa.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include "env.h"
unsigned long GetPageMask()
{
    static unsigned long pageMask = 0;
    if (pageMask == 0) {
        int pageSize = sysconf(_SC_PAGE_SIZE);
        if (pageSize > 0) {
            pageMask = ~(static_cast<unsigned long>(pageSize) - 1);
        } else {
            pageMask = ~0xFFF;
        }
    }

    return pageMask;
}

static uint64_t GetCpuCycles(int cpu)
{
    std::string freqPath = "/sys/devices/system/cpu/cpu" + std::to_string(cpu) + "/cpufreq/scaling_cur_freq";
    std::ifstream freqFile(freqPath);

    if (!freqFile.is_open()) {
        return 0;
    }

    uint64_t freq;
    freqFile >> freq;
    freqFile.close();

    if (freqFile.fail()) {
        return 0;
    }

    return freq * 1000; // 1000: kHz to Hz
}

static void InitCpuCycles(std::vector<uint64_t> &maxCycles, uint64_t &sysMaxCycles)
{
    for (int cpu = 0; cpu < maxCycles.size(); cpu++) {
        maxCycles[cpu] = GetCpuCycles(cpu);
        sysMaxCycles += maxCycles[cpu];
    }
}

bool Env::Init()
{
    if (initialized) {
        return true;
    }
    numaNum = numa_num_configured_nodes();
    cpuNum = sysconf(_SC_NPROCESSORS_CONF);
    cpu2Node.resize(cpuNum, -1);
    struct bitmask *cpumask = numa_allocate_cpumask();
    for (int nid = 0; nid < numaNum; ++nid) {
        numa_bitmask_clearall(cpumask);
        numa_node_to_cpus(nid, cpumask);
        for (int cpu = 0; cpu < cpuNum; cpu++) {
            if (numa_bitmask_isbitset(cpumask, cpu)) {
                cpu2Node[cpu] = nid;
            }
        }
    }
    numa_free_nodemask(cpumask);

    pageMask = GetPageMask();
    InitDistance();
    cpuMaxCycles.resize(cpuNum, 0);
    InitCpuCycles(cpuMaxCycles, sysMaxCycles);
    initialized = true;
    return true;
}

void Env::InitDistance()
{
    maxDistance = 0;
    distance.resize(numaNum, std::vector<int>(numaNum, 0));
    for (int i = 0; i < numaNum; ++i) {
        for (int j = 0; j < numaNum; ++j) {
            distance[i][j] = numa_distance(i, j);
            if (maxDistance < distance[i][j]) {
                maxDistance = distance[i][j];
            }
        }
    }
    diffDistance = maxDistance - distance[0][0];
}


