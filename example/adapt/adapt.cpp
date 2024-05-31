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
#include <cstdlib>
#include <algorithm>
#include "scenario.h"
#include "common.h"
#include "collector.h"

#define UNCORE_SCENARIO_BUF_NUM 1
struct DataRingBuf g_uncore_scenario_buf = { 0, 0, 0, 0, 0};
uint64_t g_pmu_uncore_buf_cnt = 0;

const char *get_version()
{
    return "v1.0";
}

const char *get_name()
{
    return "scenario_example";
}

const char *get_description()
{
    return "scenario_example";
}

const char *get_dep()
{
    return PMU_UNCORE;
}

int get_priority()
{
    return 1;
}

int get_period()
{
    return 1000;
}

bool enable()
{
    if (g_uncore_scenario_buf.buf == NULL) {
        g_uncore_scenario_buf.buf = (struct DataBuf *)malloc(sizeof(struct DataBuf) * UNCORE_SCENARIO_BUF_NUM);
        g_uncore_scenario_buf.buf_len = UNCORE_SCENARIO_BUF_NUM;
        g_uncore_scenario_buf.index = -1;
        g_uncore_scenario_buf.count = 0;
    }

    return true;
}

void disable()
{

}

void run(const struct Param *para)
{
    Scenario &ins = Scenario::getInstance();
    ins.system_loop_init();

    for (int i = 0; i < para->len; i++) {
        struct DataRingBuf *ringbuf = (struct DataRingBuf *)para->ring_bufs[i];
        if (strcmp(ringbuf->instance_name, PMU_UNCORE) == 0) {
            int dataNum = std::min(ringbuf->count - g_pmu_uncore_buf_cnt, (uint64_t)ringbuf->buf_len);
            struct DataBuf *buf = nullptr;
            for (int n = 0; n < dataNum; n++) {
                int offset = (ringbuf->index + ringbuf->buf_len - n) % ringbuf->buf_len;
                buf = &ringbuf->buf[offset];
                ins.system_uncore_update((PmuData *)buf->data, buf->len);
            }
            if (dataNum > 0) {
                ins.access_buf_updata();
            }
            g_pmu_uncore_buf_cnt = ringbuf->count;
        }
    }
}

const struct DataRingBuf *get_ring_buf()
{
    Scenario &ins = Scenario::getInstance();
    const ScenarioBuf access_buf = ins.get_scenario_buf(SBT_ACCESS);
    g_uncore_scenario_buf.instance_name = SCENARIO_ACCESS_BUF;
    g_uncore_scenario_buf.index++;
    g_uncore_scenario_buf.index %= UNCORE_SCENARIO_BUF_NUM;
    g_uncore_scenario_buf.count++;

    g_uncore_scenario_buf.buf_len = UNCORE_SCENARIO_BUF_NUM;
    g_uncore_scenario_buf.buf->len = access_buf.bufLen;
    g_uncore_scenario_buf.buf->data = access_buf.buf;

    return &g_uncore_scenario_buf;
}

struct Interface aware_interface = {
    .get_version = get_version,
    .get_name = get_name,
    .get_description = get_description,
    .get_dep = get_dep,
    .get_priority = get_priority,
    .get_type = nullptr,
    .get_period = get_period,
    .enable = enable,
    .disable = disable,
    .get_ring_buf = get_ring_buf,
    .run = run,
};

extern "C" int get_instance(struct Interface **interface)
{
    *interface = &aware_interface;
    return 1;
}

