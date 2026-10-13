# Demo RTOS (FreeRTOS) - ESP32-S2 di Wokwi for VS Code + PlatformIO

Simulasi yang menunjukkan cara kerja RTOS: multi-task, prioritas, penjadwalan
periodik, queue, mutex, dan interupsi -> task. Tidak butuh library tambahan
(FreeRTOS sudah menjadi bagian framework Arduino-ESP32).

## 1. Skenario
Sistem "monitor sensor" kecil:

    Potensiometer -> [Sensor 100 ms] --queue--> [Processor] -> data bersama (mutex)
                                                    |                |
                                          LED ALARM (merah)    [Logger 1 s] -> Serial
    Tombol -> interupsi -> [Button] -> mode STRESS -> [Load] membakar CPU
                                       [Heartbeat 500 ms] -> LED biru

| Task      | Prio | Jadwal                      | Peran |
|-----------|------|-----------------------------|-------|
| Button    | 5    | event (interupsi tombol)    | debounce, toggle mode STRESS |
| Sensor    | 4    | periodik 100 ms             | baca ADC, kirim ke queue |
| Load      | 3    | aktif hanya saat STRESS     | beban CPU buatan (sibuk 40 ms, tidur 10 ms) |
| Processor | 2    | event (data masuk queue)    | rata-rata bergerak, tentukan level, nyalakan LED alarm |
| Logger    | 1    | periodik 1000 ms            | cetak laporan |
| Heartbeat | 1    | periodik 500 ms             | kedip LED biru |
| loopTask  | 1    | (bawaan Arduino)            | hanya tidur |

## 2. Struktur modul
    include/config.h        semua pin, prioritas, periode, ambang
    src/main.cpp            membuat queue + semua task
    src/rtos.h              header gabungan + helper waktu
    src/types.h             Sample, Level
    src/log.*               cetak serial aman multi-task (MUTEX)
    src/shared_state.*      data bersama (MUTEX di dalam modul)
    src/rt_stats.*          pencatat latensi maksimum
    src/sensor_task.*       produsen (vTaskDelayUntil, xQueueSend)
    src/processor_task.*    konsumen (xQueueReceive, blocked)
    src/button_task.*       ISR -> task notification
    src/load_task.*         beban CPU buatan
    src/heartbeat_task.*    task periodik sederhana
    src/logger_task.*       laporan berkala

## 3. Konsep RTOS -> di mana di kode
| Konsep | Lokasi |
|---|---|
| Task + prioritas | `xTaskCreate(...)` di tiap `*_start()`, nilai di `config.h` |
| Periodik presisi | `vTaskDelayUntil` di sensor/heartbeat/logger |
| Blocked (tidak makan CPU) | `xQueueReceive(..., portMAX_DELAY)`, `ulTaskNotifyTake` |
| Preemption | Sensor (4) memotong Load (3); Load (3) memotong Processor (2) |
| Queue | `xQueueCreate/Send/Receive` Sensor -> Processor |
| Mutex | `log.cpp`, `shared_state.cpp`, `rt_stats.cpp` |
| ISR -> task (deferred handling) | `button_task.cpp` (`vTaskNotifyGiveFromISR`) |
| Pemantauan | `uxTaskGetStackHighWaterMark`, `uxQueueMessagesWaiting` |

## 4. Wiring (ESP32-S2-DevKitM-1)
Resistor 220 ohm di setiap LED.

| Dari | Ke |
|---|---|
| GPIO4 | resistor -> LED hijau (A); LED (C) -> GND |
| GPIO5 | resistor -> LED biru (A);  LED (C) -> GND |
| GPIO6 | resistor -> LED merah (A);  LED (C) -> GND |
| GPIO7 | resistor -> LED kuning (A); LED (C) -> GND |
| GPIO8 | push button kaki 1; kaki 2 (sisi lain) -> GND |
| GPIO1 | potensiometer SIG |
| 3V3   | potensiometer VCC |
| GND   | potensiometer GND |

