// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_span.h>
#include <az_result.h>
#include "az_iot_hub_client_c2d.h"

static az_span device_prefix = AZ_SPAN_LITERAL_FROM_STR("devices/");
static az_span module_prefix = AZ_SPAN_LITERAL_FROM_STR("/modules/");
static az_span suffix = AZ_SPAN_LITERAL_FROM_STR("/messages/devicebound/#");

/**
 * 
 *  "devices/{device_id}/messages/devicebound/#"
 *  "devices/{device_id}/modules/{module_id}/messages/devicebound/#"
 **/
az_result az_iot_hub_client_c2d_subscribe_topic_filter_get(az_iot_hub_client const * client,
                                                           az_span mqtt_topic_filter,
                                                           az_span * out_mqtt_topic_filter)
{
    az_result result;

    if(az_span_capacity(mqtt_topic_filter) <
            (az_span_length(device_prefix) +
            az_span_length(client->_internal.device_id) +
            (az_span_length(client->_internal.options.module_id) > 0 ? 
            (az_span_length(module_prefix) + 
             az_span_length(client->_internal.options.module_id)) : 0 ) +
            az_span_length(suffix)))
    {
        result = AZ_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic_filter, device_prefix, &mqtt_topic_filter));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic_filter, client->_internal.device_id, &mqtt_topic_filter));

        if (az_span_length(client->_internal.options.module_id) > 0)
        {
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic_filter, module_prefix, &mqtt_topic_filter));
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic_filter, client->_internal.options.module_id, &mqtt_topic_filter));
        }

        AZ_RETURN_IF_FAILED(az_span_append(mqtt_topic_filter, suffix, &mqtt_topic_filter));

        if (out_mqtt_topic_filter != NULL)
        {
            *out_mqtt_topic_filter = az_span_init(az_span_ptr(mqtt_topic_filter), az_span_length(mqtt_topic_filter), az_span_capacity(mqtt_topic_filter));
        }
        
        result = AZ_OK;
    }

    return result;
}
