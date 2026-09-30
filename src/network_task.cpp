//
// Created by Tran Lan Anh on 29.9.2026.
//

#include "network_task.h"
#include "system_state.h"
#include "IPStack.h"

extern "C"
{
    bool run_tls_client_test(
        const uint8_t *cert,
        size_t cert_len,
        const char *server,
        const char *request,
        int timeout
    );
}

void network_task(void *pvParameters)
{
    // wifi connection
    IPStack ipstack(WIFI_SSID, WIFI_PASSWORD);
    while (true)
    {
        float co2 = systemState.measuredCo2;
        float temperature = systemState.temperature;
        float humidity = systemState.relativeHumidity;
        float fanSpeed = systemState.fanSpeedPercent;
        float co2Setpoint = systemState.co2Setpoint;

        //sent to Thinkspeak
        char body[256];

        snprintf(body, sizeof(body),
                 "api_key=" API_KEY "&field1=%.1f"
                 "&field2=%.1f"
                 "&field3=%.1f"
                 "&field4=%u"
                 "&field5=%.1f",
                 co2,
                 temperature,
                 humidity,
                 fanSpeed,
                 co2Setpoint);

        char request[512];

        snprintf(request, sizeof(request),
                 "POST /update HTTP/1.1\r\n"
                 "Host: " TLS_CLIENT_SERVER "\r\n"
                 "Content-Type: application/x-www-form-urlencoded\r\n"
                 "Content-Length: %u\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 (unsigned int)strlen(body),
                 body);


        // TLS secure
        const uint8_t certificate[] = THINGSPEAK_CERT;
        bool pass= run_tls_client_test(certificate, sizeof(certificate), TLS_CLIENT_SERVER, request, TLS_CLIENT_TIMEOUT_SECS);
        if (pass)
        {
            printf("Test passed\n");
        }else
        {
            printf("Test failed\n");
        }
        vTaskDelay(pdMS_TO_TICKS(30000));
    }

}
