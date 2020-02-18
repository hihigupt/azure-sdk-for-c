
/**
 * @file    az_iot_twin.h
 * @brief   API's for use with the twin feature of IoT Hub.
 */

#include "az_result.h"

/**
 * @brief Parse an mqtt message if it is a twin message
 * 
 * @param   iot_client      The #az_iot_client* for which the message was sent.
 * @param   pub_msg         The #az_iot_mqtt_pub_msg* that was received and needs to be parsed.
 *                          It shall not be `NULL`.
 * @param   twin_msg        The #az_iot_twin_msg* into which the received #az_iot_mqtt_pub_msg will
 *                          be parsed.
 * 
 * @return  az_result       Returns an `az_result` with the result of the parse.
 *  @retval #AZ_OK          If the #az_iot_mqtt_pub_msg was successfully parsed and `twin_msg` is
 *                          returned as a valid #az_iot_twin_msg.
 *  @retval #AZ_ERROR_ARG   If one of the passed arguments are `NULL`.
 */
az_result az_iot_twin_handle(az_iot_client* iot_client, az_iot_mqtt_pub_msg* pub_msg
                                                az_iot_twin_msg* twin_msg);
