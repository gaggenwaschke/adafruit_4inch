#include "adafruit_4inch_driver.hpp"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

Adafruit_4inch::Adafruit_4inch(int pin_backlight, int pin_reset,
                               int pin_chip_select, int pin_dc,
                               uint16_t backlight_level,
                               ScanDirection scan_direction)
    : pin_backlight{pin_backlight}, pin_reset{pin_reset},
      pin_chip_select{pin_chip_select}, pin_dc{pin_dc},
      backlight_lvl{backlight_lvl}, scan_direction{scan_direction} {}

/*******************************************************************************
function:
        Hardware reset
*******************************************************************************/
void Adafruit_4inch::reset() {
  digitalWrite(pin_reset, HIGH);
  delay(500);
  digitalWrite(pin_reset, LOW);
  delay(500);
  digitalWrite(pin_reset, HIGH);
  delay(500);
}

void Adafruit_4inch::write_backlight() {
  analogWrite(pin_backlight, backlight_lvl);
}

/*******************************************************************************
function:
                Write register address and data
*******************************************************************************/

void Adafruit_4inch::init_io() {
  pinMode(pin_chip_select, OUTPUT);
  pinMode(pin_reset, OUTPUT);
  pinMode(pin_dc, OUTPUT);
  pinMode(pin_backlight, OUTPUT);

  // set Serial
  Serial.begin(115200);

  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();
}

void Adafruit_4inch::write_reg(uint8_t Reg) {
  digitalWrite(pin_dc, LOW);
  digitalWrite(pin_chip_select, LOW);
  SPI.transfer(Reg);
  digitalWrite(pin_chip_select, HIGH);
}

void Adafruit_4inch::write_data(uint8_t Data) {
  digitalWrite(pin_dc, HIGH);
  digitalWrite(pin_chip_select, LOW);
  SPI.transfer(Data >> 8);
  SPI.transfer(Data & 0XFF);
  digitalWrite(pin_chip_select, HIGH);
}

/*******************************************************************************
function:
                Write register data
*******************************************************************************/
void Adafruit_4inch::write_all_data(uint16_t Data, uint32_t DataLen) {
  uint32_t i;
  digitalWrite(pin_dc, HIGH);
  digitalWrite(pin_chip_select, LOW);
  for (i = 0; i < DataLen; i++) {
    SPI.transfer(Data >> 8);
    SPI.transfer(Data & 0XFF);
  }
  digitalWrite(pin_chip_select, HIGH);
}

/*******************************************************************************
function:
                Common register initialization
*******************************************************************************/
void Adafruit_4inch::initialize_peripheral_registers() {
  write_reg(0XF9);
  write_data(0x00);
  write_data(0x08);

  write_reg(0xC0);
  write_data(0x19); // VREG1OUT POSITIVE
  write_data(0x1a); // VREG2OUT NEGATIVE

  write_reg(0xC1);
  write_data(0x45); // VGH,VGL    VGH>=14V.
  write_data(0x00);

  write_reg(0xC2); // Normal mode, increase can change the display quality,
                   // while increasing power consumption
  write_data(0x33);

  write_reg(0XC5);
  write_data(0x00);
  write_data(0x28); // VCM_REG[7:0]. <=0X80.

  write_reg(0xB1);  // Sets the frame frequency of full color normal mode
  write_data(0xA0); // 0XB0 =70HZ, <=0XB0.0xA0=62HZ
  write_data(0x11);

  write_reg(0xB4);
  write_data(0x02); // 2 DOT FRAME MODE,F<=70HZ.

  write_reg(0xB6); //
  write_data(0x00);
  write_data(0x42); // 0 GS SS SM ISC[3:0];
  write_data(0x3B);

  write_reg(0xB7);
  write_data(0x07);

  write_reg(0xE0);
  write_data(0x1F);
  write_data(0x25);
  write_data(0x22);
  write_data(0x0B);
  write_data(0x06);
  write_data(0x0A);
  write_data(0x4E);
  write_data(0xC6);
  write_data(0x39);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);
  write_data(0x00);

  write_reg(0XE1);
  write_data(0x1F);
  write_data(0x3F);
  write_data(0x3F);
  write_data(0x0F);
  write_data(0x1F);
  write_data(0x0F);
  write_data(0x46);
  write_data(0x49);
  write_data(0x31);
  write_data(0x05);
  write_data(0x09);
  write_data(0x03);
  write_data(0x1C);
  write_data(0x1A);
  write_data(0x00);

  write_reg(0XF1);
  write_data(0x36);
  write_data(0x04);
  write_data(0x00);
  write_data(0x3C);
  write_data(0x0F);
  write_data(0x0F);
  write_data(0xA4);
  write_data(0x02);

  write_reg(0XF2);
  write_data(0x18);
  write_data(0xA3);
  write_data(0x12);
  write_data(0x02);
  write_data(0x32);
  write_data(0x12);
  write_data(0xFF);
  write_data(0x32);
  write_data(0x00);

  write_reg(0XF4);
  write_data(0x40);
  write_data(0x00);
  write_data(0x08);
  write_data(0x91);
  write_data(0x04);

  write_reg(0XF8);
  write_data(0x21);
  write_data(0x04);

  write_reg(0X3A); // Set Interface Pixel Format
  write_data(0x55);
}

