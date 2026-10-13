#pragma once
// Modul LOG: pencetakan serial yang AMAN dipakai banyak task (dilindungi MUTEX).
void log_begin();
void log_raw(const char* text);                                  // cetak apa adanya (\n -> \r\n)
void log_event(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
// log_event menambahkan prefix: [waktu ms][nama task]
