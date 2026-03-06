#ifndef COMUNICARE_BLUETOOTH_H
#define COMINICARE_BLUETOOTH_H

#include <freertos/semphr.h>

extern SemaphoreHandle_t semafor_semnal_start;
extern volatile bool flag_oprire_urgenta;

void init_bluetooth();
void trimite_mesaj_bluetooth(const char* mesaj);

#endif
