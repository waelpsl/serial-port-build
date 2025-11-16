cmake_minimum_required(VERSION 3.10)
project(serial_port)

add_library(serial_port SHARED serial_port_jni.c)

find_library(log-lib log)

target_compile_options(serial_port PRIVATE -fPIC)
target_link_libraries(serial_port ${log-lib})
