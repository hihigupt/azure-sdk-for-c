// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef _az_IOT_PROVISIONING_CLIENT_H
#define _az_IOT_PROVISIONING_CLIENT_H

#include <az_span.h>
#include <az_result.h>
//#include "az_iot_mqtt.h"

#include <_az_cfg_prefix.h>

typedef struct az_iot_provisioning_client {
    struct {
        az_span registration_id;
    } _internal;
} az_iot_provisioning_client;

void az_iot_provisioning_client_init(az_iot_provisioning_client* client, az_span id_scope, az_span registration_id, az_iot_mqtt_connect* mqtt_connect);

#include <_az_cfg_suffix.h>

#endif //!_az_IOT_PROVISIONING_CLIENT_H
