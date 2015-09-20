#pragma once

template<typename T>
class ImageBase {
protected:
    T* data;
    int width, height;
public:
    int Width() {
        return width;
    }
    int Height() {
        return height;
    }
    int InitWidth(int width) {
        this->width = width;
    }
    int InitHeight(int height) {
        this->height = height;
    }
    void SetPixel(int x, int y, T& color) {
        data[y * Width() + x] = color;
    }
    T GetPixel(int x, int y) {
        return data[y * Width() + x];
    }
};