#include "driver/i2c.h"
#include "servo.h"
#include "utilitare.h"
#include "esp32/rom/ets_sys.h"
#include "config.h"
#include "detector.h"
#include "esp_log.h"
#include "comunicare_bluetooth.h"

static const char *TAG_SERVO = "SERVO_CONTROL";
static int unghi_la_ultima_detectie_valida = UNGHI_CENTRU; 

void init_servo() {

    i2c_config_t config = {

        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ
    };
    i2c_param_config(I2C_MASTER_NUM, &config);
    i2c_driver_install(I2C_MASTER_NUM, config.mode, 0, 0, 0);

    // Wake up PCA9685
    uint8_t mode1[] = {0x00, 0x00};  // ieșire din sleep
    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, mode1, 2, 1000 / portTICK_PERIOD_MS);

    // Setare frecvență 50Hz (prescaler)
    uint8_t prescale[] = {0xFE, 121};  // 25MHz / (4096 * 50Hz) = ~121
    uint8_t mode1_sleep[] = {0x00, 0x10};  // sleep
    uint8_t mode1_restart[] = {0x00, 0x00};  // ieșire din sleep
    uint8_t mode1_autoinc[] = {0x00, 0xA1};  // autoincrement ON

    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, mode1_sleep, 2, 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, prescale, 2, 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, mode1_restart, 2, 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, mode1_autoinc, 2, 1000 / portTICK_PERIOD_MS);
}

void seteaza_servo(int canal, int unghi) {

    uint16_t pulse = 205 + (unghi * (410 - 205)) / 180;

    uint8_t reg = 0x06 + 4 * canal;
    uint8_t data[5];
    data[0] = reg;
    data[1] = 0;                      // LED_ON_L
    data[2] = 0;                      // LED_ON_H
    data[3] = pulse & 0xFF;           // LED_OFF_L
    data[4] = (pulse >> 8) & 0x0F;    // LED_OFF_H

    i2c_master_write_to_device(I2C_MASTER_NUM, PCA9685_ADDR, data, 5, 1000 / portTICK_PERIOD_MS);
}

bool miscare_brat_si_detectie(int delay_pe_pas_ms) {
    ESP_LOGI(TAG_SERVO, "Incepere ciclu de scanare...");

    // FAZA 1: Scanare Centru -> Stânga
    for (int unghi = UNGHI_CENTRU; unghi <= UNGHI_STANGA; unghi++) {
        if (flag_oprire_urgenta) {
            ESP_LOGW(TAG_SERVO, "Oprire de urgenta detectata!");
            return false;
        }
        seteaza_servo(SERVO_CANAL_SCANARE, unghi);
        delay_ms(delay_pe_pas_ms);
        if (detectie_metal_confirmata()) {
            ESP_LOGW(TAG_SERVO, "METAL DETECTAT! Unghi: %d. Scanare oprita.", unghi);
            unghi_la_ultima_detectie_valida = unghi;
            return true; // Am găsit. Oprim brațul aici și ieșim imediat.
        }
    }

    // FAZA 2: Scanare Stânga -> Dreapta
    for (int unghi = UNGHI_STANGA; unghi >= UNGHI_DREAPTA; unghi--) {
        if (flag_oprire_urgenta) {
            ESP_LOGW(TAG_SERVO, "Oprire de urgenta detectata!");
            return false;
        }
        seteaza_servo(SERVO_CANAL_SCANARE, unghi);
        delay_ms(delay_pe_pas_ms);
        if (detectie_metal_confirmata()) {
            ESP_LOGW(TAG_SERVO, "METAL DETECTAT! Unghi: %d. Scanare oprita.", unghi);
            unghi_la_ultima_detectie_valida = unghi;
            return true; // Am găsit. Oprim brațul aici și ieșim imediat.
        }
    }

    // FAZA 3: Scanare (Revenire) Dreapta -> Centru
    for (int unghi = UNGHI_DREAPTA; unghi <= UNGHI_CENTRU; unghi++) {
        if (flag_oprire_urgenta) {
            ESP_LOGW(TAG_SERVO, "Oprire de urgenta detectata!");
            return false;
        }
        seteaza_servo(SERVO_CANAL_SCANARE, unghi);
        delay_ms(delay_pe_pas_ms);
        if (detectie_metal_confirmata()) {
            ESP_LOGW(TAG_SERVO, "METAL DETECTAT! Unghi: %d. Scanare oprita.", unghi);
            unghi_la_ultima_detectie_valida = unghi;
            return true; // Am găsit. Oprim brațul aici și ieșim imediat.
        }
    }

    ESP_LOGI(TAG_SERVO, "Scanare completa finalizata. Niciun metal detectat.");
    return false; // Am terminat toată scanarea și nu am găsit nimic.
}

/**
 * @brief Returnează unghiul la care s-a făcut ultima detecție.
 * Nu este folosită în logica de bază, dar e bună pentru extinderi viitoare.
 */
int get_unghi_ultima_detectie() {
    return unghi_la_ultima_detectie_valida;
}




