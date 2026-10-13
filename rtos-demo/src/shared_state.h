#pragma once
#include "types.h"

// Modul DATA BERSAMA: dibaca/ditulis banyak task, dilindungi MUTEX di dalam modul.
struct SystemState {
  uint16_t lastRaw;
  float    average;
  Level    level;
  uint32_t samples;   // jumlah sampel yang sudah diproses
  uint32_t dropped;   // sampel terbuang karena queue penuh
  bool     stress;    // mode beban CPU
};

void        state_begin();
void        state_update(uint16_t raw, float avg, Level level);  // dipanggil Processor
void        state_countDropped();                                  // dipanggil Sensor
void        state_setStress(bool on);                              // dipanggil Button
bool        state_stress();                                        // dipanggil Load
SystemState state_snapshot();                                      // dipanggil Logger
