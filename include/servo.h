#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include "stdbool.h"

void init_servo();
void seteaza_servo(int canal, int unghi);
bool miscare_brat_si_detectie(int delay_pe_pas_ms);
int get_unghi_ultima_detectie();

#endif