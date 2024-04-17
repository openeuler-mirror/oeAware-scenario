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
struct DataHeader g_uncore_scenario_buf = { 0 };
uint64_t g_pmu_uncore_buf_cnt = 0;
char *get_version()
{
    return "v1.0";
}

char *get_description()
{
    return "scenario_example";
}

char *get_name()
{
    return "scenario_example";
}

char *get_dep()
{
    return "collector_pmu_uncore";
}

int get_cycle()
{
    return 1000;
}

void enable()
{
    if (g_uncore_scenario_buf.buf == NULL) {
        g_uncore_scenario_buf.buf = (struct DataBuf *)malloc(sizeof(struct DataBuf) * UNCORE_SCENARIO_BUF_NUM);
        g_uncore_scenario_buf.buf_len = UNCORE_SCENARIO_BUF_NUM;
        g_uncore_scenario_buf.index = -1;
        g_uncore_scenario_buf.count = 0;
    }
}
void disable()
{

}

void aware(void *info[], int len)
{
    Scenario &ins = Scenario::getInstance();
    ins.system_loop_init();

    for (int i = 0; i < len; i++) {
        struct DataHeader *header = (struct DataHeader *)info[i];
        if (strcmp(header->type, PMU_UNCORE) == 0) {
            int dataNum = std::min(header->count - g_pmu_uncore_buf_cnt, (uint64_t)header->buf_len);
            struct DataBuf *buf = nullptr;
            for (int n = 0; n < dataNum; n++) {
                int offset = (header->index + header->buf_len - n) % header->buf_len;
                buf = &header->buf[offset];
                ins.system_uncore_update((PmuData *)buf->data, buf->len);
            }
            if (dataNum > 0) {
                ins.access_buf_updata();
            }
            g_pmu_uncore_buf_cnt = header->count;
        }
    }
}

void *get_ring_buf()
{
    Scenario &ins = Scenario::getInstance();
    const ScenarioBuf access_buf = ins.get_scenario_buf(SBT_ACCESS);
    struct DataHeader header;
    strcpy(g_uncore_scenario_buf.type, SCENARIO_ACCESS_BUF);
    g_uncore_scenario_buf.index++;
    g_uncore_scenario_buf.index %= UNCORE_SCENARIO_BUF_NUM;
    g_uncore_scenario_buf.count++;

    g_uncore_scenario_buf.buf_len = UNCORE_SCENARIO_BUF_NUM;
    g_uncore_scenario_buf.buf->len = access_buf.bufLen;
    g_uncore_scenario_buf.buf->data = access_buf.buf;

    return (void *)&g_uncore_scenario_buf;
}

struct ScenarioInterface aware_interface = {
    .get_version = get_version,
    .get_name = get_name,
    .get_description = get_description,
    .get_dep = get_dep,
    .get_cycle = get_cycle,
    .enable = enable,
    .disable = disable,
    .aware = aware,
    .get_ring_buf = get_ring_buf,
};

extern "C" int get_instance(ScenarioInterface * *ins)
{
    *ins = &aware_interface;
    return 1;
}

