#include <Arduino.h>
#include "comunicare_bluetooth.h"

extern "C" {
  #include <freertos/FreeRTOS.h>
  #include <freertos/task.h>
  #include <freertos/semphr.h>
  #include <driver/gpio.h>
  #include <esp_log.h>
  #include <nvs_flash.h>

  #include "motoare.h"
  #include "servo.h"
  #include "utilitare.h"
  #include "detector.h"
  #include "config.h"
}

const float OFFSET_BRAT_MM = 240.0;
SemaphoreHandle_t semafor_semnal_start;
static float distanta_parcursa_mm = 0.0;
const float VITEZA_ROBOT_MM_PER_MS = 0.33; 
static const char *TAG_MAIN = "SETUP";
static const char *TAG_ROBOT = "TASK_ROBOT";
volatile bool flag_oprire_urgenta = false;

void task_logica_robot(void *pvParameter) {

    ESP_LOGI(TAG_ROBOT, "Astept semnalul de START de la aplicatie...");
    
    xSemaphoreTake(semafor_semnal_start, portMAX_DELAY);
    
    ESP_LOGI(TAG_ROBOT, "SEMNAL START PRIMIT! Incepere ciclu de operare.");
    trimite_mesaj_bluetooth("Misiune inceputa.");
    delay_ms(100);

    while (1) {

        if (flag_oprire_urgenta) {

            ESP_LOGE(TAG_ROBOT, "OPRIRE DE URGENTA. Oprire task.");
            oprire();
            delay_ms(200); 
            vTaskDelete(NULL);
        }

        ESP_LOGI(TAG_ROBOT, "--- Ciclu Nou: Scanare si Decizie ---");
        
        trimite_mesaj_bluetooth("Scanez zona...");
        delay_ms(100);
        bool metal_detectat = miscare_brat_si_detectie(DELAY_PAS_SERVO_SCANARE_MS);

        if (metal_detectat) {

            ESP_LOGW(TAG_ROBOT, "!!! OBIECT DETECTAT !!!");
            oprire();
            
            float distanta_reala_mm = distanta_parcursa_mm + OFFSET_BRAT_MM;
            char mesaj_alerta[64];
            sprintf(mesaj_alerta, "Metal detectat la aprox. %.2f m!", distanta_reala_mm / 1000.0);
            trimite_mesaj_bluetooth(mesaj_alerta);
            delay_ms(100);

            ESP_LOGI(TAG_ROBOT, "Initiere manevra de ocolire...");
            trimite_mesaj_bluetooth("Obstacol gasit. Ocolesc...");
            delay_ms(100);
            ocolire_prin_stanga();
            
            ESP_LOGI(TAG_ROBOT, "Readucere brat la centru post-ocolire...");
            seteaza_servo(SERVO_CANAL_SCANARE, UNGHI_CENTRU);
            delay_ms(500);

        } else {

            ESP_LOGI(TAG_ROBOT, "Cale libera. Avansare...");
            trimite_mesaj_bluetooth("Cale libera. Avansare...");
            delay_ms(100);
            mers_inainte_controlat(DURATA_AVANSARE_NORMAL_MS);
            
            distanta_parcursa_mm += DURATA_AVANSARE_NORMAL_MS * VITEZA_ROBOT_MM_PER_MS;
        }

        delay_ms(100); 
    }
}

void setup() {

  Serial.begin(115200);
  ESP_LOGI(TAG_MAIN, "--- SISTEMUL PORNESTE (in setup()) ---");

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {

    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG_MAIN, "Initializare module hardware...");
  init_motoare();
  init_servo();
  init_detector();

  ESP_LOGI(TAG_MAIN, "Aducere brat la centru...");
  seteaza_servo(SERVO_CANAL_SCANARE, UNGHI_CENTRU);
  delay_ms(500);

  semafor_semnal_start = xSemaphoreCreateBinary();
  if (semafor_semnal_start == NULL) {

    ESP_LOGE(TAG_MAIN, "Eroare la crearea semaforului!");
  }

  init_bluetooth();

  xTaskCreate(
      task_logica_robot,     
      "task_logica_robot",   
      8192,                 
      NULL,                 
      5,                    
      NULL                   
  );

  ESP_LOGI(TAG_MAIN, "Initializare completa. Sistemul este pregatit si asteapta comenzi.");
}

void loop() {
  
  vTaskDelay(pdTICKS_TO_MS(1000));
}
