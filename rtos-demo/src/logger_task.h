#pragma once
#include "rtos.h"

// Task PELAPOR: tiap 1 detik mencetak ringkasan sistem (data, queue, latensi,
// sisa stack). Task lain didaftarkan lewat logger_register() agar stack-nya
// bisa dipantau.
void         logger_register(const char* name, TaskHandle_t handle);
TaskHandle_t logger_start(QueueHandle_t queue);
