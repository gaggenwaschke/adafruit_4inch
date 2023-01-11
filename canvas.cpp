#include "canvas.hpp"
#include "Arduino.h"
#include "fonts.hpp"

/******************************************************************************
  function:	Draw Point(Xpoint, Ypoint) Fill the color
  parameter:
        Xpoint		:   The x coordinate of the point
        Ypoint		:   The y coordinate of the point
        Color		:   Set color
        Dot_Pixel	:	point size
******************************************************************************/
void Canvas::draw_point(POINT Xpoint, POINT Ypoint, COLOR Color,
                        DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE) {
  int16_t XDir_Num, YDir_Num;
  if (DOT_STYLE == DOT_STYLE_DFT) {
    for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
      for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
        set_point_i_color(Xpoint + XDir_Num - Dot_Pixel,
                          Ypoint + YDir_Num - Dot_Pixel, Color);
      }
    }
  } else {
    for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++) {
      for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++) {
        set_point_i_color(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1, Color);
      }
    }
  }
}

/******************************************************************************
  function:	Draw a line of arbitrary slope
  parameter:
        Xstart ：Starting x point coordinates
        Ystart ：Starting x point coordinates
        Xend   ：End point x coordinate
        Yend   ：End point y coordinate
        Color  ：The color of the line segment
******************************************************************************/
void Canvas::draw_line(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                       COLOR Color, LINE_STYLE Line_Style,
                       DOT_PIXEL Dot_Pixel) {
  POINT Xpoint = Xstart;
  POINT Ypoint = Ystart;
  int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
  int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

  // Increment direction, 1 is positive, -1 is counter;
  int XAddway = Xstart < Xend ? 1 : -1;
  int YAddway = Ystart < Yend ? 1 : -1;

  // Cumulative error
  int Esp = dx + dy;
  char Line_Style_Temp = 0;

  for (;;) {
    Line_Style_Temp++;
    // Painted dotted line, 2 point is really virtual
    if (Line_Style == LINE_DOTTED && Line_Style_Temp % 3 == 0) {
      //       draw_point(Xpoint, Ypoint, LCD_BACKGROUND, Dot_Pixel,
      //       DOT_STYLE_DFT);
      Line_Style_Temp = 0;
    } else {
      draw_point(Xpoint, Ypoint, Color, Dot_Pixel, DOT_STYLE_DFT);
    }
    if (2 * Esp >= dy) {
      if (Xpoint == Xend)
        break;
      Esp += dy;
      Xpoint += XAddway;
    }
    if (2 * Esp <= dx) {
      if (Ypoint == Yend)
        break;
      Esp += dx;
      Ypoint += YAddway;
    }
  }
}

/******************************************************************************
  function:	Draw a rectangle
  parameter:
        Xstart ：Rectangular  Starting x point coordinates
        Ystart ：Rectangular  Starting x point coordinates
        Xend   ：Rectangular  End point x coordinate
        Yend   ：Rectangular  End point y coordinate
        Color  ：The color of the Rectangular segment
        Filled : Whether it is filled--- 1 solid 0：empty
******************************************************************************/
void Canvas::draw_rectangle(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                            COLOR Color, DRAW_FILL Filled,
                            DOT_PIXEL Dot_Pixel) {
  if (Filled) {
#if LOW_Speed_Show
    POINT Ypoint;
    for (Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
      draw_line(Xstart, Ypoint, Xend, Ypoint, Color, LINE_SOLID, Dot_Pixel);
    }
#elif HIGH_Speed_Show
    set_area_i_color(Xstart, Ystart, Xend, Yend, Color);
#endif
  } else {
    draw_line(Xstart, Ystart, Xend, Ystart, Color, LINE_SOLID, Dot_Pixel);
    draw_line(Xstart, Ystart, Xstart, Yend, Color, LINE_SOLID, Dot_Pixel);
    draw_line(Xend, Yend, Xend, Ystart, Color, LINE_SOLID, Dot_Pixel);
    draw_line(Xend, Yend, Xstart, Yend, Color, LINE_SOLID, Dot_Pixel);
  }
}

