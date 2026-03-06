#ifndef MOTOARE_H
#define MOTOARE_H

#include <stdint.h>

void init_motoare();

void mers_inainte();
void mers_inainte_controlat(uint32_t durata_ms);
void mers_inapoi();

void viraj_stanga();
void viraj_dreapta();

void oprire();

void asteapta_start();

void ocolire_prin_stanga();

#endif