#pragma once
#include "rtos.h"

// Interupsi tombol -> task notification -> Button task (prioritas tertinggi).
// Pola "deferred interrupt handling": ISR hanya membangunkan task, kerja
// sebenarnya (debounce, ubah mode, log) dilakukan di task.
TaskHandle_t button_start();
