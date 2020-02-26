// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_span.h>
#include <az_result.h>
#include "az_iot_hub_client.h"
#include "az_iot_hub_client_telemetry.h"

static az_span telemetry_prop_delim = AZ_SPAN_LITERAL_FROM_STR("?");
static az_span telemetry_topic_devices_prefix = AZ_SPAN_LITERAL_FROM_STR("devices/");
static az_span telemetry_topic_modules_prefix = AZ_SPAN_LITERAL_FROM_STR("modules/");
static az_span telemetry_topic_suffix = AZ_SPAN_LITERAL_FROM_STR("/messages/events/");

/**
 * e.g. topic for deviceId and moduleId :
 *      devices/{device_id}/modules/{module_id}/messages/events/ 
 *      topic for deviceId:
 *      devices/{device_id}/messages/events/ 
 */
az_result 
az_iot_client_telemetry_publish_topic_get(az_iot_hub_client const * client,
                                          az_iot_hub_properties const *properties,
                                          az_span mqtt_topic,
                                          az_span * out_mqtt_topic)
{
    az_result result;

    if(az_span_capacity(mqtt_topic) <
            (az_span_length(telemetry_topic_devices_prefix) +
            az_span_length(telemetry_topic_suffix) +
            az_span_length(client->_internal.device_id) +
            (az_span_length(client->_internal.options.module_id) > 0 ? 
            (az_span_length(telemetry_topic_modules_prefix) + 
             az_span_length(client->_internal.options.module_id)) : 0 ) +
            az_span_length(telemetry_topic_suffix) +
            az_span_length(telemetry_prop_delim) +
            az_span_length(properties->_internal.properties)))
    {
        result = AZ_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, telemetry_topic_devices_prefix, &mqtt_topic));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, client->_internal.device_id, &mqtt_topic));

        if (az_span_length(client->_internal.options.module_id) > 0)
        {
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, telemetry_topic_modules_prefix, &mqtt_topic));
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, client->_internal.options.module_id, &mqtt_topic));
        }

        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, telemetry_topic_suffix, &mqtt_topic));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, telemetry_prop_delim, &mqtt_topic));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic, properties->_internal.properties, &mqtt_topic));

        if (out_mqtt_topic != NULL)
        {
            *out_mqtt_topic = az_span_init(az_span_ptr(mqtt_topic), az_span_length(mqtt_topic), az_span_capacity(mqtt_topic));
        }
        
        result = AZ_OK;
    }

    return result;
}