Tanpa potensiometer? Set `USE_POTENTIOMETER 0` di `config.h` (sensor sintetis).

## 5. Menjalankan
1. `pio run`
2. F1 -> "Wokwi: Start Simulator"
3. Putar potensiometer: lihat `raw`, `avg`, `level`, dan LED merah.
4. Klik push button: STRESS ON/OFF (LED kuning). Bandingkan baris "Latensi maks".

## 6. Membaca laporan (tiap 1 detik)
    ======== LAPORAN  t=4028 ms ========
    STRESS       : ON  (Load membakar CPU)
    Sensor       : raw=3600 avg=3188 level=WARNING  diproses=40  dibuang=0
    Queue        : terisi 0/8
    Latensi maks : Sensor=0  Heartbeat=33  Logger=28  Pipeline=33 (ms)
    Sisa stack   : Button=... Sensor=... (byte)

| Field | Arti |
|---|---|
| raw / avg / level | nilai ADC terakhir / rata-rata 8 sampel / NORMAL-WARNING-ALARM |
| diproses | sampel yang sudah diolah Processor |
| dibuang | sampel yang gagal masuk queue karena penuh |
| Queue | isi queue saat laporan |
| Latensi Sensor/Heartbeat/Logger | keterlambatan bangun terbesar (ms) dibanding jadwal, sejak laporan lalu |
| Latensi Pipeline | waktu terlama sampel menunggu sebelum diolah Processor |
| Sisa stack | stack minimum yang pernah tersisa tiap task (makin kecil = makin mepet) |

Hasil yang diharapkan (diuji di komputer di atas kernel FreeRTOS asli; angka di Wokwi bisa sedikit berbeda):
- STRESS OFF: semua latensi 0 ms.
- STRESS ON: Sensor tetap 0 ms (prioritas 4 > Load 3), tetapi Heartbeat, Logger, Pipeline naik ke ~30-35 ms.

Ambang level (2000 / 3200) mengikuti skala ADC. Jika potensiometer di Wokwi
tidak mencapai ALARM, lihat nilai `raw` maksimum lalu sesuaikan
`THRESH_WARNING` / `THRESH_ALARM` di `config.h`.

## 7. Eksperimen untuk laporan (ubah `config.h`, build ulang)
1. **Prioritas terbalik**: `PRIO_LOAD = 5` (di atas Sensor). Saat STRESS ON,
   latensi Sensor naik dari 0 menjadi puluhan ms. Prioritas menentukan siapa
   yang tetap tepat waktu.
2. **Naikkan Processor**: `PRIO_PROCESSOR = 4`. Saat STRESS ON, latensi
   Pipeline turun ke 0 (Processor kini memotong Load).
3. **Queue penuh**: `LOAD_BURST_MS = 1500`. Processor kelaparan (starvation) lebih dari
   1 detik, queue (8 slot) penuh, kolom `dibuang` bertambah. Laporan Logger
   juga bisa muncul beruntun karena `vTaskDelayUntil` mengejar jadwal yang terlewat.
4. **Kecilkan queue**: `QUEUE_LENGTH = 2` lalu ulangi eksperimen 3.
5. **Tanpa mutex** (opsional): di `log.cpp`, hapus `xSemaphoreTake/Give` lalu
   amati apakah teks dari task berbeda bisa bercampur.

## 8. Catatan
- ESP32-S2 hanya punya 1 core, jadi semua task berbagi satu CPU. Itu sebabnya
  Load bisa mengganggu task lain.
- Pakai `\r\n` untuk serial (sudah ditangani `log_raw`); terminal Wokwi butuh `\r`.
- Di Wokwi web: upload semua file di `src/` dan `include/config.h` sejajar
  (tanpa folder), kosongkan `sketch.ino`. Library tidak diperlukan.
