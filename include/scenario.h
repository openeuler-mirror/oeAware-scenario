/******************************************************************************
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 * oeAware is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 ******************************************************************************/
#ifndef SCENARIO_H
#define SCENARIO_H

#ifdef __cplusplus
extern "C" {
#endif
struct ScenarioInterface {
    char* (*get_version)();
    char* (*get_name)();
    char* (*get_description)();
    char* (*get_dep)();
    int (*get_cycle)();
    void (*enable)();
    void (*disable)();
    void (*aware)(void*[], int);
    void* (*get_ring_buf)();
};

int get_instance(struct ScenarioInterface **ins);

#ifdef __cplusplus
}
#endif

#endif // !SCENARIO_H