/******************************************************************************
  function:	Use the 8-point method to draw a circle of the
                                specified size at the specified position.
  parameter:
        X_Center  ：Center X coordinate
        Y_Center  ：Center Y coordinate
        Radius    ：circle Radius
        Color     ：The color of the ：circle segment
        Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void Canvas::draw_circle(POINT X_Center, POINT Y_Center, LENGTH Radius,
                         COLOR Color, DRAW_FILL Draw_Fill,
                         DOT_PIXEL Dot_Pixel) {
  // Draw a circle from(0, R) as a starting point
  int16_t XCurrent, YCurrent;
  XCurrent = 0;
  YCurrent = Radius;

  // Cumulative error,judge the next point of the logo
  int16_t Esp = 3 - (Radius << 1);

  int16_t sCountY;
  if (Draw_Fill == DRAW_FULL) {
    while (XCurrent <= YCurrent) { // Realistic circles
      for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++) {
        draw_point(X_Center + XCurrent, Y_Center + sCountY, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 1
        draw_point(X_Center - XCurrent, Y_Center + sCountY, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 2
        draw_point(X_Center - sCountY, Y_Center + XCurrent, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 3
        draw_point(X_Center - sCountY, Y_Center - XCurrent, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 4
        draw_point(X_Center - XCurrent, Y_Center - sCountY, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 5
        draw_point(X_Center + XCurrent, Y_Center - sCountY, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 6
        draw_point(X_Center + sCountY, Y_Center - XCurrent, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT); // 7
        draw_point(X_Center + sCountY, Y_Center + XCurrent, Color,
                   DOT_PIXEL_DFT, DOT_STYLE_DFT);
      }
      if (Esp < 0)
        Esp += 4 * XCurrent + 6;
      else {
        Esp += 10 + 4 * (XCurrent - YCurrent);
        YCurrent--;
      }
      XCurrent++;
    }
  } else { // Draw a hollow circle
    while (XCurrent <= YCurrent) {
      draw_point(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 1
      draw_point(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 2
      draw_point(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 3
      draw_point(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 4
      draw_point(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 5
      draw_point(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 6
      draw_point(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 7
      draw_point(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel,
                 DOT_STYLE_DFT); // 0

      if (Esp < 0)
        Esp += 4 * XCurrent + 6;
      else {
        Esp += 10 + 4 * (XCurrent - YCurrent);
        YCurrent--;
      }
      XCurrent++;
    }
  }
}

/******************************************************************************
  function:	Show English characters
  parameter:
        Xpoint           ：X coordinate
        Ypoint           ：Y coordinate
        Acsii_Char       ：To display the English characters
        Font             ：A structure pointer that displays a character size
        Color_Background : Select the background color of the English character
        Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void Canvas::draw_char(POINT Xpoint, POINT Ypoint, const char Acsii_Char,
                       const Font &font, COLOR Color_Background,
                       COLOR Color_Foreground) {
  POINT Page, Column;

  uint32_t Char_Offset = (Acsii_Char - ' ') * font.Height *
                         (font.Width / 8 + (font.Width % 8 ? 1 : 0));
  const unsigned char *ptr = &font.table[Char_Offset];

  for (Page = 0; Page < font.Height; Page++) {
    for (Column = 0; Column < font.Width; Column++) {

      // To determine whether the font background color and screen background
      // color is consistent
      if (FONT_BACKGROUND ==
          Color_Background) { // this process is to speed up the scan
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8)))
          draw_point(Xpoint + Column, Ypoint + Page, Color_Foreground,
                     DOT_PIXEL_DFT, DOT_STYLE_DFT);
      } else {
        if (pgm_read_byte(ptr) & (0x80 >> (Column % 8))) {
          draw_point(Xpoint + Column, Ypoint + Page, Color_Foreground,
                     DOT_PIXEL_DFT, DOT_STYLE_DFT);
        } else {
          draw_point(Xpoint + Column, Ypoint + Page, Color_Background,
                     DOT_PIXEL_DFT, DOT_STYLE_DFT);
        }
      }
      // One pixel is 8 bits
      if (Column % 8 == 7)
        ptr++;
    } /* Write a line */
    if (font.Width % 8 != 0)
      ptr++;
  } /* Write all */
}

