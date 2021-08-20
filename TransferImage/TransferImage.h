#ifndef _TRANSFERIMAGE_
#define _TRANSFERIMAGE_

#include "GFXCanvas_Extras.h"
#include "Adafruit_ImageReader.h"

class Transfer_Image : public Adafruit_Image {
  public:
    void draw4(GFXcanvas4* dest);
};

#endif
