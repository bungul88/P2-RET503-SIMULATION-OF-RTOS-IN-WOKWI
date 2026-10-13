#pragma once
#include "rtos.h"

// Beban CPU buatan. Saat mode STRESS aktif, task ini "membakar" CPU 40 ms lalu
// tidur 10 ms. Karena prioritasnya (3) di atas Processor (2), Logger (1), dan
// Heartbeat (1), ketiganya akan terlambat; Sensor (4) dan Button (5) tidak.
TaskHandle_t load_start();
