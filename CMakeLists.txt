cmake_minimum_required(VERSION 3.23)
project(mmn14_1 C)

set(CMAKE_C_STANDARD 90)

add_executable(mmn14_1 main.c comands_proccesing.c comands_proccesing.h errors.c errors.h first_pass.c first_pass.h sec_pass.c sec_pass.h linked_list.c linked_list.h printer.c printer.h pre_assmbler.c pre_assmbler.h globals.h)
