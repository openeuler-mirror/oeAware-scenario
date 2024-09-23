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
#include <iostream>
#include <unordered_map>
#include <numaif.h>
#include "analysis.h"
#include <sstream>
#include <iomanip>
#include <cmath>
const int TUNE_PID_LOW_BOUND = 1000;

static bool validPmu(const PmuData &data) {
    return data.pid > TUNE_PID_LOW_BOUND && data.tid > TUNE_PID_LOW_BOUND;
}

static bool validSpe(const PmuData &data) {
    return validPmu(data) && (data.ext->event & 0x200) != 0;  // 0x200 later change to enum value
}

int Analysis::getPeriod() {
    return 1000;
}

void Analysis::init() {
    env.init();
    instanceInit();
    sysInfo.init();  // initialized after env init
    loopCnt = 0;
}

void Analysis::updatePmu(const std::string &eventName, int dataLen, const PmuData *data) {
    if (eventName == "pmu_spe_sampling") {
        updateSpe(dataLen, data);
        updateAccess();
    }
}

void Analysis::instanceInit() {
    tuneInstances[NUMA_TUNE].name = "tune_numa_mem_access";
    tuneInstances[IRQ_TUNE].name = "tune_irq";
}

void Analysis::updateSpe(int dataLen, const PmuData *data) {
    auto &procs = sysInfo.procs;
    for (int i = 0; i < dataLen; i++) {
        if (!validSpe(data[i])) {
            continue;
        }
        procs[data[i].pid].pageVa.push_back((void *)data[i].ext->va);
        procs[data[i].pid].speBuff.push_back(&data[i]);
        procs[data[i].pid].threads[data[i].tid].cpu = data[i].cpu;
    }
}

void Analysis::updateAccess() {
    unsigned long pageMask = env.pageMask;
    const std::vector<int> &cpu2Node = Env::getInstance().cpu2Node;
    auto &procs = sysInfo.procs;
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        int pid = proc_item.first;
        size_t buff_len = proc.pageVa.size();
        std::vector<int> node_id(buff_len);
        long ret = move_pages(pid, buff_len, proc.pageVa.data(), nullptr, node_id.data(), MPOL_MF_MOVE_ALL);
        if (ret == 0) {
            unsigned long pa = 0;
            for (size_t i = 0; i < buff_len; ++i) {
                if (node_id[i] < 0) {
                    continue;
                }
                proc.threads[proc.speBuff[i]->tid].realtimeInfo.access[cpu2Node[proc.speBuff[i]->cpu]][node_id[i]]++;
            }
        }
        proc.pageVa.clear();
        proc.speBuff.clear();
    }
}

void Analysis::showSummary() {
    std::cout << "============================ Analysis Summary ==============================" << std::endl;
    std::cout << "| ";
    const int nameWidth = 30;
    const int suggestWidth = 10;
    const int notesWidth = 30;
    std::cout << std::left << std::setw(nameWidth) << "Tune Instance" << " | ";
    std::cout << std::left << std::setw(suggestWidth) << "Suggest" << " | ";
    std::cout << std::left << std::setw(notesWidth) << "Note" << " |" << std::endl;

    for (auto &item : tuneInstances) {
        auto &ins = item.second;
        if (ins.name == "unknown") {
            continue;
        }
        std::cout << "| ";
        std::cout << std::left << std::setw(nameWidth) << ins.name << " | ";
        std::cout << std::left << std::setw(suggestWidth) << (ins.suggest ? "Yes" : "No") << " | ";
        std::cout << std::left << std::setw(notesWidth) << ins.notes << " |" << std::endl;
    }
    std::cout << "Note : analysis plugin period is " << getPeriod() << "ms, loop " << loopCnt << "times" << std::endl;
}

void Analysis::numaTuneSuggest(const TaskInfo &taskInfo, bool isSummary) {
    tuneInstances[NUMA_TUNE].suggest = false;
    if (env.numaNum <= 1) {
        tuneInstances[NUMA_TUNE].notes = "No NUMA";
        return;
    }

    if (isSummary && taskInfo.loopCnt == 0) {
        tuneInstances[NUMA_TUNE].notes = "loop cnt error";
        return;
    }
    uint64_t accessSum = isSummary ? ceil(taskInfo.accessSum * 1.0 / taskInfo.loopCnt) : taskInfo.accessSum;
    if (accessSum < 200) {
        tuneInstances[NUMA_TUNE].notes = "No access";
        return;
    }
    if (taskInfo.numaScore > 0.95) {
        tuneInstances[NUMA_TUNE].notes = "Most local access";
        return;
    }
    tuneInstances[NUMA_TUNE].suggest = true;
    std::ostringstream tmp;
    tmp << std::fixed << std::setprecision(2) << (1 - taskInfo.numaScore) * 100;
    tuneInstances[NUMA_TUNE].notes = "Gap : " + tmp.str() + "%";
}

void Analysis::summary() {
    sysInfo.traceInfoSummary();
    numaTuneSuggest(sysInfo.summaryInfo, true);
}

void Analysis::analyze() {
    sysInfo.summaryProcs();
    sysInfo.calculateNumaScore();
    sysInfo.setLoopCnt(loopCnt);
    sysInfo.appendTraceInfo();
    sysInfo.clearRealtimeInfo();
    loopCnt++;
}

void Analysis::exit() {
    summary();
    showSummary();
    sysInfo.reset();
}
