cmake_minimum_required(VERSION 3.22)

project(ping_pong)

add_executable(ping "ping.cpp")
target_link_libraries(ping PRIVATE pthread)

add_executable(pong "pong.cpp")
target_link_libraries(pong PRIVATE pthread