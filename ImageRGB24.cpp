#include "ImageRGB24.h"

#include <iostream>

#include "ImageBW.h"

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
        AllocData();
        for (int y = 0; y < this->Height(); y++) {
            for (int x = 0; x < this->Width(); x++) {
                RGBQUAD color;
                FreeImage_GetPixelColor(dib, x, y, &color);
                RGB24 c(color.rgbRed, color.rgbGreen, color.rgbBlue);
                SetPixel(x, y, c);
            }
        }
    } else {
        throw;
    }
    FreeImage_Unload(dib);
}
void ImageRGB24::SaveAs(const char* filename) const {
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
    for (int y = 0; y < Height(); y++) {
        for (int x = 0; x < Width(); x++) {
            RGBQUAD color;
            RGB24* colorXY = (RGB24*)(data + y * Width() + x);
            color.rgbRed = colorXY->r;
            color.rgbGreen = colorXY->g;
            color.rgbBlue = colorXY->b;
            FreeImage_SetPixelColor(dib, x, y, &color);
        }
    }
    BOOL isSuccess = FreeImage_Save(FIF_PNG, dib, filename);
    if (!isSuccess) {
        throw;
    }
    FreeImage_Unload(dib);
}
ImageBW ImageRGB24::ToImageBW() {
    ImageBW ret(Width(), Height());
    for(int y = 0; y < Height(); y++) {
        for(int x = 0; x < Width(); x++) {
            RGB24 srcColor = GetPixel(x, y);
            BW bwColor;
            if(srcColor.r + srcColor.g + srcColor.b <= 190 * 3) {
                bwColor = BW::Black;
            } else {
                bwColor = BW::White;
            }
            ret.SetPixel(x, y, bwColor);
        }
    }
    return ret;
}
ImageRGB24::~ImageRGB24() {

}
} // namespace phoenix