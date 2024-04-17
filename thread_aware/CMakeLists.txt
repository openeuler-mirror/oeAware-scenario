cmake_minimum_required(VERSION 3.11)
project(thread_aware)
include_directories(../include)
add_library(thread_scenario SHARED
    thread_aware.cpp
) 