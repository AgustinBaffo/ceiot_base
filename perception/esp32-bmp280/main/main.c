/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <time.h>
#include <inttypes.h>


#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include <bmp280.h>
#include "../config.h"

/* HTTP Constants that aren't configurable in menuconfig */
#define WEB_PATH "/measurement"

static const char *TAG = "temp_collector";

static char *BODY = "id="DEVICE_ID"&timestamp=%lu&t=%0.2f&p=%0.2f&h=%0.2f";

static char *REQUEST_POST = "POST "WEB_PATH" HTTP/1.0\r\n"
    "Host: "API_IP_PORT"\r\n"
    "User-Agent: "USER_AGENT"\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
    "Content-Length: %d\r\n"
    "\r\n"
    "%s";

static void get_timestamp()
{
    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;

    time(&now);
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();

    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "-------------------------------\n");
    ESP_LOGI(TAG, "The current date/time is: %s\n", strftime_buf);
    ESP_LOGI(TAG, "-------------------------------\n");
}


char* get_json_param(char* str, char** pos)
{
    char* ptr = strchr(str, '&');  // Busca la primera ocurrencia del carácter '&' en el string.
    if (ptr == NULL) {  // Si no encuentra '&' devuelve NULL.
        return NULL;
    }
    char* ptr2 = strchr(ptr, '=');  // Busca la primera ocurrencia del carácter '=' después del '&'.
    if (ptr2 == NULL) {  // Si no encuentra '=' devuelve NULL.
        return NULL;
    }
    ptr++;  // Avanza el puntero para saltar el carácter '&'.
    size_t len = ptr2 - ptr;  // Calcula la longitud de la subcadena que queremos obtener.
    char* result = malloc(len + 1);  // Reserva memoria para la subcadena (más el carácter nulo).
    if (result == NULL) {  // Si no se pudo reservar memoria devuelve NULL.
        return NULL;
    }
    strncpy(result, ptr, len);  // Copia la subcadena en el buffer resultante.
    result[len] = '\0';  // Agrega el carácter nulo al final.
    (*pos) = ptr2;
    return result;
}

char* get_json_value(char* str, char** pos)
{
    char* ptr = strchr(str, '=');  // Busca la primera ocurrencia del carácter '=' en el string.
    if (ptr == NULL) {  // Si no encuentra '=' devuelve NULL.
        return NULL;
    }
    char* ptr2 = strchr(ptr, '&');  // Busca la primera ocurrencia del carácter '&' después del '='.
    if (ptr2 == NULL) {  // Si no encuentra '&' devuelve NULL.
        return NULL;
    }
    ptr++;  // Avanza el puntero para saltar el carácter '='.
    size_t len = ptr2 - ptr;  // Calcula la longitud de la subcadena que queremos obtener.
    char* result = malloc(len + 1);  // Reserva memoria para la subcadena (más el carácter nulo).
    if (result == NULL) {  // Si no se pudo reservar memoria devuelve NULL.
        return NULL;
    }
    strncpy(result, ptr, len);  // Copia la subcadena en el buffer resultante.
    result[len] = '\0';  // Agrega el carácter nulo al final.
    (*pos) = ptr2;
    return result;
}

char* append_character(char* str, char init, char end) {
    int len = strlen(str);
    char* result = (char*)malloc((len + 3) * sizeof(char)); // allocate memory for the quoted string
    if (result == NULL) {
        printf("Error: failed to allocate memory for the quoted string.\n");
        exit(1);
    }
    result[0] = init; // append a quote at the beginning of the string
    strcpy(result + 1, str); // copy the original string after the quote
    result[len + 1] = end; // append a quote at the end of the string
    result[len + 2] = '\0'; // null-terminate the string
    return result;
}

