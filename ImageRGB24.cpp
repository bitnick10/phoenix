#include "ImageRGB24.h"

#include <iostream>

namespace phoenix {

ImageRGB24::ImageRGB24(const char* filename) {
    std::string fn = filename;
    std::string ext = fn.substr(fn.find_last_of("."));
    FIBITMAP *dib;
    if (ext == ".png") {
        dib = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
    } else {
        throw;
    }
    if (dib) {
        //std::cout << "Image<RGB<unsigned char>> construct" << std::endl;
        unsigned int width = FreeImage_GetWidth(dib);
        unsigned int height = FreeImage_GetHeight(dib);
        InitWidth(width);
        InitHeight(height);
        this->data = (RGB24*)malloc(Width() * Height());
        for (unsigned int y = 0; y < this->Height(); y++) {
            for (unsigned int x = 0; x < this->Width(); x++) {
                RGBQUAD color;
                FreeImage_GetPixelColor(dib, x, y, &color);
                RGB24 c(color.rgbRed, color.rgbGreen, color.rgbBlue);
                SetPixel(x, y, c);
            }
        }
        else {
            throw;
        }
    } else {
        throw;
    }
    FreeImage_Unload(dib);
}


ImageRGB24::~ImageRGB24() {
}
}