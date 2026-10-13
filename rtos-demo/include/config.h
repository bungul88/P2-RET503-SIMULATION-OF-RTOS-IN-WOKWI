#pragma once
// =====================================================================
//  config.h - semua pengaturan demo RTOS ada di sini.
//  Ubah nilai di sini untuk bereksperimen (lihat README.md bagian Eksperimen).
// =====================================================================
#include <stdint.h>

// ---------- Pin ESP32-S2 ----------
constexpr int PIN_LED_SENSOR    = 4;   // LED hijau  : berkedip tiap sampel sensor
constexpr int PIN_LED_HEARTBEAT = 5;   // LED biru   : detak sistem (Heartbeat task)
constexpr int PIN_LED_ALARM     = 6;   // LED merah  : menyala saat level = ALARM
constexpr int PIN_LED_STRESS    = 7;   // LED kuning : menyala saat mode STRESS ON
constexpr int PIN_BUTTON        = 8;   // push button ke GND (INPUT_PULLUP)
constexpr int PIN_POT           = 1;   // wiper potensiometer (ADC1_CH0)

// 1 = baca potensiometer, 0 = sensor sintetis (gelombang segitiga, tanpa komponen)
#define USE_POTENTIOMETER 1

// ---------- Prioritas task (angka BESAR = prioritas TINGGI) ----------
// Catatan: loopTask bawaan Arduino berprioritas 1, task idle berprioritas 0.
constexpr int PRIO_BUTTON    = 5;  // event dari interupsi -> harus paling responsif
constexpr int PRIO_SENSOR    = 4;  // periodik 100 ms -> harus tepat waktu
constexpr int PRIO_LOAD      = 3;  // beban CPU buatan (hanya aktif saat STRESS)
constexpr int PRIO_PROCESSOR = 2;  // pengolah data, kalah oleh Load
constexpr int PRIO_LOGGER    = 1;  // laporan, paling rendah
constexpr int PRIO_HEARTBEAT = 1;

// ---------- Ukuran stack (byte pada ESP32) ----------
constexpr uint32_t STACK_DEFAULT = 3072;
constexpr uint32_t STACK_LOGGER  = 4096;

// ---------- Periode (ms) ----------
constexpr uint32_t SENSOR_PERIOD_MS    = 100;
constexpr uint32_t HEARTBEAT_PERIOD_MS = 500;
constexpr uint32_t LOGGER_PERIOD_MS    = 1000;

// ---------- Queue Sensor -> Processor ----------
constexpr int QUEUE_LENGTH = 8;

// ---------- Pengolahan data ----------
constexpr int   MOVING_AVG_WINDOW = 8;
constexpr float THRESH_WARNING    = 2000.0f;  // skala ADC 0..4095
constexpr float THRESH_ALARM      = 3200.0f;
constexpr float HYSTERESIS        = 100.0f;

// ---------- Beban CPU buatan (mode STRESS) ----------
constexpr uint32_t LOAD_BURST_MS = 40;  // sibuk menghitung 40 ms ...
constexpr uint32_t LOAD_REST_MS  = 10;  // ... lalu tidur 10 ms (beban ~80%)