char* format_param(char* param, char* value) {
    
    int len_param = strlen(param);
    int len_value = strlen(value);
    char* result = (char*)malloc((len_param + len_value + 6) * sizeof(char)); // allocate memory for the quoted string
    if (result == NULL) {
        printf("Error: failed to allocate memory for the quoted string.\n");
        exit(1);
    }
    
    int len_append = 0;
    
    result[0] = '\"';
    len_append++;
    
    strcpy(result + len_append, param);
    len_append += len_param;
    
    result[len_append] = '\"';
    len_append++;
    
    result[len_append] = ':';
    len_append++;
    
    result[len_append] = '\"';
    len_append++;
    
    strcpy(result + len_append, value);
    len_append += len_value;
    
    result[len_append] = '\"';
    len_append++;
    
    result[len_append] = '\0';
    return result;
}


char* append_tuple(char* msg, char* tuple) {
    
    int len_msg = strlen(msg);
    int len_tuple = strlen(tuple);
    char* result = (char*)malloc((len_msg + len_tuple + 2) * sizeof(char)); // allocate memory for the quoted string
    if (result == NULL) {
        printf("Error: failed to allocate memory for the quoted string.\n");
        exit(1);
    }
    
    strcpy(result, msg);
    result[len_msg] = ',';
    strcpy(result + len_msg + 1, tuple);
    result[len_msg+len_tuple + 1] = '\0';
    
    return result;
}

char* append_first(char* msg, char* tuple) {
    
    int len_msg = strlen(msg);
    int len_tuple = strlen(tuple);
    char* result = (char*)malloc((len_msg + len_tuple + 2) * sizeof(char)); // allocate memory for the quoted string
    if (result == NULL) {
        printf("Error: failed to allocate memory for the quoted string.\n");
        exit(1);
    }
    
    strcpy(result, msg);
    strcpy(result + len_msg, tuple);
    result[len_msg + len_tuple + 1] = '\0';
    
    return result;
}


char* http_to_json(char* str_original){
    str_original = append_character(str_original, '&', '&');
    
    char* str = str_original;
    char* not_used;
    
    char* msg_json = "";
    bool first_value = true;
    while(1){
        char* param = get_json_param(str,&not_used);
        if (param == NULL) {
            break;
        }
        // printf("param: %s\n", param);
        char* value = get_json_value(str,&str);
        // printf("val: %s\n", value);
        
        char* tuple = format_param(param, value);
        
        if(first_value){
            msg_json = append_first(msg_json,tuple);
            first_value = false;
        }else{
            msg_json = append_tuple(msg_json,tuple);
        }
        
    }
    
    msg_json = append_character(msg_json, '{', '}');
    
    return msg_json;
}

static void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    char body[128];
    char recv_buf[64];

    char send_buf[512];

    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, SDA_GPIO, SCL_GPIO));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    ESP_LOGI(TAG, "BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;


    while(1) {
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK) {
            ESP_LOGI(TAG, "Temperature/pressure reading failed\n");
        } else {
            time_t current_time = time(NULL);
            unsigned long timestamp = (unsigned long)current_time;
            get_timestamp();

            ESP_LOGI(TAG, "Timestamp: %lu, Pressure: %.2f Pa, Temperature: %.2f C, Humidity: %.2f\n", timestamp, pressure, temperature, humidity);

            sprintf(body, BODY, timestamp,  temperature , pressure , humidity );
            sprintf(send_buf, REQUEST_POST, (int)strlen(body),body );
            
            ESP_LOGI(TAG, "*****************************************\n");
            char* msg_json = http_to_json(body);
            ESP_LOGI(TAG, "msg_json: %s\n", msg_json);
            ESP_LOGI(TAG, "*****************************************\n");

	        ESP_LOGI(TAG,"sending: \n%s\n",send_buf);
        }    

        int err = getaddrinfo(API_IP, API_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.

           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, send_buf, strlen(send_buf)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);

        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        close(s);

        for(int countdown = 10; countdown >= 0; countdown--) {
            ESP_LOGI(TAG, "%d... ", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "Starting again!");
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(i2cdev_init());

    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
}
