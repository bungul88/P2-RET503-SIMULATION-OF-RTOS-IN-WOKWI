#include "rtos.h"
#include "config.h"
#include "log.h"
#include "shared_state.h"
#include "button_task.h"

static TaskHandle_t s_handle = nullptr;

// ISR: sesingkat mungkin. Hanya memberi notifikasi ke Button task.
static void IRAM_ATTR onButtonIsr() {
  BaseType_t woken = pdFALSE;
  vTaskNotifyGiveFromISR(s_handle, &woken);
  if (woken == pdTRUE) portYIELD_FROM_ISR();   // langsung pindah ke Button task
}

static void buttonTask(void*) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);        // BLOCKED sampai ISR memberi sinyal
    vTaskDelay(pdMS_TO_TICKS(30));                  // debounce
    if (digitalRead(PIN_BUTTON) == LOW) {
      bool on = !state_stress();
      state_setStress(on);
      digitalWrite(PIN_LED_STRESS, on ? HIGH : LOW);
      log_event("tombol ditekan -> mode STRESS %s", on ? "ON" : "OFF");
    }
    vTaskDelay(pdMS_TO_TICKS(150));                 // abaikan pantulan berikutnya
    ulTaskNotifyTake(pdTRUE, 0);                    // buang notifikasi yang menumpuk
  }
}

TaskHandle_t button_start() {
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED_STRESS, OUTPUT);
  xTaskCreate(buttonTask, "Button", STACK_DEFAULT, nullptr, PRIO_BUTTON, &s_handle);
  // Pasang interupsi SETELAH task (dan handle-nya) ada.
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), onButtonIsr, FALLING);
  return s_handle;
}
