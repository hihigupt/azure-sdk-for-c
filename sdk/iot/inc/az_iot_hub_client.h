// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#ifndef _az_IOT_HUB_CLIENT_H
#define _az_IOT_HUB_CLIENT_H

#include <az_result.h>
#include <az_span.h>

#include <_az_cfg_prefix.h>

typedef struct az_iot_hub_client_options {
  az_span module_id;
  az_span user_agent;
} az_iot_hub_client_options;

typedef struct az_iot_hub_client {
  struct {
    az_span iot_hub_hostname;
    az_span device_id;
    az_iot_hub_client_options options;
  } _internal;
} az_iot_hub_client;

az_iot_hub_client_options az_iot_hub_client_options_default();

void az_iot_hub_client_init(
    az_iot_hub_client const * client,
    az_span iot_hub_hostname,
    az_span device_id,
    az_iot_hub_client_options * options);

az_result az_iot_hub_client_user_name_get(
    az_iot_hub_client const * client,
    az_span mqtt_user_name,
    az_span * out_mqtt_user_name);

az_result az_iot_hub_client_client_id_get(
    az_iot_hub_client const * client,
    az_span mqtt_client_id,
    az_span * out_mqtt_client_id);

#include <_az_cfg_suffix.h>

#endif //!_az_IOT_HUB_CLIENT_H
