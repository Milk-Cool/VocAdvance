#include "save.h"
#include <string.h>

void save_write(size_t offset, uint8_t* data, size_t len) {
    memcpy(SAVE_ADDR + offset, data, len);
}
void save_read(size_t offset, uint8_t* data, size_t len) {
    memcpy(data, SAVE_ADDR + offset, len);
}