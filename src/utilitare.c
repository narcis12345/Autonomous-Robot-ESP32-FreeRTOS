#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "utilitare.h"

void delay_ms(int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

//un tick este de 10ms -> 300 tick uri = 3 secunde