#pragma once
// Header gabungan: Arduino + FreeRTOS + beberapa helper waktu.
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

#ifndef portTICK_PERIOD_MS
#define portTICK_PERIOD_MS (1000 / configTICK_RATE_HZ)
#endif

// Waktu sekarang dalam ms, berdasarkan tick RTOS (1 tick = 1 ms pada ESP32).
static inline uint32_t rtos_nowMs() { return (uint32_t)xTaskGetTickCount() * portTICK_PERIOD_MS; }
// Selisih tick -> ms.
static inline uint32_t rtos_ticksToMs(TickType_t t) { return (uint32_t)t * portTICK_PERIOD_MS; }
