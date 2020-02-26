// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_span.h>
#include <az_result.h>
#include "az_iot_hub_client.h"

static az_span amp = AZ_SPAN_LITERAL_FROM_STR("&");
static az_span delemeter = AZ_SPAN_LITERAL_FROM_STR("/");
static az_span api_version = AZ_SPAN_LITERAL_FROM_STR("?api-version=2019-07-01-preview");

void az_iot_hub_client_init(az_iot_hub_client const *client,
                            az_span iot_hub_hostname,
                            az_span device_id,
                            az_iot_hub_client_options *options)
{
    if (client != NULL)
    {
        memcpy((void*)&client->_internal.iot_hub_hostname, (void*)&iot_hub_hostname, sizeof(iot_hub_hostname));
        memcpy((void*)&client->_internal.device_id, (void*)&device_id, sizeof(device_id));

        if (options != NULL)
        {
            memcpy((void*)&client->_internal.options, (void*)options, sizeof(az_iot_hub_client_options));
        }
        else
        {
            memset((void*)&client->_internal.options, 0, sizeof(az_iot_hub_client_options));
        }
    }
}

az_result az_iot_hub_client_user_name_get(az_iot_hub_client const *client,
                                          az_span mqtt_user_name,
                                          az_span *out_mqtt_user_name)
{
    az_result result;

    if(az_span_capacity(mqtt_user_name) <
            (az_span_length(client->_internal.iot_hub_hostname) +
            az_span_length(delemeter) +
            az_span_length(client->_internal.device_id) +
            (az_span_length(client->_internal.options.module_id) > 0 ? 
            (az_span_length(delemeter) + 
             az_span_length(client->_internal.options.module_id)) : 0 ) +
            az_span_length(delemeter) + 
            az_span_length(api_version) + 
            (az_span_length(client->_internal.options.user_agent) > 0 ? 
            (az_span_length(amp) + 
             az_span_length(client->_internal.options.user_agent)) : 0 )))
    {
        result = AZ_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, client->_internal.iot_hub_hostname, &mqtt_user_name));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, delemeter, &mqtt_user_name));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, client->_internal.device_id, &mqtt_user_name));

        if (az_span_length(client->_internal.options.module_id) > 0)
        {
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, delemeter, &mqtt_user_name));
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, client->_internal.options.module_id, &mqtt_user_name));
        }

        AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, delemeter, &mqtt_user_name));
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, api_version, &mqtt_user_name));

        if (az_span_length(client->_internal.options.user_agent) > 0)
        {
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, amp, &mqtt_user_name));
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_user_name, client->_internal.options.user_agent, &mqtt_user_name));
        }

        if (out_mqtt_user_name != NULL)
        {
            *out_mqtt_user_name = az_span_init(az_span_ptr(mqtt_user_name), az_span_length(mqtt_user_name), az_span_capacity(mqtt_user_name));
        }
        
        result = AZ_OK;
    }

    return result;
}

/**
 * device_id/module_id
 */
az_result az_iot_hub_client_client_id_get(
    az_iot_hub_client const * client,
    az_span mqtt_client_id,
    az_span * out_mqtt_client_id)
{
    az_result result;

    if(az_span_capacity(mqtt_client_id) <
            (az_span_length(client->_internal.device_id) +
            (az_span_length(client->_internal.options.module_id) > 0 ? 
            (az_span_length(delemeter) + 
             az_span_length(client->_internal.options.module_id)) : 0 )))
    {
        result = AZ_ERROR_OUT_OF_MEMORY;
    }
    else
    {
        AZ_RETURN_IF_FAILED(az_span_append(mqtt_client_id, client->_internal.device_id, &mqtt_client_id));

        if (az_span_length(client->_internal.options.module_id) > 0)
        {
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_client_id, delemeter, &mqtt_client_id));
            AZ_RETURN_IF_FAILED(az_span_append(mqtt_client_id, client->_internal.options.module_id, &mqtt_client_id));
        }

        if (out_mqtt_client_id != NULL)
        {
            *out_mqtt_client_id = az_span_init(az_span_ptr(mqtt_client_id), az_span_length(mqtt_client_id), az_span_capacity(mqtt_client_id));
        }
        
        result = AZ_OK;
    }

    return result;
}

az_iot_hub_client_options az_iot_hub_client_options_default()
{
  az_iot_hub_client_options option;
  option.module_id = az_span_null();
  option.user_agent = az_span_null();

  return option;
}