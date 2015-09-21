#pragma once

namespace phoenix {

template<typename T>
class ImageBase {
protected:
    T* data;
    int width, height;
public:
    int Width() const {
        return width;
    }
    int Height() const {
        return height;
    }
    void InitWidth(int width) {
        this->width = width;
    }
    void InitHeight(int height) {
        this->height = height;
    }
    void AllocData() {
        data = (T*)malloc(width * height * sizeof(T));
    }
    void SetPixel(int x, int y, T& color) {
        data[y * Width() + x] = color;
    }
    T GetPixel(int x, int y) const {
        return data[y * Width() + x];
    }
public:
    ImageBase() {
        throw;
    }
    ImageBase(int width, int height) {
        InitWidth(width);
        InitHeight(height);
        AllocData();
    }
    ~ImageBase() {
        free(data);
    }
};

}