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
#include <string>
#include <iostream>
 /* oeaware manager interface */
#include "scenario.h"
/* dependent external plugin interfaces */
#include "pmu_plugin.h"
/* external plugin dependent interfaces */

/* internal data processing interface */
#include "analysis.h"

static std::unordered_map<std::string, uint64_t> lastBufCnt;
static std::unordered_map<std::string, const DataRingBuf *> ringBufMap;

static std::string deps;
static Analysis analysis;
static const char *getVersion() {
    return nullptr;
}

static const char *getName() {
    return "analysis";
}

static const char *getDescription() {
    return nullptr;
}

static const char *getDep() {
    deps = PMU_SPE;
    deps += "-";
    deps += PMU_CYCLES_SAMPLING;
    return deps.c_str();
}

static int getPriority() {
    return 1;
}

static int getPeriod() {
    return analysis.getPeriod();
}

static bool enable() {
    analysis.init();
    return true;
}
static void disable() {
    analysis.exit();
}

static const struct DataRingBuf *getRingBuf() {
    return nullptr;
}

static int getRingBufNum(const std::string &instanceName) {
    const DataRingBuf *ringBufs = ringBufMap[instanceName];
    if (ringBufs == nullptr) {
        return 0;
    }
    return std::min(ringBufs->count - lastBufCnt[instanceName], (uint64_t)ringBufs->buf_len);
}

static void updateBufNPmu(int n, const std::string &instanceName) {
    const DataRingBuf *ringBufs = ringBufMap[instanceName];
    if (ringBufs == nullptr) {
        return;
    }
    int offset = (ringBufs->index + ringBufs->buf_len - n) % ringBufs->buf_len;
    DataBuf *buf = &ringBufs->buf[offset];
    if (buf && buf->len > 0) {
        analysis.updatePmu(instanceName, buf->len, (PmuData *)buf->data);
    }
}

static void updateBufCnt(const std::string &instanceName) {
    if (ringBufMap[instanceName]) {
        lastBufCnt[instanceName] = ringBufMap[instanceName]->count;
    } else {
        lastBufCnt[instanceName] = 0; // reset
    }
}

static void updatePmu() {
    int speBufNum = getRingBufNum(PMU_SPE);
    // spe and cycles should have same ring buf num
    if (speBufNum != 0 && speBufNum == getRingBufNum(PMU_CYCLES_SAMPLING)) {
        for (int n = 0; n < speBufNum; ++n) {
            updateBufNPmu(n, PMU_CYCLES_SAMPLING);
            updateBufNPmu(n, PMU_SPE);
            // to do something after update spe and cycles every time
        }
    }
    updateBufCnt(PMU_SPE);
    updateBufCnt(PMU_CYCLES_SAMPLING);
}

static void run(const Param *param) {
    for (int i = 0; i < param->len; ++i) {
        if (param->ring_bufs[i]) {
            ringBufMap[param->ring_bufs[i]->instance_name] = param->ring_bufs[i];
        }
    }
    updatePmu();
    analysis.analyze();
    ringBufMap.clear();
}

struct Interface analysisIntf = {
    .get_version = getVersion,
    .get_name = getName,
    .get_description = getDescription,
    .get_dep = getDep,
    .get_priority = getPriority,
    .get_type = nullptr,
    .get_period = getPeriod,
    .enable = enable,
    .disable = disable,
    .get_ring_buf = getRingBuf,
    .run = run,
};

extern "C" int get_instance(Interface **ins) {
    *ins = &analysisIntf;
    return 1;
}