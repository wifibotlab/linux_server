cmake_minimum_required(VERSION 3.7)
project(server_modbus)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c rs232.c key_non_block.c modbus_wifibot.c tcp_server.c)
add_executable(server_modbus ${SOURCE_FILES})
target_link_libraries(server_modbus pthread)