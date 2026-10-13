#include "rtos.h"
#include <stdarg.h>
#include <stdio.h>
#include "log.h"

static SemaphoreHandle_t s_mutex = nullptr;

void log_begin() {
  Serial.begin(115200);
  s_mutex = xSemaphoreCreateMutex();
}

// Satu mutex menjaga Serial: tanpa ini, tulisan dari beberapa task bisa
// tercampur di tengah baris.
void log_raw(const char* text) {
  xSemaphoreTake(s_mutex, portMAX_DELAY);
  for (const char* p = text; *p; ++p) {
    if (*p == '\n') Serial.write('\r');   // terminal Wokwi butuh \r\n
    Serial.write((uint8_t)*p);
  }
  xSemaphoreGive(s_mutex);
}

void log_event(const char* fmt, ...) {
  char msg[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(msg, sizeof(msg), fmt, ap);
  va_end(ap);

  char line[192];
  snprintf(line, sizeof(line), "[%7lu ms][%-9s] %s\n",
           (unsigned long)rtos_nowMs(), pcTaskGetName(NULL), msg);
  log_raw(line);
}
