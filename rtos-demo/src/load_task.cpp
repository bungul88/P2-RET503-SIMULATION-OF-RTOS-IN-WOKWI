#include "rtos.h"
#include "config.h"
#include "shared_state.h"
#include "load_task.h"

static void loadTask(void*) {
  for (;;) {
    if (state_stress()) {
      uint32_t t0 = millis();
      while (millis() - t0 < LOAD_BURST_MS) { /* sibuk: tidak pernah Blocked */ }
      vTaskDelay(pdMS_TO_TICKS(LOAD_REST_MS)); // beri kesempatan task idle & lainnya
    } else {
      vTaskDelay(pdMS_TO_TICKS(50));           // mode normal: tidur, tidak memakan CPU
    }
  }
}

TaskHandle_t load_start() {
  TaskHandle_t h = nullptr;
  xTaskCreate(loadTask, "Load", STACK_DEFAULT, nullptr, PRIO_LOAD, &h);
  return h;
}
