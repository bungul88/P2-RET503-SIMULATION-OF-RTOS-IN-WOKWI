#include "rtos.h"
#include "config.h"
#include "types.h"
#include "log.h"
#include "shared_state.h"
#include "rt_stats.h"
#include "processor_task.h"

static QueueHandle_t s_queue = nullptr;

// Level dengan histeresis supaya tidak "bolak-balik" di sekitar ambang.
static Level classify(float avg, Level prev) {
  float warnOn  = THRESH_WARNING;
  float alarmOn = THRESH_ALARM;
  if (prev >= LEVEL_WARNING) warnOn  -= HYSTERESIS;
  if (prev == LEVEL_ALARM)   alarmOn -= HYSTERESIS;
  if (avg >= alarmOn) return LEVEL_ALARM;
  if (avg >= warnOn)  return LEVEL_WARNING;
  return LEVEL_NORMAL;
}

static void processorTask(void*) {
  uint16_t window[MOVING_AVG_WINDOW] = {0};
  int      idx = 0, filled = 0;
  uint32_t sum = 0;
  Level    prev = LEVEL_NORMAL;
  Sample   s;

  for (;;) {
    // BLOCKED sampai ada data: tidak memakai CPU selama menunggu.
    if (xQueueReceive(s_queue, &s, portMAX_DELAY) != pdTRUE) continue;

    // Berapa lama sampel ini menunggu sejak dibuat Sensor sampai diolah?
    stats_record(STAT_PIPELINE_LATENCY, rtos_nowMs() - s.tickMs);

    sum -= window[idx];
    window[idx] = s.raw;
    sum += s.raw;
    idx = (idx + 1) % MOVING_AVG_WINDOW;
    if (filled < MOVING_AVG_WINDOW) filled++;
    float avg = (float)sum / (float)filled;

    Level level = classify(avg, prev);
    digitalWrite(PIN_LED_ALARM, level == LEVEL_ALARM ? HIGH : LOW);
    state_update(s.raw, avg, level);

    if (level != prev) {
      log_event("level %s -> %s (rata-rata=%.0f)", levelName(prev), levelName(level), avg);
      prev = level;
    }
  }
}

TaskHandle_t processor_start(QueueHandle_t queue) {
  s_queue = queue;
  pinMode(PIN_LED_ALARM, OUTPUT);
  TaskHandle_t h = nullptr;
  xTaskCreate(processorTask, "Processor", STACK_DEFAULT, nullptr, PRIO_PROCESSOR, &h);
  return h;
}
