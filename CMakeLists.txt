cmake_minimum_required(VERSION 3.30)
project(inode C)

set(CMAKE_C_STANDARD 11)

add_executable(inode main.c
        bibliotecas/bloco.h
        bibliotecas/comandos.h
        bibliotecas/sistema.h)

target_link_libraries(inode m)