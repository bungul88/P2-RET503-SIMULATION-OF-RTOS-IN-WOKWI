#include "rtos.h"
#include "rt_stats.h"

static SemaphoreHandle_t s_mutex = nullptr;
static uint32_t s_max[STAT_COUNT];

void stats_begin() {
  s_mutex = xSemaphoreCreateMutex();
  for (int i = 0; i < STAT_COUNT; i++) s_max[i] = 0;
}

void stats_record(StatId id, uint32_t valueMs) {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  if (valueMs > s_max[id]) s_max[id] = valueMs;
  xSemaphoreGive(s_mutex);
}

uint32_t stats_takeMax(StatId id) {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  uint32_t v = s_max[id];
  s_max[id] = 0;
  xSemaphoreGive(s_mutex);
  return v;
}
