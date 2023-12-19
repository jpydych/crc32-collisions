cmake_minimum_required(VERSION 3.27)
project(CRC32Collisions C)

set(CMAKE_C_STANDARD 23)

add_executable(CRC32Collisions main.c
        crc32.c)
