#include "motoare.h"
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include "utilitare.h"
#include "config.h"
#include <esp_log.h>
#include "comunicare_bluetooth.h"

static const char *TAG_MOTOARE = "MOTOARE_OCOLIRE";

void init_motoare() {
    
    gpio_set_direction(MOTOR_A_INAINTE, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_A_INAPOI, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_INAINTE, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_B_INAPOI, GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_MOTOR_A, GPIO_MODE_OUTPUT);
    gpio_set_direction(ENABLE_MOTOR_B, GPIO_MODE_OUTPUT);    

    ledc_timer_config_t pwm_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT, 
        .freq_hz = 1000,                     
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
    
    for (uint32_t i = 0; i < durata_ms; i += 10) {

        if (flag_oprire_urgenta) {

            oprire(); 
            return;
        }
        delay_ms(10);
    }
    
    oprire();
}

void ocolire_prin_stanga() {
    
    ESP_LOGI(TAG_MOTOARE, "Ocolire stanga: Pas 1 - Viraj stanga");
    viraj_stanga(); 
    delay_ms(DELAY_VIRAJ_MS); 
    oprire();
    delay_ms(DELAY_STOP_MS);

    ESP_LOGI(TAG_MOTOARE, "Ocolire stanga: Pas 2 - Mers inainte (lateral)");
    mers_inainte();
    delay_ms(DELAY_MERS_LATERAL_MS);
    oprire();
    delay_ms(DELAY_STOP_MS);

    ESP_LOGI(TAG_MOTOARE, "Ocolire stanga: Pas 3 - Viraj dreapta");
    viraj_dreapta(); 
    delay_ms(DELAY_VIRAJ_MS);
    oprire();
    delay_ms(DELAY_STOP_MS);
}
