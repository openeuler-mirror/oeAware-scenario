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
#include "thread_aware.h"
#include "scenario.h"
#include <vector>
#include <fstream>

char *THREAD_SCENARIO = "thread_scenario";
char *THREAD_DEPS = "thread_collector";
const std::string CONFIG_PATH = "/usr/lib64/oeAware-plugin/scenario/thread_scenario.ini";
const int CYCLE_SIZE = 100;

static std::vector<ThreadInfo> thread_info(THREAD_NUM);
static DataHeader data_header;
static DataBuf data_buf;
static std::vector<std::string> key_list;

static void read_key_list(const std::string &file_name) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        key_list.emplace_back(line);
    }
    file.close();
}

char* get_version() {
    return nullptr;
}

char* get_name() {
    return THREAD_SCENARIO;
}

char* get_description() {
    return nullptr;
}

char* get_dep() {
    return THREAD_DEPS;
}

int get_cycle() {
    return CYCLE_SIZE;
}

void enable() {
    data_header.buf_len = 1;
    data_header.buf = &data_buf;
    read_key_list(CONFIG_PATH);
}

void disable() {

}

void aware(void *info[], int len) {
    data_header.index++;
    data_header.count++;
    int index = data_header.count % data_header.buf_len;
    
    DataHeader *header = (DataHeader*)info[0];
    DataBuf buf = header->buf[header->count % header->buf_len];
    ThreadInfo *data = (ThreadInfo*)buf.data;
    int cnt = 0;
    for (int i = 0; i < buf.len; ++i) {
        for (int j = 0; j < key_list.size(); ++j) {
            if (data[i].name == key_list[j] && cnt < THREAD_NUM) {
                thread_info[cnt].name = data[i].name;
                thread_info[cnt].tid = data[i].tid;
                thread_info[cnt].pid = data[i].pid;
                cnt++;
                break;
            }
        }
    }
    data_buf.len = cnt;
    data_buf.data = thread_info.data();
    data_header.buf[index] = data_buf;
}

void* get_ring_buf() {
    return (void*)&data_header;
}

static struct ScenarioInterface aware_interface = {
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

extern "C" int get_instance(ScenarioInterface **ins) {
    *ins = &aware_interface;
    return 1;
}
