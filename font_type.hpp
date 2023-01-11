#pragma once

#include <stdint.h>

struct Font {
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;
};
