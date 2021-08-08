#ifndef _GFXCANVAS_EXTRAS_
#define _GFXCANVAS_EXTRAS_

#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"

#ifndef Color565
#define Color565(r, g, b) ((r << 11) | (g << 5) | b)
#define Color16(r, g, b) Color565(r, ((g << 1) | (g & 1)), b)
#endif

/// A GFX 2-bit canvas context for graphics
class GFXcanvas2 : public Adafruit_GFX {
public:
  GFXcanvas2(uint16_t w, uint16_t h);
  ~GFXcanvas2(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void fillScreen(uint16_t color);
  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  void setPalette(uint16_t* colors, uint8_t colorCount);
  void draw16(Adafruit_SPITFT* display, uint16_t x, uint16_t y, uint16_t* colors = NULL);

private:
  uint16_t _w;
  uint16_t _h;
  uint16_t _rowBytes;
  uint8_t *_buffer;
  uint16_t _palette[4];
};

/// A GFX 4-bit canvas context for graphics
class GFXcanvas4 : public Adafruit_GFX {
public:
  GFXcanvas4(uint16_t w, uint16_t h);
  ~GFXcanvas4(void);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  uint16_t getPixel(int16_t x, int16_t y);
  void fillScreen(uint16_t color);
  void setColor(uint8_t red, uint8_t green, uint8_t blue);
  void setColor(uint32_t color);
  bool setIntensity(uint8_t intensity);
  void setPalette(uint16_t* colors, uint8_t colorCount);
  void draw16(Adafruit_SPITFT* display, int16_t x, int16_t y, uint16_t* colors = NULL);

private:
  uint16_t _w;
  uint16_t _h;
  uint16_t _rowBytes;
  uint8_t *_buffer;
  uint8_t _red, _green, _blue;
  uint8_t _intensity = 255;
  uint16_t _palette[16];

  void makePalette();
  int32_t mapAndConstrain(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
};

/// A GFX double size canvas context for graphics
class GFXcanvasDouble : public Adafruit_GFX {
  public:
    GFXcanvasDouble(GFXcanvas4* destCanvas);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void destTextBounds(String str, uint16_t* width, uint16_t* height);
    void setDestCursor(int16_t x, int16_t y);
    uint16_t fontLineHeight() { return gfxFont->yAdvance / 2; };
    uint16_t fontAscender() { return -(gfxFont->glyph['A' - gfxFont->first].yOffset) / 2; };

  private:
    GFXcanvas4* _destCanvas;

};

#endif
