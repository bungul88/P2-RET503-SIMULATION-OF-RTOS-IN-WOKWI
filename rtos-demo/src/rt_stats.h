#pragma once
#include <stdint.h>

// Modul STATISTIK WAKTU: mencatat keterlambatan terbesar (ms) tiap pengukuran
// sejak laporan terakhir. Inilah "alat ukur" untuk melihat efek prioritas.
enum StatId {
  STAT_SENSOR_LATE,      // Sensor bangun terlambat berapa ms dari jadwal
  STAT_HEARTBEAT_LATE,   // idem untuk Heartbeat
  STAT_LOGGER_LATE,      // idem untuk Logger
  STAT_PIPELINE_LATENCY, // lama sampel menunggu di queue sebelum diolah Processor
  STAT_COUNT
};

void     stats_begin();
void     stats_record(StatId id, uint32_t valueMs);
uint32_t stats_takeMax(StatId id);   // ambil nilai maksimum lalu reset
