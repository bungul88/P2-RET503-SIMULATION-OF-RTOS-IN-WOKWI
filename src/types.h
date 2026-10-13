#pragma once
#include <stdint.h>

// Satu pembacaan sensor yang dikirim lewat queue.
struct Sample {
  uint32_t tickMs;   // waktu pengambilan sampel (ms sejak boot)
  uint16_t raw;      // nilai ADC 0..4095
};

enum Level : uint8_t { LEVEL_NORMAL = 0, LEVEL_WARNING = 1, LEVEL_ALARM = 2 };

static inline const char* levelName(Level l) {
  switch (l) {
    case LEVEL_NORMAL:  return "NORMAL";
    case LEVEL_WARNING: return "WARNING";
    default:            return "ALARM";
  }
}
