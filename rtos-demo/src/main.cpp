#include "rtos.h"
#include "config.h"
#include "types.h"
#include "log.h"
#include "shared_state.h"
#include "rt_stats.h"
#include "sensor_task.h"
#include "processor_task.h"
#include "button_task.h"
#include "load_task.h"
#include "heartbeat_task.h"
#include "logger_task.h"

void setup() {
  // 1) Siapkan modul pendukung (membuat mutex) SEBELUM ada task yang memakainya.
  log_begin();
  state_begin();
  stats_begin();

  // 2) Buat queue penghubung Sensor -> Processor.
  QueueHandle_t queue = xQueueCreate(QUEUE_LENGTH, sizeof(Sample));
  if (queue == nullptr) {
    log_event("GAGAL membuat queue");
    return;
  }

  // 3) Buat semua task. Task yang prioritasnya lebih tinggi dari loopTask (1)
  //    langsung berjalan begitu dibuat.
  logger_register("Button",    button_start());
  logger_register("Sensor",    sensor_start(queue));
  logger_register("Load",      load_start());
  logger_register("Processor", processor_start(queue));
  logger_register("Heartbeat", heartbeat_start());
  logger_register("Logger",    logger_start(queue));

  log_raw("\n=== DEMO RTOS (FreeRTOS) - ESP32-S2 ===\n"
          "Task        Prioritas  Jadwal\n"
          "Button      5          event (interupsi tombol)\n"
          "Sensor      4          periodik 100 ms\n"
          "Load        3          aktif hanya saat STRESS\n"
          "Processor   2          event (data di queue)\n"
          "Logger      1          periodik 1000 ms\n"
          "Heartbeat   1          periodik 500 ms\n"
          "Tekan tombol = STRESS ON/OFF. Putar potensiometer = ubah level.\n\n");
}

// loop() sendiri adalah sebuah task FreeRTOS ("loopTask", prioritas 1).
// Semua pekerjaan sudah dipindah ke task di atas, jadi di sini cukup tidur.
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}
