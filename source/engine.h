#pragma once
#include <stdint.h>

uint32_t mibis();
void reset_timers();
void init_engine();
void play_note(const char* lyric, int16_t pitch, uint32_t start_mibis, uint16_t duration_mibis);
void play_note_now(const char* lyric, int16_t pitch, uint32_t duration_mibis);
void loop_engine();
uint8_t get_max_new_notes();