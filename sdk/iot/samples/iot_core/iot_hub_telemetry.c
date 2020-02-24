
#include <stdio.h>
#include "az_span.h"
#include "az_iot_hub_telemetry.h"
#include "az_iot_hub_connect.h"

#define DEVICE_ID "my_test_device"

static uint8_t topic_buffer[100];
static az_span props = AZ_SPAN_LITERAL_FROM_STR("key=value");
static az_iot_identity id = {
    ._internal.device_id = AZ_SPAN_LITERAL_FROM_STR(DEVICE_ID)
};

int main(void)
{
    az_span out_topic = AZ_SPAN_FROM_BUFFER(topic_buffer);
    if(az_succeeded(az_iot_telemetry_publish_topic_get(id, props, out_topic, &out_topic)))
    {
        printf("%.*s\r\n", az_span_length(out_topic), az_span_ptr(out_topic));
    }
}

