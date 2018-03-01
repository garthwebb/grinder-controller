cmake_minimum_required(VERSION 3.9)
project(grinder_controller)

SET(MCU "atmega168")  # atmega168pb
SET(F_CPU "8000000")
SET(CMAKE_SYSTEM_NAME Generic)

# For some reason, these paths have to be absolute, otherwise
# CLion won't be able to find headers etc.
SET(CMAKE_C_COMPILER /opt/local/bin/avr-gcc)
SET(CMAKE_CXX_COMPILER /opt/local/bin/avr-g++)

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_FLAGS "-mmcu=${MCU} -DF_CPU=${F_CPU} -Os")
SET(CMAKE_C_LINK_FLAGS "-mmcu=${MCU}")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

add_executable(grinder_controller
        main.c
        main.h
        Makefile state_machine.c state_machine.h motor.c motor.h switch.c switch.h serial.c serial.h timers.c timers.h)
