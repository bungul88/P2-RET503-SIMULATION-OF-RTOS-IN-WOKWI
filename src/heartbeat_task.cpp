#include "rtos.h"
#include "config.h"
#include "rt_stats.h"
#include "heartbeat_task.h"

static void heartbeatTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  bool ledOn = false;
  for (;;) {
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(HEARTBEAT_PERIOD_MS));
    stats_record(STAT_HEARTBEAT_LATE, rtos_ticksToMs(xTaskGetTickCount() - lastWake));
    ledOn = !ledOn;
    digitalWrite(PIN_LED_HEARTBEAT, ledOn);
  }
}

TaskHandle_t heartbeat_start() {
  pinMode(PIN_LED_HEARTBEAT, OUTPUT);
  TaskHandle_t h = nullptr;
  xTaskCreate(heartbeatTask, "Heartbeat", STACK_DEFAULT, nullptr, PRIO_HEARTBEAT, &h);
  return h;
}
