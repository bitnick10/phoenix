#include <FreeImage.h>
#include <iostream>
template<typename T>
struct RGB {
    T r, g, b;
};

template<typename T>
class Image {
    unsigned int width_, height_;
    T* data_;
public:
    unsigned int width() {
        return width_;
    }
    unsigned int height() {
        return height_;
    }
    void set_width(unsigned int width) {
        width_ = width;
    }
    void set_height(unsigned int height) {
        height_ = height;
    }
    void set_pixel(unsigned int x, unsigned int y, T& color) {
        data_[y * height() + x] = color;
    }
    Image(const char* filename) {
        FIBITMAP *dib = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
        if(dib) {
            if(typeid(T) == typeid(RGB<unsigned char>)) {
                //std::cout << "Image<RGB<unsigned char>> construct" << std::endl;
                unsigned int width = FreeImage_GetWidth(dib);
                unsigned int height = FreeImage_GetHeight(dib);
                set_width(width);
                set_height(height);
                data_ = new T[width * height];
                for(unsigned int y = 0; y < this->height(); y++) {
                    for(unsigned int x = 0; x < this->width(); x++) {
                        RGBQUAD color;
                        FreeImage_GetPixelColor(dib, 0, 0, &color);
                        RGB<unsigned char> c;
                        c.r = color.rgbRed;
                        c.g = color.rgbGreen;
                        c.b = color.rgbBlue;
                        set_pixel(x, y, c);
                    }
                }
            } else {
                throw ;
            }
        } else {
            throw ;
        }
        FreeImage_Unload(dib);
    }
    ~Image() {
        if(data_ != nullptr) {
            delete []data_;
        }
    }
};