#pragma once

#include "Color.h"
#include "ImageBase.h"

#include "FreeImage.h"

namespace phoenix {
class ImageRGB24 : ImageBase<RGB24> {
public:
    ImageRGB24(const char* filename);
    ~ImageRGB24();
};
} // namespace phoenix