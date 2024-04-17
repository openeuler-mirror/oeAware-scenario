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
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include "scenario.h"
#include "collector.h"

Scenario::Scenario()
{
}

void Scenario::system_loop_init()
{
    memset(system_uncore, 0, sizeof(system_uncore));
}
void Scenario::system_uncore_update(struct PmuData *data, int dateLen)
{

    for (int i = 0; i < dateLen; i++) {
        if (strstr(data[i].evt, "rx_outer") != 0) {
            system_uncore[UNCORE_EV_RX_OUTER] += data[i].count;
        } else if (strstr(data[i].evt, "rx_sccl") != 0) {
            system_uncore[UNCORE_EV_RX_SCCL] += data[i].count;
        } else if (strstr(data[i].evt, "rx_ops_num") != 0) {
            system_uncore[UNCORE_EV_RX_OPS_NUM] += data[i].count;
        }
    }
}

void Scenario::access_buf_updata()
{
    uncore_rst.numa_score = (system_uncore[UNCORE_EV_RX_OPS_NUM] < 10000) ? 0.0 : \
        ((float)system_uncore[UNCORE_EV_RX_OUTER] + system_uncore[UNCORE_EV_RX_SCCL]) / system_uncore[UNCORE_EV_RX_OPS_NUM];
    std::cout << "[scenario example] numa_score = " << std::fixed << std::setprecision(2) << uncore_rst.numa_score << '\n';
    buf[SBT_ACCESS].buf = &uncore_rst;
    buf[SBT_ACCESS].bufLen = 1;
}

const ScenarioBuf &Scenario::get_scenario_buf(ScenarioBufType type)
{
    return buf[type];
}