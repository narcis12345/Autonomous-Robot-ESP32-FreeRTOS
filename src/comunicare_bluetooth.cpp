#include <string>

extern "C" {
  #include <freertos/FreeRTOS.h>  
  #include <freertos/task.h>  
  #include <freertos/semphr.h>
  #include <driver/gpio.h>
  #include <esp_log.h>
  
  #include "motoare.h"
  #include "servo.h"
  #include "utilitare.h"
  #include "detector.h"
  #include "config.h"
}

#include "BluetoothSerial.h" 
#include "comunicare_bluetooth.h" 

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `pio run -t menuconfig` to and enable it
#endif

static const char *TAG_BT = "BLUETOOTH_COMM";
static BluetoothSerial SerialBT;

void task_comunicare_bluetooth(void *pvParameter) {

    std::string mesajPrimit = "";
    
    ESP_LOGI(TAG_BT, "Task-ul de comunicare a pornit");

    while (1) {

        if (SerialBT.available()) {

            mesajPrimit = SerialBT.readString().c_str();

            if (mesajPrimit == "START_CMD") {
                
                ESP_LOGI(TAG_BT, "Comanda START primita de la aplicatie!");
                xSemaphoreGive(semafor_semnal_start); 
            }

            if (mesajPrimit == "STOP_CMD") {
                
                ESP_LOGW(TAG_BT, "Comanda STOP primita de la aplicatie!");
                flag_oprire_urgenta = true; 
                trimite_mesaj_bluetooth("OPRIRE DE URGENTA!");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void trimite_mesaj_bluetooth(const char* mesaj) {

    if (SerialBT.hasClient()) {

        SerialBT.println(mesaj); 
        SerialBT.println("");      
    }
    
    ESP_LOGD(TAG_BT, "Mesaj trimis: '%s'", mesaj);
}

void init_bluetooth() {

    SerialBT.begin("Vehicul_Autonom_NARCIS");
    ESP_LOGI(TAG_BT, "Serviciul Bluetooth a pornit: Vehicul_Autonom_NARCIS");
    xTaskCreate(task_comunicare_bluetooth, "task_bt_comm", 4096, NULL, 5, NULL);
}
