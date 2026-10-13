#include "rtos.h"
#include "config.h"
#include "types.h"
#include "log.h"
#include "shared_state.h"
#include "rt_stats.h"
#include "sensor_task.h"

static QueueHandle_t s_queue = nullptr;

static uint16_t readSensor(uint32_t n) {
#if USE_POTENTIOMETER
  (void)n;
  return (uint16_t)analogRead(PIN_POT);          // 0..4095
#else
  uint32_t p = n % 200;                          // segitiga, periode 20 detik
  uint32_t v = (p < 100 ? p : 200 - p) * 41;
  return (uint16_t)(v > 4095 ? 4095 : v);
#endif
}

static void sensorTask(void*) {
  TickType_t lastWake = xTaskGetTickCount();
  bool ledOn = false;
  uint32_t n = 0;

  for (;;) {
    // Tidur sampai jadwal berikutnya. vTaskDelayUntil (bukan vTaskDelay) menjaga
    // periode tetap 100 ms walaupun isi loop memakan waktu.
    vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(SENSOR_PERIOD_MS));

    // Setelah vTaskDelayUntil, lastWake = waktu JADWAL bangun. Selisihnya dengan
    // waktu sekarang = keterlambatan task ini (latensi penjadwalan).
    stats_record(STAT_SENSOR_LATE, rtos_ticksToMs(xTaskGetTickCount() - lastWake));

    Sample s = { rtos_nowMs(), readSensor(n++) };
    if (xQueueSend(s_queue, &s, 0) != pdTRUE) {   // timeout 0: jangan menunggu
      state_countDropped();                        // queue penuh -> sampel terbuang
    }

    ledOn = !ledOn;
    digitalWrite(PIN_LED_SENSOR, ledOn);
  }
}

TaskHandle_t sensor_start(QueueHandle_t queue) {
  s_queue = queue;
  pinMode(PIN_LED_SENSOR, OUTPUT);
#if USE_POTENTIOMETER
  pinMode(PIN_POT, INPUT);
  analogReadResolution(12);   // <-- BARIS BARU: paksa ADC 12-bit (0..4095)
#endif
  TaskHandle_t h = nullptr;
  xTaskCreate(sensorTask, "Sensor", STACK_DEFAULT, nullptr, PRIO_SENSOR, &h);
  return h;
}
