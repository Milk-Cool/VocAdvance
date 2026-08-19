#pragma once
#include <stdlib.h>

#define SAVE_ADDR ((volatile uint8_t*)0x0e000000)
void save_write(size_t offset, uint8_t* data, size_t len);
void save_read(size_t offset, uint8_t* data, size_t len);