/********************************************************************************
function:
        initialization
********************************************************************************/
void Adafruit_4inch::init() {
  init_io();

  // Hardware reset
  reset();

  write_backlight();

  // Set the initialization register
  initialize_peripheral_registers();

  // Set the display scan and color transfer modes
  write_gram_scan_way();
  delay(200);

  // sleep out
  write_reg(0x11);
  delay(120);

  // Turn on the LCD display
  write_reg(0x29);
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
        Xstart 	:   X direction Start coordinates
        Ystart  :   Y direction Start coordinates
        Xend    :   X direction end coordinates
        Yend    :   Y direction end coordinates
********************************************************************************/
void Adafruit_4inch::set_window(POINT Xstart, POINT Ystart, POINT Xend,
                                POINT Yend) {
  // set the X coordinates
  write_reg(0x2A);
  write_data(Xstart >>
             8); // Set the horizontal starting point to the high octet
  write_data(Xstart &
             0xff); // Set the horizontal starting point to the low octet
  write_data((Xend - 1) >> 8);   // Set the horizontal end to the high octet
  write_data((Xend - 1) & 0xff); // Set the horizontal end to the low octet

  // set the Y coordinates
  write_reg(0x2B);
  write_data(Ystart >> 8);
  write_data(Ystart & 0xff);
  write_data((Yend - 1) >> 8);
  write_data((Yend - 1) & 0xff);
  write_reg(0x2C);
}

/********************************************************************************
function:	Set the display point (Xpoint, Ypoint)
parameter:
        xStart :   X direction Start coordinates
        xEnd   :   X direction end coordinates
********************************************************************************/
void Adafruit_4inch::set_cursor(POINT Xpoint, POINT Ypoint) {
  set_window(Xpoint, Ypoint, Xpoint, Ypoint);
}

/********************************************************************************
function:	Set show color
parameter:
                Color  :   Set show color,16-bit depth
********************************************************************************/
// static void set_color(LENGTH Dis_Width, LENGTH Dis_Height, COLOR Color ){
void Adafruit_4inch::set_color(COLOR Color, POINT Xpoint, POINT Ypoint) {
  write_all_data(Color, (uint32_t)Xpoint * (uint32_t)Ypoint);
}

/********************************************************************************
function:	Point (Xpoint, Ypoint) Fill the color
parameter:
        Xpoint :   The x coordinate of the point
        Ypoint :   The y coordinate of the point
        Color  :   Set the color
********************************************************************************/
void Adafruit_4inch::set_point_i_color(POINT Xpoint, POINT Ypoint,
                                       COLOR Color) {
  if ((Xpoint <= width) && (Ypoint <= height)) {
    set_cursor(Xpoint, Ypoint);
    set_color(Color, 1, 1);
  }
}

/********************************************************************************
function:	Fill the area with the color
parameter:
        Xstart :   Start point x coordinate
        Ystart :   Start point y coordinate
        Xend   :   End point coordinates
        Yend   :   End point coordinates
        Color  :   Set the color
********************************************************************************/
void Adafruit_4inch::set_area_i_color(POINT Xstart, POINT Ystart, POINT Xend,
                                      POINT Yend, COLOR Color) {
  if ((Xend > Xstart) && (Yend > Ystart)) {
    set_window(Xstart, Ystart, Xend, Yend);
    set_color(Color, Xend - Xstart, Yend - Ystart);
  }
}

void Adafruit_4inch::clear(COLOR Color) {
  set_area_i_color(0, 0, width, height, Color);
}

LENGTH Adafruit_4inch::get_width() const { return width; }

LENGTH Adafruit_4inch::get_height() const { return height; }

void Adafruit_4inch::write_gram_scan_way() {
  uint16_t MemoryAccessReg_Data = 0; // addr:0x36
  uint16_t DisFunReg_Data = 0;       // addr:0xB6

  constexpr LENGTH axis_0_size{480};
  constexpr LENGTH axis_1_size{320};

  // Gets the scan direction of GRAM
  switch (scan_direction) {
  case L2R_U2D:
    MemoryAccessReg_Data = 0x08; // 0x08 | 0X8
    DisFunReg_Data = 0x22;
    break;
  case L2R_D2U:
    MemoryAccessReg_Data = 0x08;
    DisFunReg_Data = 0x62;
    break;
  case R2L_U2D: // 0X4
    MemoryAccessReg_Data = 0x08;
    DisFunReg_Data = 0x02;
    break;
  case R2L_D2U: // 0XC
    MemoryAccessReg_Data = 0x08;
    DisFunReg_Data = 0x42;
    break;
  case U2D_L2R: // 0X2
    MemoryAccessReg_Data = 0x28;
    DisFunReg_Data = 0x22;
    break;
  case U2D_R2L: // 0X6
    MemoryAccessReg_Data = 0x28;
    DisFunReg_Data = 0x02;
    break;
  case D2U_L2R: // 0XA
    MemoryAccessReg_Data = 0x28;
    DisFunReg_Data = 0x62;
    break;
  case D2U_R2L: // 0XE
    MemoryAccessReg_Data = 0x28;
    DisFunReg_Data = 0x42;
    break;
  }

  // Get GRAM and LCD width and height
  if (scan_direction == L2R_U2D || scan_direction == L2R_D2U ||
      scan_direction == R2L_U2D || scan_direction == R2L_D2U) {
    width = axis_1_size;
    height = axis_0_size;
  } else {
    width = axis_0_size;
    height = axis_1_size;
  }

  // Set the read / write scan direction of the frame memory
  write_reg(0xB6);
  write_data(0X00);
  write_data(DisFunReg_Data);

  write_reg(0x36);
  write_data(MemoryAccessReg_Data);
}
