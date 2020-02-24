// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include "az_iot_hub_telemetry.h"

static az_span telemetry_prop_delim = AZ_SPAN_LITERAL_FROM_STR("?");
static az_span telemetry_topic_prefix = AZ_SPAN_LITERAL_FROM_STR("devices/");
static az_span telemetry_topic_suffix = AZ_SPAN_LITERAL_FROM_STR("/messages/events/");


az_result az_iot_telemetry_publish_topic_get(az_iot_identity identity, az_span properties, 
                                                        az_span mqtt_topic, az_span *out_mqtt_topic)
{
    az_result result;

    if(az_span_capacity(mqtt_topic) <
            az_span_length(identity._internal.device_id) +
            az_span_length(identity._internal.module_id) +
            az_span_length(properties) + 
            az_span_length(telemetry_topic_prefix) +
            az_span_length(telemetry_topic_suffix))
    {
        result = AZ_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        if((result = az_span_append(*out_mqtt_topic, telemetry_topic_prefix, out_mqtt_topic)) == AZ_OK)
        {
            if((result = az_span_append(*out_mqtt_topic, identity._internal.device_id, out_mqtt_topic)) == AZ_OK)
            {
                if((result = az_span_append(*out_mqtt_topic, telemetry_topic_suffix, out_mqtt_topic)) == AZ_OK)
                {
                    if((result = az_span_append(*out_mqtt_topic, telemetry_prop_delim, out_mqtt_topic)) == AZ_OK)
                    {
                        if((result = az_span_append(*out_mqtt_topic, properties, out_mqtt_topic)) == AZ_OK)
                        {

                        }
                    }
                }
            }
        }
    }
    

    return result;
}