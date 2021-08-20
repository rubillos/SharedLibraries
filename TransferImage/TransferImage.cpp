#include "TransferImage.h"

void Transfer_Image::draw4(GFXcanvas4* dest) {
	GFXcanvas16* srcCanvas = canvas.canvas16;
	uint16_t width = srcCanvas->width();
	uint16_t height = srcCanvas->height();

	for (uint16_t y=0; y<height; y++) {
		for (uint16_t x=0; x<width; x++) {
			uint16_t color = srcCanvas->getPixel(x, y);

			color = (color >> 1) & 0x0F;
			dest->drawPixel(x, y, color);
		}
	}
}
