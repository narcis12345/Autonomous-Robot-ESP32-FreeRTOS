#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "utilitare.h"
#include "comunicare_bluetooth.h"

void delay_ms(uint32_t ms) {

    uint32_t start = esp_log_timestamp();
    while (esp_log_timestamp() - start < ms) {

        if (flag_oprire_urgenta) {
            return;
        }

        vTaskDelay(pdMS_TO_TICKS(1)); 
    }
}
