#pragma once
#include "rtos.h"

// Task KONSUMEN: tidur (Blocked) sampai ada data di queue, lalu olah:
// rata-rata bergerak -> tentukan level -> perbarui data bersama & LED alarm.
TaskHandle_t processor_start(QueueHandle_t queue);
