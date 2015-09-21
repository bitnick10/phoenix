#pragma once

#include "Color.h"
#include "ImageBase.h"
#include "ImageBW.h"
#include "FreeImage.h"

namespace phoenix {

class ImageRGB24 : public ImageBase<RGB24> {
public:
    ImageRGB24() {
        throw;
    }
    ImageRGB24(const ImageRGB24& rhs) {
        throw;
    }
    ImageRGB24(const char* filename);
    void SaveAs(const char* filename) const;
    ImageBW ToImageBW();
    ~ImageRGB24();
};
} // namespace phoenix