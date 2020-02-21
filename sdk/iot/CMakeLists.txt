# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.12)

project(az_iot LANGUAGES C)

add_library(az_iot_core
    ${CMAKE_CURRENT_LIST_DIR}/src/az_iot_hub_telemetry.c
)

target_include_directories(az_iot_core
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/inc
)

target_link_libraries(az_iot_core
    PUBLIC
        az_core
)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/samples)
