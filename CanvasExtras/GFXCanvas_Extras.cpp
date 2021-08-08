#include "GFXCanvas_Extras.h"

//---------------------------------------------------------

GFXcanvas2::GFXcanvas2(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
	_w = w;
	_h = h;
	_rowBytes = (w + 3) / 4;
	uint16_t bytes = _rowBytes * _h;
	if (bytes > 0) {
		if ((_buffer = (uint8_t *)malloc(bytes))) {
			memset(_buffer, 0, bytes);
		}
	}
	setColor(0xFF, 0xFF, 0xFF);
}

GFXcanvas2::~GFXcanvas2(void) {
	if (_buffer)
		free(_buffer);
}

void GFXcanvas2::setColor(uint8_t red, uint8_t green, uint8_t blue) {
	for (int i=0; i<4; i++) {
		uint8_t r = (i * red) >> 5;
		uint8_t g = (i * green) >> 4;
		uint8_t b = (i * blue) >> 5;

		_palette[i] = Color565(r, g, b);
	}
}

void GFXcanvas2::setPalette(uint16_t* colors, uint8_t colorCount) {
	if (colorCount <= 4) {
		for (int i=0; i<colorCount; i++) {
			_palette[i] = colors[i];
		}
	}
}

static uint8_t bitmask2[] = { 0x3F, 0xCF, 0xF3, 0xFC };
static uint8_t bitshift2[] { 6, 4, 2, 0 };

void GFXcanvas2::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (_buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
			return;
		}

		uint8_t *ptr = _buffer + x / 4 + y * _rowBytes;
		uint8_t index = x & 0x03;
		*ptr = ((*ptr) & bitmask2[index]) | (color << bitshift2[index]);
	}
}

void GFXcanvas2::fillScreen(uint16_t color) {
	if (_buffer) {
    	static uint8_t colorFill[] = { 0x00, 0x55, 0xAA, 0xFF };
		memset(_buffer, colorFill[color], _rowBytes * _h);
	}
}

void GFXcanvas2::draw16(Adafruit_SPITFT* display, uint16_t x, uint16_t y, uint16_t* colors) {
	int16_t xDest = x, yDest = y, widthDest = _width, heightDest = _height;
	int16_t vMin = 0, vMax = _height, hMin = 0, hMax = _width;

	if (x < 0) {
		xDest = 0;
		widthDest -= -x;
		hMin = -x;
	}
	int16_t xOver = (x + _width) - display->width();
	if (xOver > 0) {
		widthDest -= xOver;
		hMax -= xOver;
	}
	if (y < 0) {
		yDest = 0;
		heightDest -= -y;
		vMin = -y;
	}
	int16_t yOver = (y + _height) - display->height();
	if (yOver > 0) {
		heightDest -= yOver;
		vMax -= yOver;
	}

	uint16_t lineBuffer[widthDest];

	if (colors == NULL) {
		colors = _palette;
	}

	display->startWrite();
	display->setAddrWindow(xDest, yDest, widthDest, heightDest); // Clipped area

	for (uint16_t v=vMin; v<vMax; v++) {
		uint8_t *ptr = _buffer + v * _rowBytes + (hMin / 4);
		uint8_t color;
		uint16_t* dest = lineBuffer;

		for (uint16_t h=hMin; h<hMax; h++) {
			uint8_t bitIndex = h & 0x03;
			if (bitIndex == 3) {
				color = *(ptr++) & 0x03;
			}
			else {
				color = ((*ptr) >> bitshift2[bitIndex]) & 0x03;
			}
			*(dest++) = colors[color];
		}
		display->writePixels(lineBuffer, widthDest); // Push one (clipped) row
	}
	display->endWrite();
}

//---------------------------------------------------------

GFXcanvas4::GFXcanvas4(uint16_t w, uint16_t h) : Adafruit_GFX(w, h) {
	_w = w;
	_h = h;
	_rowBytes = (w + 1) / 2;
	uint16_t bytes = _rowBytes * _h;
	if (bytes > 0) {
		if ((_buffer = (uint8_t *)malloc(bytes))) {
			memset(_buffer, 0, bytes);
		}
	}
	setColor(0x00, 0x00, 0xFF);
}

void GFXcanvas4::setPalette(uint16_t* colors, uint8_t colorCount) {
	if (colorCount <= 16) {
		for (int i=0; i<colorCount; i++) {
			_palette[i] = colors[i];
		}
	}
}

