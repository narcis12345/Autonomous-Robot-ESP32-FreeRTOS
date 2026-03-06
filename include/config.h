#ifndef CONFIG_H
#define CONFIG_H

#include <driver/gpio.h>

//Constante
extern const float OFFSET_BRAT_MM;

//Pini motoare
#define MOTOR_A_INAPOI GPIO_NUM_26 
#define MOTOR_A_INAINTE GPIO_NUM_27
#define MOTOR_B_INAPOI GPIO_NUM_21 
#define MOTOR_B_INAINTE GPIO_NUM_19
#define ENABLE_MOTOR_A GPIO_NUM_25
#define ENABLE_MOTOR_B GPIO_NUM_18

//Pini servo
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA GPIO_NUM_22
#define I2C_MASTER_SCL GPIO_NUM_23
#define I2C_MASTER_FREQ_HZ 100000
#define PCA9685_ADDR 0x40
#define SERVO_CANAL_SCANARE 0
//#define SERVO_CANAL_STABILITATE 8
//#define SERVO_CANAL_VERTICAL 4

//Pin detector + constante
#define PIN_SEMNAL_METAL GPIO_NUM_4
#define NUMAR_CITIRI_PENTRU_CONFIRMARE 10
#define PRAG_DETECTII_POZITIVE_NECESARE 5

//Constante pentru unghiuri servo
#define UNGHI_CENTRU 120
#define UNGHI_STANGA (UNGHI_CENTRU + 90)  
#define UNGHI_DREAPTA (UNGHI_CENTRU - 90)
//#define UNGHI_STABILITATE 80
//#define UNGHI_VERTICAL 40

//Constante delay-uri
#define DELAY_VIRAJ_MS 700
#define DELAY_MERS_LATERAL_MS 800
#define DELAY_STOP_MS 300
#define DELAY_INTRE_CITIRI_MS 5
#define DURATA_AVANSARE_NORMAL_MS 600
#define DELAY_PAS_SERVO_SCANARE_MS 5

//Alte define-uri
#define BUTON_START GPIO_NUM_0
#define LED_TEST GPIO_NUM_2

#endif
