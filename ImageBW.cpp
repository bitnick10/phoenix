#include "ImageBW.h"

namespace phoenix {

#include <iostream>

ImageBW::ImageBW() {
}
void ImageBW::SaveAs(const char* filename) const {
    std::string fn = filename;
    std::string ext = fn.substr(fn.find_last_of("."));
    FIBITMAP *dib;
    if (ext == ".png") {
        dib = FreeImage_Allocate(Width(), Height(), 24);
        if (!dib) {
            throw;
        }
    } else {
        throw;
    }
    for (unsigned int y = 0; y < Height(); y++) {
        for (unsigned int x = 0; x < Width(); x++) {
            RGBQUAD color;
            BW* colorXY = (BW*)(data + y * Width() + x);
            color.rgbRed = *colorXY * 255;
            color.rgbGreen = *colorXY * 255;
            color.rgbBlue = *colorXY * 255;
            FreeImage_SetPixelColor(dib, x, y, &color);
        }
    }
    BOOL isSuccess = FreeImage_Save(FIF_PNG, dib, filename);
    if (!isSuccess) {
        throw;
    }
    FreeImage_Unload(dib);
}
ImageBW::~ImageBW() {
}

}