int32_t GFXcanvas4::mapAndConstrain(int32_t value, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
  value = (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

  int32_t minV = min(out_min, out_max);
  int32_t maxV = max(out_min, out_max);

  if (value <= minV) {
    return minV;
  }
  else if (value >= maxV) {
    return maxV;
  }
  else {
    return value;
  }
}

void GFXcanvas4::makePalette() {
	uint8_t maxRed = mapAndConstrain(_red, 0, 255, 0, _intensity >> 3);
	uint8_t maxGreen = mapAndConstrain(_green, 0, 255, 0, _intensity >> 2);
	uint8_t maxBlue = mapAndConstrain(_blue, 0, 255, 0, _intensity >> 3);
	
	for (int i=0; i<16; i++) {
		uint8_t r = mapAndConstrain(i, 0, 15, 0, maxRed);
		uint8_t g = mapAndConstrain(i, 0, 15, 0, maxGreen);
		uint8_t b = mapAndConstrain(i, 0, 15, 0, maxBlue);

		_palette[i] = Color565(r, g, b);
	}
}

void GFXcanvas4::setColor(uint8_t red, uint8_t green, uint8_t blue) {
	if (red!=_red || green!=_green || blue!=_blue) {
		_red = red;
		_green = green;
		_blue = blue;
		makePalette();
	}
}

void GFXcanvas4::setColor(uint32_t color) {
	setColor(color >> 16, (color >> 8) & 0xFF, color & 0xFF);
}

bool GFXcanvas4::setIntensity(uint8_t intensity) {
	bool visibleChange = false;

	if (intensity != _intensity) {
		uint16_t oldMax = _palette[15];
		_intensity = intensity;
		makePalette();
		visibleChange = (oldMax != _palette[15]);
	}
	return visibleChange;
}

GFXcanvas4::~GFXcanvas4(void) {
	if (_buffer)
		free(_buffer);
}

static uint8_t bitmask4[] = { 0x0F, 0xF0 };
static uint8_t bitshift4[] { 4, 0 };

void GFXcanvas4::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (_buffer) {
		if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
			return;
		}

		uint8_t *ptr = _buffer + x / 2 + y * _rowBytes;
		uint8_t index = x & 0x01;
		*ptr = ((*ptr) & bitmask4[index]) | (color << bitshift4[index]);
	}
}

uint16_t GFXcanvas4::getPixel(int16_t x, int16_t y) {
	if ((_buffer == NULL) || (x < 0) || (y < 0) || (x >= _width) || (y >= _height))
    	return 0;

	uint8_t *ptr = _buffer + x / 2 + y * _rowBytes;
	if (x & 1) {
		return *(ptr++) & 0x0F;
	}
	else {
		return (*ptr) >> 4;
	}
}

void GFXcanvas4::fillScreen(uint16_t color) {
	if (_buffer) {
    	color = color | (color << 4);
		memset(_buffer, color, _rowBytes * _h);
	}
}

void GFXcanvas4::draw16(Adafruit_SPITFT* display, int16_t x, int16_t y, uint16_t* colors) {
	int16_t xDest = x, yDest = y, widthDest = _width, heightDest = _height;
	int16_t vMin = 0, vMax = _height, hMin = 0, hMax = _width;

	if (x < 0) {
		xDest = 0;
		widthDest -= -x;
		hMin = -x;
	}
	int16_t xOver = (x + _width) - display->width();
	if (xOver > 0) {
		widthDest -= xOver;
		hMax -= xOver;
	}
	if (y < 0) {
		yDest = 0;
		heightDest -= -y;
		vMin = -y;
	}
	int16_t yOver = (y + _height) - display->height();
	if (yOver > 0) {
		heightDest -= yOver;
		vMax -= yOver;
	}

	// uint16_t lineBuffer[widthDest];

	if (colors == NULL) {
		colors = _palette;
	}

	display->startWrite();
	display->setAddrWindow(xDest, yDest, widthDest, heightDest); // Clipped area

	for (uint16_t v=vMin; v<vMax; v++) {
		uint8_t *ptr = _buffer + v * _rowBytes + (hMin / 2);
		uint8_t color;
		// uint16_t* dest = lineBuffer;

		for (uint16_t h=hMin; h<hMax; h++) {
			if (h & 1) {
				color = *(ptr++) & 0x0F;
			}
			else {
				color = (*ptr) >> 4;
			}
			// *(dest++) = colors[color];
			display->SPI_WRITE16(colors[color]);
		}
		// display->writePixels(lineBuffer, widthDest); // Push one (clipped) row
	}
	display->endWrite();
}

//---------------------------------------------------------

GFXcanvasDouble::GFXcanvasDouble(GFXcanvas4* destCanvas)
			: Adafruit_GFX(destCanvas->width()*2, destCanvas->height()*2) {
	_destCanvas = destCanvas;
}

void GFXcanvasDouble::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height)) {
		return;
	}

	uint16_t destX = x / 2;
	uint16_t destY = y / 2;

	_destCanvas->drawPixel(destX, destY, _destCanvas->getPixel(destX, destY) + color);
}

uint16_t countLines(String str) {
	uint16_t count = 1;
	uint16_t start = 0;
	int16_t index = str.indexOf("\n", start);

	while (index != -1) {
		count++;
		start = index + 1;
		index = str.indexOf("\n", start);
	}

	return count;
}

void GFXcanvasDouble::destTextBounds(String str, uint16_t* width, uint16_t* height) {
    int16_t x1, y1;

   	_destCanvas->getTextBounds(str, 20, 40, &x1, &y1, width, height);
	uint16_t lineCount = countLines(str);
	*height = -(gfxFont->glyph['A' - gfxFont->first].yOffset) + (lineCount-1) * gfxFont->yAdvance;
	*width /= 2;
	*height /= 2;
}

void GFXcanvasDouble::setDestCursor(int16_t x, int16_t y) {
	setCursor(x * 2, y * 2);
}
