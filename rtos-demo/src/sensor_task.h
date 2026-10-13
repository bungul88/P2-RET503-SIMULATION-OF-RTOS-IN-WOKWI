#pragma once
#include "rtos.h"

// Task PRODUSEN: periodik 100 ms (vTaskDelayUntil), mengirim Sample ke queue.
TaskHandle_t sensor_start(QueueHandle_t queue);
