# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: MIT

add_executable(iot_hub_telemetry
    ${CMAKE_CURRENT_LIST_DIR}/iot_core/iot_hub_telemetry.c
)

target_link_libraries(iot_hub_telemetry
    PRIVATE
        az_iot_core
        az_core
)