/******************************************************************************
  function:	Display the string
  parameter:
        Xstart           ：X coordinate
        Ystart           ：Y coordinate
        pString          ：The first address of the English string to be
displayed Font             ：A structure pointer that displays a character size
        Color_Background : Select the background color of the English character
        Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void Canvas::draw_string(POINT Xstart, POINT Ystart, const char *pString,
                         const Font &font, COLOR Color_Background,
                         COLOR Color_Foreground) {
  POINT Xpoint = Xstart;
  POINT Ypoint = Ystart;

  while (*pString != '\0') {
    // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y
    // direction plus the height of the character
    if ((Xpoint + font.Width) > get_width()) {
      Xpoint = Xstart;
      Ypoint += font.Height;
    }

    // If the Y direction is full, reposition to(Xstart, Ystart)
    if ((Ypoint + font.Height) > get_height()) {
      Xpoint = Xstart;
      Ypoint = Ystart;
    }
    draw_char(Xpoint, Ypoint, *pString, font, Color_Background,
              Color_Foreground);

    // The next character of the address
    pString++;

    // The next word of the abscissa increases the font of the broadband
    Xpoint += font.Width;
  }
}

/******************************************************************************
  function:	Display the string
  parameter:
        Xstart           ：X coordinate
        Ystart           : Y coordinate
        Number          : The number displayed
        Font             ：A structure pointer that displays a character size
        Color_Background : Select the background color of the English character
        Color_Foreground : Select the foreground color of the English character
******************************************************************************/
#define ARRAY_LEN 255
void Canvas::draw_number(POINT Xpoint, POINT Ypoint, int32_t Number,
                         const Font &font, COLOR Color_Background,
                         COLOR Color_Foreground) {

  int16_t Num_Bit = 0, Str_Bit = 0;
  uint8_t Str_Array[ARRAY_LEN] = {0}, Num_Array[ARRAY_LEN] = {0};
  uint8_t *pStr = Str_Array;

  if (Xpoint > get_width() || Ypoint > get_height()) {
    return;
  }

  // Converts a number to a string
  while (Number) {
    Num_Array[Num_Bit] = Number % 10 + '0';
    Num_Bit++;
    Number /= 10;
  }

  // The string is inverted
  while (Num_Bit > 0) {
    Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
    Str_Bit++;
    Num_Bit--;
  }

  // show
  draw_string(Xpoint, Ypoint, (const char *)pStr, font, Color_Background,
              Color_Foreground);
}

