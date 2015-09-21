#pragma once

#include "Color.h"
#include "ImageBase.h"
#include "FreeImage.h"

namespace phoenix {

class ImageBW : public ImageBase<BW> {
public:
    ImageBW();
    void SaveAs(const char* filename) const;
    ImageBW(int width, int height) : ImageBase(width, height) {};
    ~ImageBW();
};

}