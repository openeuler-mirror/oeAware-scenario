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
#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <iostream>
#include <unordered_map>
#include "common.h"
#include <vector>
#include "env.h"

enum InstanceName {
    NUMA_TUNE,
    IRQ_TUNE,
    GAZELLE_TUNE,
};

struct Instance {
    std::string name = "unknown";
    bool suggest = false;
    std::string notes;
};

class Analysis {
private:
    SystemInfo sysInfo;
    std::unordered_map<InstanceName, Instance> tuneInstances;
    uint64_t loopCnt = 0;
    void instanceInit();
    void updateSpe(int dataLen, const PmuData *data);
    void updateAccess();
    void numaTuneSuggest(const TaskInfo &taskInfo, bool isSummary);
    void summary();
    void showSummary();
public:
    Env &env = Env::getInstance();
    int getPeriod();
    void init();
    void updatePmu(const std::string &eventName, int dataLen, const PmuData *data);
    void analyze();
    void exit();
};

#endif