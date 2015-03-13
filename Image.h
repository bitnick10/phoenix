#pragma once

#include <FreeImage.h>
#include <cstdint>
#include <iostream>
#include <string>

#define NAMESPACE_IMAGEPP_BEGIN namespace imagepp {
#define NAMESPACE_IMAGEPP_END }

NAMESPACE_IMAGEPP_BEGIN

template<typename T> struct RGB;

struct BW {
    BW() {
    }
    template<typename T>
    BW(const RGB<T>& rgb) {
        throw;
    }
    enum Color : unsigned char {
        Black,
        White
    };
    Color color;
    template<typename T>
    const BW& operator=(const RGB<T>& rhs) {
        throw;
        return *this;
    }
};

template<typename T>
struct RGB {
    T r, g, b;
    RGB() {
    }
    RGB(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    RGB(const BW& bw) {
        this->r = bw.color * 255;
        this->g = bw.color * 255;
        this->b = bw.color * 255;
    }
    bool operator==(const RGB<T>& rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
    }
    const RGB<T>& operator=(const BW& bw) {
        this->r = bw.color * 255;
        this->g = bw.color * 255;
        this->b = bw.color * 255;
        return *this;
    }
};

template<typename T>
struct Rect {
    T x, y, width, height;
};

template<typename T>
class Image {
    unsigned int width_, height_;
    T* data_;
public:
    unsigned int width() const {
        return width_;
    }
    unsigned int height() const {
        return height_;
    }
    T* data() const {
        return data_;
    }
    void set_width(unsigned int width) {
        width_ = width;
    }
    void set_height(unsigned int height) {
        height_ = height;
    }
    void SetPixel(unsigned int x, unsigned int y, T& color) {
        data_[y * width() + x] = color;
    }
    T GetPixel(unsigned int x, unsigned int y) {
        return data_[y * width() + x];
    }
    template<typename T1>
    Image(Image<T1>& src) {
        if(typeid(T) == typeid(BW) && typeid(T1) == typeid(RGB<unsigned char>)) {
            set_width(src.width());
            set_height(src.height());
            data_ = new T[width() * height()];
            for(unsigned int y = 0; y < this->height(); y++) {
                for(unsigned int x = 0; x < this->width(); x++) {
                    RGB<unsigned char> srcColor = src.GetPixel(x, y);
                    BW bwColor;
                    if(srcColor.r + srcColor.g + srcColor.b <= 190 * 3) {
                        bwColor.color = BW::Color::Black;
                    } else {
                        bwColor.color = BW::Color::White;
                    }
                    SetPixel(x, y, bwColor);
                }
            }
        } else {
            throw;
        }
    }
    Image(unsigned int width, unsigned int height) {
        set_width(width);
        set_height(height);
        data_ = new T[width * height];
    }
    Image(const char* filename) {
        std::string fn = filename;
        std::string ext = fn.substr(fn.find_last_of("."));
        FIBITMAP *dib;
        if(ext == ".png") {
            dib = FreeImage_Load(FIF_PNG, filename, PNG_DEFAULT);
        } else {
            throw;
        }
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
                        FreeImage_GetPixelColor(dib, x, y, &color);
                        RGB<unsigned char> c;
                        c.r = color.rgbRed;
                        c.g = color.rgbGreen;
                        c.b = color.rgbBlue;
                        SetPixel(x, y, c);
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
    Image<T> SubImage(unsigned int x1, unsigned int x2) {

    }
    void Save(const char* filename) {
        std::string fn = filename;
        std::string ext = fn.substr(fn.find_last_of("."));
        FIBITMAP *dib;
        if(ext == ".png") {
            dib = FreeImage_Allocate(width(), height(), 24);
            if(!dib) {
                throw;
            }
        } else {
            throw;
        }
        if(typeid(T) == typeid(RGB<unsigned char>)) {
            for(unsigned int y = 0; y < this->height(); y++) {
                for(unsigned int x = 0; x < this->width(); x++) {
                    RGBQUAD color;
                    RGB<unsigned char> colorXY = GetPixel(x, y);
                    color.rgbRed = colorXY.r;
                    color.rgbGreen = colorXY.g;
                    color.rgbBlue = colorXY.b;
                    FreeImage_SetPixelColor(dib, x, y, &color);
                }
            }
        } else if(typeid(T) == typeid(BW)) {
            for(unsigned int y = 0; y < this->height(); y++) {
                for(unsigned int x = 0; x < this->width(); x++) {
                    RGBQUAD color;
                    BW colorXY = GetPixel(x, y);
                    color.rgbRed = colorXY.color * 255;
                    color.rgbGreen = colorXY.color * 255;
                    color.rgbBlue = colorXY.color * 255;
                    FreeImage_SetPixelColor(dib, x, y, &color);
                }
            }
        } else {
            throw;
        }
        BOOL isSuccess = FreeImage_Save(FIF_PNG, dib, filename);
        if(!isSuccess) {
            throw;
        }
        FreeImage_Unload(dib);
    }
    ~Image() {
        if(data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
    }
};

std::vector<unsigned int> VerticalOverlapping(Image<BW>& bwImage) {
    std::vector<unsigned int> overlapped(bwImage.width(), 0);
    for(unsigned int x = 0; x < bwImage.width(); x++) {
        for(unsigned int y = 0; y < bwImage.height(); y++) {
            if(bwImage.GetPixel(x, y).color == BW::Color::Black)
                overlapped[x] += 1;
        }
    }
    return overlapped;
}
namespace cluster {
unsigned int ClusterCount(std::vector<unsigned int> nums) {
    unsigned int count = 0;
    bool in_cluster = true;
    for(unsigned int& n : nums) {
        if(in_cluster) {
            if(n > 0) {
                continue;
            } else {
                in_cluster = false;
            }
        }
        if(!in_cluster) {
            if(n > 0) {
                in_cluster = true;
                count++;
            } else {
                continue;
            }
        }
    }
    return count;
}

std::vector<std::pair<unsigned int, unsigned int>> GetBorders(std::vector<unsigned int> nums) {
    std::vector<std::pair<unsigned int, unsigned int>> borders;
    bool in_cluster = false;
    for(unsigned int i = 0; i < nums.size(); i++) {
        unsigned int n = nums[i];
        if(in_cluster) {
            if(n > 0) {
                continue;
            } else {
                in_cluster = false;
                borders[borders.size() - 1].second = i - 1;
            }
        }
        if(!in_cluster) {
            if(n > 0) {
                std::pair<unsigned int, unsigned int> pair;
                pair.first = i ;
                in_cluster = true;
                borders.push_back(pair);
            } else {
                continue;
            }
        }
    }
    return borders;
}
}
NAMESPACE_IMAGEPP_END