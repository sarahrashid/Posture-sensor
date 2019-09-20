#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "azure_c_shared_utility/macro_utils.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/platform.h"
#include "iothub_device_client.h"
#include "iothub_client_options.h"
#include "iothub.h"
#include "iothub_message.h"
#include "parson.h"
#include "sdkconfig.h"
#include "postureSensor.h"

#define SAMPLE_MQTT

#ifdef SAMPLE_MQTT
#include "iothubtransportmqtt.h"
#endif // SAMPLE_MQTT
#ifdef SAMPLE_MQTT_OVER_WEBSOCKETS
#include "iothubtransportmqtt_websockets.h"
#endif // SAMPLE_MQTT_OVER_WEBSOCKETS
#ifdef SAMPLE_AMQP
#include "iothubtransportamqp.h"
#endif // SAMPLE_AMQP
#ifdef SAMPLE_AMQP_OVER_WEBSOCKETS
#include "iothubtransportamqp_websockets.h"
#endif // SAMPLE_AMQP_OVER_WEBSOCKETS
#ifdef SAMPLE_HTTP
#include "iothubtransporthttp.h"
#endif // SAMPLE_HTTP

#ifdef SET_TRUSTED_CERT_IN_SAMPLES
#include "certs.h"
#endif // SET_TRUSTED_CERT_IN_SAMPLES

static const char *connectionString = CONFIG_IOTHUB_CONNECTION_STRING;

#define DOWORK_LOOP_NUM 3

//  Converts the desired properties of the Device Twin JSON blob received from IoT Hub into a PostureSensor object. - SR
PostureSensor *parseFromJson(const char *json, DEVICE_TWIN_UPDATE_STATE update_state, PostureSensor *currentPostureSensor)
{
    PostureSensor *postureSensor = malloc(sizeof(PostureSensor)); // ? 
    postureSensor->desired_operating_state = currentPostureSensor->desired_operating_state;
    postureSensor->desired_threshold = currentPostureSensor->desired_threshold;

    JSON_Value *root_value = NULL;
    JSON_Object *root_object = NULL;

    // printing the json for debugging
    // printf("*************\n%s\n****************\n", json);

    if (NULL == postureSensor)
    {
        (void)printf("ERROR: Failed to allocate memory\r\n");
    }
    else
    {
        root_value = json_parse_string(json);
        root_object = json_value_get_object(root_value);

        // Only desired properties:
        JSON_Value *desired_operating_state = NULL;
        JSON_Value *desired_threshold = NULL;

        // logic for updating one of the desired properties individually
        if (update_state == DEVICE_TWIN_UPDATE_COMPLETE)
        {
            // read the json and get the values - SR
            if (json_object_dothas_value(root_object, "desired.desired_operating_state.value"))
            {
                desired_operating_state = json_object_dotget_value(root_object, "desired.desired_operating_state.value");
            }
            if (json_object_dothas_value(root_object, "desired.desired_threshold.value"))
            {
                desired_threshold = json_object_dotget_value(root_object, "desired.desired_threshold.value");
            }
        }
        else
        {
            if (json_object_dothas_value(root_object, "desired_operating_state.value"))
            {
                desired_operating_state = json_object_dotget_value(root_object, "desired_operating_state.value");
            }
            if (json_object_dothas_value(root_object, "desired_threshold.value"))
            {
                desired_threshold = json_object_dotget_value(root_object, "desired_threshold.value");
            }
        }

        if (desired_operating_state != NULL)
        {
            printf("dos = %d\n", (bool)json_value_get_boolean(desired_operating_state));
            postureSensor->desired_operating_state = (bool)json_value_get_boolean(desired_operating_state);
        }

        if (desired_threshold != NULL)
        {
            printf("dt = %d\n", (uint32_t)json_value_get_number(desired_threshold));
            postureSensor->desired_threshold = (uint32_t)json_value_get_number(desired_threshold);
        }
        return postureSensor;
    }
    return NULL;
}

static void iothub_client_device_twin_and_methods_sample_run(void)
{
    IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
    IOTHUB_DEVICE_CLIENT_LL_HANDLE iotHubClientHandle;

    // Select the Protocol to use with the connection
#ifdef SAMPLE_MQTT
    protocol = MQTT_Protocol;
#endif // SAMPLE_MQTT
#ifdef SAMPLE_MQTT_OVER_WEBSOCKETS
    protocol = MQTT_WebSocket_Protocol;
#endif // SAMPLE_MQTT_OVER_WEBSOCKETS
#ifdef SAMPLE_AMQP
    protocol = AMQP_Protocol;
#endif // SAMPLE_AMQP
#ifdef SAMPLE_AMQP_OVER_WEBSOCKETS
    protocol = AMQP_Protocol_over_WebSocketsTls;
#endif // SAMPLE_AMQP_OVER_WEBSOCKETS
#ifdef SAMPLE_HTTP
    protocol = HTTP_Protocol;
#endif // SAMPLE_HTTP

// can take all of these out right?

    if (IoTHub_Init() != 0)
    {
        (void)printf("Failed to initialize the platform.\r\n");
    }
    else
    {
        if ((iotHubClientHandle = IoTHubDeviceClient_LL_CreateFromConnectionString(connectionString, protocol)) == NULL)
        {
            (void)printf("ERROR: iotHubClientHandle is NULL!\r\n");
        }
        else
        {
            // Uncomment the following lines to enable verbose logging (e.g., for debugging).
            //bool traceOn = true;
            //(void)IoTHubDeviceClient_SetOption(iotHubClientHandle, OPTION_LOG_TRACE, &traceOn);

            (void)IoTHubDeviceClient_LL_SetDeviceTwinCallback(iotHubClientHandle, deviceTwinCallback, &postureSensorCurrent);

            while (1)
            {
                IoTHubDeviceClient_LL_DoWork(iotHubClientHandle);
                ThreadAPI_Sleep(10);
            }

            IoTHubDeviceClient_LL_Destroy(iotHubClientHandle);
        }

        IoTHub_Deinit();
    }
}

int iothub_client_device_twin_init(void)
{
    iothub_client_device_twin_and_methods_sample_run();
    return 0;
}