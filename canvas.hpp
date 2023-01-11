#pragma once

#include "font_type.hpp"

using LENGTH = uint16_t; // The type of coordinate (unsigned short)

#define LOW_Speed_Show 0
#define HIGH_Speed_Show 1

enum ScanDirection {
  L2R_U2D = 0, // The display interface is displayed , left to right, up to down
  L2R_D2U,
  R2L_U2D,
  R2L_D2U,

  U2D_L2R,
  U2D_R2L,
  D2U_L2R,
  D2U_R2L,
};

using COLOR = uint16_t; // The variable type of the color (unsigned short)
using POINT = uint16_t; // The type of coordinate (unsigned short)

/********************************************************************************
function:
                        dot pixel
********************************************************************************/
typedef enum {
  DOT_PIXEL_1X1 = 1, // dot pixel 1 x 1
  DOT_PIXEL_2X2,     // dot pixel 2 X 2
  DOT_PIXEL_3X3,     // dot pixel 3 X 3
  DOT_PIXEL_4X4,     // dot pixel 4 X 4
  DOT_PIXEL_5X5,     // dot pixel 5 X 5
  DOT_PIXEL_6X6,     // dot pixel 6 X 6
  DOT_PIXEL_7X7,     // dot pixel 7 X 7
  DOT_PIXEL_8X8,     // dot pixel 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT DOT_PIXEL_1X1 // Default dot pilex

/********************************************************************************
function:
                        dot Fill style
********************************************************************************/
typedef enum {
  DOT_FILL_AROUND = 1, // dot pixel 1 x 1
  DOT_FILL_RIGHTUP,    // dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT DOT_FILL_AROUND // Default dot pilex
/********************************************************************************
function:
                        solid line and dotted line
********************************************************************************/
typedef enum {
  LINE_SOLID = 0,
  LINE_DOTTED,
} LINE_STYLE;

/********************************************************************************
function:
                        DRAW Internal fill
********************************************************************************/
typedef enum {
  DRAW_EMPTY = 0,
  DRAW_FULL,
} DRAW_FILL;

/********************************************************************************
function:
                        Defines commonly used colors for the display
********************************************************************************/
#define LCD_BACKGROUND WHITE  // Default background color
#define FONT_BACKGROUND WHITE // Default font background color
#define FONT_FOREGROUND GRED  // Default font foreground color

#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define BROWN 0XBC40
#define BRRED 0XFC07
#define GRAY 0X8430

class Canvas {
public:
  // Interface to implement for port.
  virtual void init() = 0;
  virtual void clear(COLOR Color) = 0;
  virtual LENGTH get_width() const = 0;
  virtual LENGTH get_height() const = 0;

  // Drawing
  void draw_point(POINT Xpoint, POINT Ypoint, COLOR Color, DOT_PIXEL Dot_Pixel,
                  DOT_STYLE Dot_FillWay = DOT_FILL_AROUND);
  void draw_line(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                 COLOR Color, LINE_STYLE Line_Style = LINE_SOLID,
                 DOT_PIXEL Dot_Pixel = DOT_PIXEL_1X1);
  void draw_rectangle(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                      COLOR Color, DRAW_FILL Filled,
                      DOT_PIXEL Dot_Pixel = DOT_PIXEL_1X1);
  void draw_circle(POINT X_Center, POINT Y_Center, LENGTH Radius, COLOR Color,
                   DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel = DOT_PIXEL_1X1);

  // pic
  void disbitmap(POINT Xpoint, POINT Ypoint, const unsigned char *pMap,
                 POINT Width, POINT Height);
  void disgraymap(POINT Xpoint, POINT Ypoint, const unsigned char *pBmp);

  // Display string
  void draw_char(POINT Xstart, POINT Ystart, const char Acsii_Char,
                 const Font &font, COLOR Color_Background,
                 COLOR Color_Foreground);
  void draw_string(POINT Xstart, POINT Ystart, const char *pString,
                   const Font &font, COLOR Color_Background,
                   COLOR Color_Foreground);
  void draw_number(POINT Xpoint, POINT Ypoint, int32_t Number, const Font &font,
                   COLOR Color_Background, COLOR Color_Foreground);
  // show
  void show_demo();

protected:
  virtual void set_point_i_color(POINT Xpoint, POINT Ypoint, COLOR Color) = 0;
  virtual void set_area_i_color(POINT Xstart, POINT Ystart, POINT Xend,
                                POINT Yend, COLOR Color) = 0;
};
