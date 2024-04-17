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
#ifndef __SCEMARO_HH_
#define __SCEMARO_HH_
#include <cstdint>
#include <string.h>
#include "scenario_plugin.h"

typedef struct {
    int bufLen;
    void *buf;
} ScenarioBuf;

typedef enum {
    SBT_TOPO,
    SBT_ACCESS,
    SBT_IRQ,
    SBT_MAX,
} ScenarioBufType;

typedef enum {
    UNCORE_EV_RX_OUTER,
    UNCORE_EV_RX_SCCL,
    UNCORE_EV_RX_OPS_NUM,
    UNCORE_EV_FLUX_RD,
    UNCORE_EV_FLUX_WR,
    UNCORE_EV_MAX
} UNCORE_EV_E;

class Scenario {
public:
    static Scenario &getInstance()
    {
        static Scenario instance;
        return instance;
    }
    Scenario(const Scenario &) = delete;
    Scenario &operator=(const Scenario &) = delete;
    Scenario();
    ~Scenario() {}

private:
    uint64_t system_uncore[UNCORE_EV_MAX];
    struct uncore_scenario uncore_rst;

public:
    void system_loop_init();
    void system_uncore_update(struct PmuData *data, int dateLen);
    void access_buf_updata();
    const ScenarioBuf &get_scenario_buf(ScenarioBufType type);
private:
    ScenarioBuf buf[SBT_MAX];
};


#endif