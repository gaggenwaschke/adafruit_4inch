#pragma once

#include "canvas.hpp"
#include <stdint.h>

/********************************************************************************
function:
                        Macro definition variable name
********************************************************************************/

class Adafruit_4inch : public Canvas {
public:
  Adafruit_4inch(int pin_backlight, int pin_reset, int pin_chip_select,
                 int pin_dc, uint16_t backlight_lvl = 100,
                 ScanDirection scan_direction = ScanDirection::D2U_L2R);

  void init() override;

  void clear(COLOR Color) override;
  virtual LENGTH get_width() const override;
  virtual LENGTH get_height() const override;

private:
  // Driver specific functions
  void initialize_peripheral_registers();
  void init_io();
  void write_reg(uint8_t Reg);
  void write_data(uint8_t Data);
  void write_all_data(uint16_t data, uint32_t length);
  void reset();
  void write_backlight();
  void write_gram_scan_way();

  void set_window(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend);
  void set_cursor(POINT Xpoint, POINT Ypoint);
  void set_color(COLOR Color, POINT Xpoint, POINT Ypoint);

  // Canvas Interface implementation
  void set_point_i_color(POINT Xpoint, POINT Ypoint, COLOR Color) override;
  void set_area_i_color(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                        COLOR Color) override;

  int pin_backlight;
  int pin_reset;
  int pin_chip_select;
  int pin_dc;

  uint16_t backlight_lvl;
  ScanDirection scan_direction;

  LENGTH width;  // COLUMN
  LENGTH height; // PAGE

  POINT LCD_X_Adjust; // LCD x actual display position calibration
  POINT LCD_Y_Adjust; // LCD y actual display position calibration
};
