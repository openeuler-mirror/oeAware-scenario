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
#ifndef THREAD_AWARE_H
#define THREAD_AWARE_H
#include <string>

#define DATA_HEADER_TYPE_SIZE 64

const int THREAD_NUM = 65536;
const int DATA_BUF_DEFAULT_SIZE = 20;

struct ThreadInfo {
    int pid;
    int tid;
    std::string name;
};

struct DataBuf {
    int len;
    void *data;
};

struct DataHeader {
    char type[DATA_HEADER_TYPE_SIZE];              // collector type
    int index;                                     // buf write index, initial value is -1
    uint64_t count;                                // collector times
    struct DataBuf *buf;
    int buf_len;
};

#endif // !THREAD_AWARE_H
