#include "rtos.h"
#include "shared_state.h"

static SemaphoreHandle_t s_mutex = nullptr;
static SystemState s_state = {0, 0.0f, LEVEL_NORMAL, 0, 0, false};

void state_begin() { s_mutex = xSemaphoreCreateMutex(); }

void state_update(uint16_t raw, float avg, Level level) {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  s_state.lastRaw = raw;
  s_state.average = avg;
  s_state.level   = level;
  s_state.samples++;
  xSemaphoreGive(s_mutex);
}

void state_countDropped() {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  s_state.dropped++;
  xSemaphoreGive(s_mutex);
}

void state_setStress(bool on) {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  s_state.stress = on;
  xSemaphoreGive(s_mutex);
}

bool state_stress() {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  bool v = s_state.stress;
  xSemaphoreGive(s_mutex);
  return v;
}

SystemState state_snapshot() {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  SystemState copy = s_state;   // salin utuh saat dikunci
  xSemaphoreGive(s_mutex);
  return copy;
}
