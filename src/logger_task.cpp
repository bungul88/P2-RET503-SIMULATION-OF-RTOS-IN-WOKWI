#include "rtos.h"
#include <stdio.h>
#include "config.h"
#include "log.h"
#include "shared_state.h"
#include "rt_stats.h"
#include "logger_task.h"

struct Registered { const char* name; TaskHandle_t handle; };
static Registered   s_reg[8];
static int          s_regCount = 0;
static QueueHandle_t s_queue   = nullptr;

void logger_register(const char* name, TaskHandle_t handle) {
  if (s_regCount < 8) s_reg[s_regCount++] = { name, handle };
}

static void loggerTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();

  for (;;) {
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(LOGGER_PERIOD_MS));
    stats_record(STAT_LOGGER_LATE, rtos_ticksToMs(xTaskGetTickCount() - lastWake));

    SystemState st = state_snapshot();

    // Seluruh laporan disusun di satu buffer lalu dicetak SEKALI (atomik),
    // supaya log dari task lain tidak menyelip di tengah laporan.
    char buf[640];
    int n = 0;
    n += snprintf(buf + n, sizeof(buf) - n,
        "======== LAPORAN  t=%lu ms ========\n", (unsigned long)rtos_nowMs());
    n += snprintf(buf + n, sizeof(buf) - n,
        "STRESS       : %s\n", st.stress ? "ON  (Load membakar CPU)" : "OFF");
    n += snprintf(buf + n, sizeof(buf) - n,
        "Sensor       : raw=%u avg=%.0f level=%s  diproses=%lu  dibuang=%lu\n",
        (unsigned)st.lastRaw, st.average, levelName(st.level),
        (unsigned long)st.samples, (unsigned long)st.dropped);
    n += snprintf(buf + n, sizeof(buf) - n,
        "Queue        : terisi %u/%d\n",
        (unsigned)uxQueueMessagesWaiting(s_queue), QUEUE_LENGTH);
    n += snprintf(buf + n, sizeof(buf) - n,
        "Latensi maks : Sensor=%lu  Heartbeat=%lu  Logger=%lu  Pipeline=%lu (ms)\n",
        (unsigned long)stats_takeMax(STAT_SENSOR_LATE),
        (unsigned long)stats_takeMax(STAT_HEARTBEAT_LATE),
        (unsigned long)stats_takeMax(STAT_LOGGER_LATE),
        (unsigned long)stats_takeMax(STAT_PIPELINE_LATENCY));
    n += snprintf(buf + n, sizeof(buf) - n, "Sisa stack   :");
    for (int i = 0; i < s_regCount; i++) {
      n += snprintf(buf + n, sizeof(buf) - n, " %s=%u", s_reg[i].name,
                    (unsigned)uxTaskGetStackHighWaterMark(s_reg[i].handle));
    }
    n += snprintf(buf + n, sizeof(buf) - n, " (byte)\n");

    log_raw(buf);
  }
}

TaskHandle_t logger_start(QueueHandle_t queue) {
  s_queue = queue;
  TaskHandle_t h = nullptr;
  xTaskCreate(loggerTask, "Logger", STACK_LOGGER, nullptr, PRIO_LOGGER, &h);
  return h;
}