/******************************************************************************
  function:	Display the bit map,1 byte = 8bit = 8 points
  parameter:
        Xpoint ：X coordinate
        Ypoint : Y coordinate
        pMap   : Pointing to the picture
        Width  ：Bitmap Width
        Height : Bitmap Height
  note:
        This function is suitable for bitmap, because a 16-bit data accounted
for 16 points
******************************************************************************/
void Canvas::disbitmap(POINT Xpoint, POINT Ypoint, const unsigned char *pMap,
                       POINT Width, POINT Height) {
  POINT i, j, byteWidth = (Width + 7) / 8;
  for (j = 0; j < Height; j++) {
    for (i = 0; i < Width; i++) {
      if (*(pMap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        draw_point(Xpoint + i, Ypoint + j, WHITE, DOT_PIXEL_DFT, DOT_STYLE_DFT);
      }
    }
  }
}

/******************************************************************************
  function:	Display the Gray map,1 byte = 8bit = 2 points
  parameter:
        Xpoint ：X coordinate
        Ypoint : Y coordinate
        pMap   : Pointing to the picture
        Width  ：Bitmap Width
        Height : Bitmap Height
  note:
        This function is suitable for bitmap, because a 4-bit data accounted for
1 points Please use the Image2lcd generated array
******************************************************************************/
void Canvas::disgraymap(POINT Xpoint, POINT Ypoint, const unsigned char *pBmp) {
  // Get the Map header Gray, width, height
  char Gray;
  Gray = *(pBmp + 1);
  POINT Height, Width;
  Width = (*(pBmp + 3) << 8) | (*(pBmp + 2));
  Height = (*(pBmp + 5) << 8) | (*(pBmp + 4));

  POINT i, j;
  if (Gray == 0x04) { // Sixteen gray levels
    pBmp = pBmp + 6;
    for (j = 0; j < Height; j++)
      for (i = 0; i < Width / 2; i++) {
        draw_point(Xpoint + i * 2, Ypoint + j, ~(*pBmp >> 4), DOT_PIXEL_DFT,
                   DOT_STYLE_DFT);
        draw_point(Xpoint + i * 2 + 1, Ypoint + j, ~*pBmp, DOT_PIXEL_DFT,
                   DOT_STYLE_DFT);
        pBmp++;
      }
  } else {
    return;
  }
}

void Canvas::show_demo() {
  clear(WHITE);
  draw_line(0, 10, get_width(), 10, RED, LINE_SOLID, DOT_PIXEL_2X2);
  draw_line(0, 20, get_width(), 20, RED, LINE_DOTTED, DOT_PIXEL_DFT);
  draw_line(0, 300, get_width(), 300, RED, LINE_DOTTED, DOT_PIXEL_DFT);
  draw_line(0, 310, get_width(), 310, RED, LINE_SOLID, DOT_PIXEL_2X2);

  draw_rectangle(10, 30, get_width() - 10, get_height() - 30, BLUE, DRAW_EMPTY,
                 DOT_PIXEL_DFT);
  draw_rectangle(20, 40, get_width() - 20, 60, BLUE, DRAW_FULL, DOT_PIXEL_DFT);

  uint16_t Cx1 = 190, Cy1 = 240, Cr = 20;
  uint16_t Cx2 = Cx1 + (2.5 * Cr), Cy2 = Cy1;
  uint16_t Cx3 = Cx1 + (5 * Cr), Cy3 = Cy1;
  uint16_t Cx4 = (Cx1 + Cx2) / 2, Cy4 = Cy1 + Cr;
  uint16_t Cx5 = (Cx2 + Cx3) / 2, Cy5 = Cy1 + Cr;

  draw_circle(Cx1, Cy1, Cr, BLUE, DRAW_EMPTY, DOT_PIXEL_2X2);
  draw_circle(Cx2, Cy2, Cr, BLACK, DRAW_EMPTY, DOT_PIXEL_2X2);
  draw_circle(Cx3, Cy3, Cr, RED, DRAW_EMPTY, DOT_PIXEL_2X2);
  draw_circle(Cx4, Cy4, Cr, YELLOW, DRAW_EMPTY, DOT_PIXEL_2X2);
  draw_circle(Cx5, Cy5, Cr, GREEN, DRAW_EMPTY, DOT_PIXEL_2X2);

  draw_circle(50, 250, 30, CYAN, DRAW_FULL, DOT_PIXEL_DFT);
  draw_circle(get_width() - 50, 250, 30, CYAN, DRAW_FULL, DOT_PIXEL_DFT);

  draw_string(80, 80, "WaveShare Electronic", Font24, LCD_BACKGROUND, BLUE);
  draw_string(80, 120, "3.5inch TFTLCD", Font20, RED, BLUE);

  draw_number(80, 150, 1234567890, Font16, LCD_BACKGROUND, BLUE);
}
