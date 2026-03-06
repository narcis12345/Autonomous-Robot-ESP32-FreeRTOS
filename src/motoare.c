#include "motoare.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "utilitare.h"
#include "config.h"
#include "esp_log.h"
#include "comunicare_bluetooth.h"

void init_motoare() {
    
    gpio_set_direction(MOTOR_A_INAINTE, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_A_INAPOI, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_INAINTE, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_INAPOI, GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_MOTOR_A, GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_MOTOR_B, GPIO_MODE_OUTPUT);    


    // PWM pe ENABLE_A și ENABLE_B
    ledc_timer_config_t pwm_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, // rezoluție 0-255
        .freq_hz = 1000,                     // frecvență PWM: 1kHz
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&pwm_timer);

    ledc_channel_config_t pwm_channel_A = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = ENABLE_MOTOR_A,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&pwm_channel_A);

    ledc_channel_config_t pwm_channel_B = {
        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = ENABLE_MOTOR_B,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0
    };
    ledc_channel_config(&pwm_channel_B);
}

void set_viteza_motoare(uint8_t duty_a, uint8_t duty_b) {

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty_a);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, duty_b);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
}

void mers_inainte() {

    set_viteza_motoare(120, 120);

    gpio_set_level(MOTOR_A_INAINTE, 1);
    gpio_set_level(MOTOR_A_INAPOI, 0);

    gpio_set_level(MOTOR_B_INAINTE, 1);
    gpio_set_level(MOTOR_B_INAPOI, 0);
}

void mers_inapoi() {

    set_viteza_motoare(120, 120);

    gpio_set_level(MOTOR_A_INAINTE, 0);
    gpio_set_level(MOTOR_A_INAPOI, 1);

    gpio_set_level(MOTOR_B_INAINTE, 0);
    gpio_set_level(MOTOR_B_INAPOI, 1);
}

void viraj_dreapta() {

    set_viteza_motoare(150, 150);

    gpio_set_level(MOTOR_A_INAINTE, 0);
    gpio_set_level(MOTOR_A_INAPOI, 1);

    gpio_set_level(MOTOR_B_INAINTE, 1);
    gpio_set_level(MOTOR_B_INAPOI, 0);
}

void viraj_stanga() {

    set_viteza_motoare(150, 150);

    gpio_set_level(MOTOR_A_INAINTE, 1);
    gpio_set_level(MOTOR_A_INAPOI, 0);

    gpio_set_level(MOTOR_B_INAINTE, 0);
    gpio_set_level(MOTOR_B_INAPOI, 1);
}

void oprire() {

    set_viteza_motoare(0, 0);
}

void mers_inainte_controlat(uint32_t durata_ms) {
    mers_inainte();
    
    // În loc de un singur delay mare, facem multe delay-uri mici
    // și verificăm flag-ul la fiecare pas.
    for (uint32_t i = 0; i < durata_ms; i += 10) {
        if (flag_oprire_urgenta) {
            oprire(); // Oprim imediat ce vedem flag-ul
            return;  // Și ieșim din funcție
        }
        delay_ms(10); // Așteptăm 10ms
    }
    
    oprire(); // Oprim la final, dacă nu a fost nicio urgență
}

void asteapta_start() {

    gpio_set_direction(BUTON_START, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTON_START, GPIO_PULLUP_ONLY);
    gpio_set_direction(LED_TEST, GPIO_MODE_OUTPUT);

    printf("se asteapta startul!\n");

    while (gpio_get_level(BUTON_START) == 1) {
        
        vTaskDelay(1);
    }

    printf("START!\n");

    gpio_set_level(LED_TEST, 1);
}

void ocolire_prin_stanga() {
    
    ESP_LOGI("MOTOARE_OCOLIRE", "Ocolire stanga SIMPLA: Pas 1 - Viraj stanga");
    viraj_stanga(); // Sau viraj_stanga_puternic
    delay_ms(DELAY_VIRAJ_MS); // Calibrează acest DELAY_VIRAJ_MS
    oprire();
    delay_ms(DELAY_STOP_MS);

    ESP_LOGI("MOTOARE_OCOLIRE", "Ocolire stanga SIMPLA: Pas 2 - Mers inainte (lateral)");
    mers_inainte();
    delay_ms(DELAY_MERS_LATERAL_MS); // Calibrează acest DELAY_MERS_MS
    oprire();
    delay_ms(DELAY_STOP_MS);

    ESP_LOGI("MOTOARE_OCOLIRE", "Ocolire stanga SIMPLA: Pas 3 - Viraj dreapta");
    viraj_dreapta(); // Sau viraj_dreapta_puternic
    delay_ms(DELAY_VIRAJ_MS); // Folosește același DELAY_VIRAJ_MS (sau unul separat calibrat)
    oprire();
    delay_ms(DELAY_STOP_MS);
}