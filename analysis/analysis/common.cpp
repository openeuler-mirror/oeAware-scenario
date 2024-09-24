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
#include "common.h"
#include <iostream>

void traceInfoSummary(const std::vector<TaskInfo> &infos, TaskInfo &summary) {
    for (auto &info : infos) {
        for (size_t i = 0; i < info.access.size(); i++) {
            for (size_t j = 0; j < info.access[i].size(); j++) {
                summary.access[i][j] += info.access[i][j];
            }
        }
        summary.cycles += info.cycles;
    }
    summary.loopCnt = infos.size();
    summary.calculateNumaScore();
}

TaskInfo::TaskInfo() {
    init();
}

void TaskInfo::init() {
    loopCnt = 0;
    int numaNum = Env::getInstance().numaNum;
    access.resize(numaNum);
    for (int i = 0; i < numaNum; i++) {
        access[i].resize(numaNum);
    }
}

void TaskInfo::calculateNumaScore() {
    Env &env = Env::getInstance();
    accessCost = 0;
    accessSum = 0;
    for (int i = 0; i < env.numaNum; i++) {
        for (int j = 0; j < env.numaNum; j++) {
            accessCost += access[i][j] * env.distance[i][j];
            accessSum += access[i][j];
        }
    }
    numaScore = accessSum == 0 ? 0.0 : (accessSum * env.maxDistance - accessCost) * 1.0 / (accessSum * env.diffDistance);
}

void TaskInfo::clearData() {
    for (auto &tmp : access) {
        for (auto &i : tmp) { i = 0; }
    }
    numaScore = 0.0;
    cycles = 0;
    accessSum = 0;
    accessCost = 0;
}

void Proc::summaryThreads() {
    for (auto &t : threads) {
        auto &thread = t.second;
        for (size_t i = 0; i < thread.realtimeInfo.access.size(); i++) {
            for (size_t j = 0; j < thread.realtimeInfo.access[i].size(); j++) {
                realtimeInfo.access[i][j] += thread.realtimeInfo.access[i][j];
            }
        }
        realtimeInfo.cycles += thread.realtimeInfo.cycles;
    }
}

void Proc::calculateNumaScore() {
    for (auto &thread : threads) {
        thread.second.realtimeInfo.calculateNumaScore();
    }
    realtimeInfo.calculateNumaScore();
}

void Proc::clearRealtimeInfo() {
    for (auto &t : threads) {
        auto &thread = t.second;
        thread.realtimeInfo.clearData();
    }
    realtimeInfo.clearData();
}

void SystemInfo::init() {
    realtimeInfo.init();
    summaryInfo.init();
}

void SystemInfo::summaryProcs() {
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        proc.summaryThreads();
        for (size_t i = 0; i < proc.realtimeInfo.access.size(); i++) {
            for (size_t j = 0; j < proc.realtimeInfo.access[i].size(); j++) {
                realtimeInfo.access[i][j] += proc.realtimeInfo.access[i][j];
            }
        }
        realtimeInfo.cycles += proc.realtimeInfo.cycles;
    }
}

void SystemInfo::calculateNumaScore() {
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        proc.calculateNumaScore();
    }
    realtimeInfo.calculateNumaScore();
}

void SystemInfo::setLoopCnt(uint64_t loopCnt) {
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        for (auto &thread_item : proc.threads) {
            auto &thread = thread_item.second;
            thread.realtimeInfo.loopCnt = loopCnt;
        }
        proc.realtimeInfo.loopCnt = loopCnt;
    }
    realtimeInfo.loopCnt = loopCnt;
}

void SystemInfo::clearRealtimeInfo() {
    for (auto &proc_item : procs) {
        proc_item.second.clearRealtimeInfo();
    }

    realtimeInfo.clearData();
}

void SystemInfo::appendTraceInfo() {
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        for (auto &thread_item : proc.threads) {
            auto thread = thread_item.second;
            thread.traceInfo.emplace_back(thread.realtimeInfo);
        }
        proc.traceInfo.emplace_back(proc.realtimeInfo);
    }
    traceInfo.emplace_back(realtimeInfo);
}

void SystemInfo::traceInfoSummary() {
    for (auto &proc_item : procs) {
        auto &proc = proc_item.second;
        for (auto &thread_item : proc.threads) {
            auto thread = thread_item.second;
            ::traceInfoSummary(thread.traceInfo, thread.summaryInfo);
        }
        ::traceInfoSummary(proc.traceInfo, proc.summaryInfo);
    }
    ::traceInfoSummary(traceInfo, summaryInfo);
}

void SystemInfo::reset() {
    procs.clear();
    realtimeInfo.clearData();
    summaryInfo.clearData();
    traceInfo.clear();
}
