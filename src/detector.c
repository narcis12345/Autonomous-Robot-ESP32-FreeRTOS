#include "detector.h"
#include "driver/gpio.h"
#include "config.h"
#include "utilitare.h"
#include "esp_log.h"

static const char *TAG_DETECTOR = "DETECTOR_METAL";

void init_detector() {

    gpio_set_direction(PIN_SEMNAL_METAL, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_SEMNAL_METAL, GPIO_PULLDOWN_ONLY); 
}

bool detectie_metal_confirmata() {

    int numarDetectiiPozitive = 0;

    for (int i = 0; i < NUMAR_CITIRI_PENTRU_CONFIRMARE; i++) {

        if (gpio_get_level(PIN_SEMNAL_METAL) == 1) { 

            numarDetectiiPozitive++;
        }
        
        delay_ms (DELAY_INTRE_CITIRI_MS);
    }

    // Comparam cu pragul
    if (numarDetectiiPozitive >= PRAG_DETECTII_POZITIVE_NECESARE) {

        ESP_LOGD(TAG_DETECTOR, "Detectie metal confirmata (%d/%d citiri pozitive)", 
                 numarDetectiiPozitive, NUMAR_CITIRI_PENTRU_CONFIRMARE);
        return true;
    } else {

        // Acest log poate fi prea "zgomotos" dacă funcția e apelată foarte des.
        // Poți să-l comentezi sau să-i crești nivelul la ESP_LOGV (Verbose) după testare.
        ESP_LOGD(TAG_DETECTOR, "Fara detectie metal stabila (%d/%d citiri pozitive)", 
                 numarDetectiiPozitive, NUMAR_CITIRI_PENTRU_CONFIRMARE);
        return false;
    }
}
