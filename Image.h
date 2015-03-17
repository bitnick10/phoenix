#pragma once

#include <FreeImage.h>
#include <cstdint>
#include <iostream>
#include <string>

#define NAMESPACE_IMAGEPP_BEGIN namespace imagepp {
#define NAMESPACE_IMAGEPP_END }
#define NAMESPACE_CLUSTER_BEGIN namespace cluster {
#define NAMESPACE_CLUSTER_END }
#define NAMESPACE_FEATURE_EXTRACTION_BEGIN namespace feature_extraction {
#define NAMESPACE_FEATURE_EXTRACTION_END }

NAMESPACE_IMAGEPP_BEGIN

struct BW {
    enum Color : unsigned char {
        Black,
        White
    };
    BW() {
    }
    BW(const BW& bw) {
        this->color = bw.color;
    }
    BW(const Color color) {
        this->color = color;
    }
    Color color;
    bool operator ==(const BW& rhs) const {
        return this->color == rhs.color;
    }
};

template<typename T> struct Gray {
    Gray() {}
    Gray(const Gray& gray) {
        this->value = gray.value;
    }
    bool operator==(unsigned char rhs) {
        return value == rhs;
    }
    T value;
};

template<typename T> struct RGB {
    T r, g, b;
    RGB() {
    }
    RGB(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
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

template<typename T> struct Rect {
    T x, y, width, height;
};

template<typename T> class Image {
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
    T GetPixel(unsigned int x, unsigned int y) const {
        return data_[y * width() + x];
    }
    Image(const Image<T>& src) {
        set_width(src.width());
        set_height(src.height());
        data_ = new T[width() * height()];
        memcpy(data_, src.data(), sizeof(T)*width()*height());
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
    Image<T> SubImage(const Rect<unsigned int>& rect) const {
        assert(rect.width > 0 && rect.height > 0);
        assert(rect.x < width() && rect.y < height());
        assert(rect.x + rect.width <= width() && rect.y + rect.height <= height());
        Image<T> subImage(rect.width, rect.height);
        for(unsigned int y = 0; y < subImage.height(); y++) {
            for(unsigned int x = 0; x < subImage.width(); x++) {
                subImage.SetPixel(x, y, GetPixel(rect.x + x, rect.y + y));
            }
        }
        return subImage;
    }
    void Save(const char* filename) const {
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
                    RGB<unsigned char>* colorXY = (RGB<unsigned char>*)(data_ + y * width() + x);
                    color.rgbRed = colorXY->r;
                    color.rgbGreen = colorXY->g;
                    color.rgbBlue = colorXY->b;
                    FreeImage_SetPixelColor(dib, x, y, &color);
                }
            }
        } else if(typeid(T) == typeid(BW)) {
            for(unsigned int y = 0; y < this->height(); y++) {
                for(unsigned int x = 0; x < this->width(); x++) {
                    RGBQUAD color;
                    BW* colorXY = (BW*)(data_ + y * width() + x);
                    color.rgbRed = colorXY->color * 255;
                    color.rgbGreen = colorXY->color * 255;
                    color.rgbBlue = colorXY->color * 255;
                    FreeImage_SetPixelColor(dib, x, y, &color);
                }
            }
        } else if(typeid(T) == typeid(Gray<unsigned char>)) {
            for(unsigned int y = 0; y < this->height(); y++) {
                for(unsigned int x = 0; x < this->width(); x++) {
                    RGBQUAD color;
                    Gray<unsigned int> *colorXY = (Gray<unsigned int>*)(data_ + y * width() + x);
                    color.rgbRed = colorXY->value;
                    color.rgbGreen = colorXY->value;
                    color.rgbBlue = colorXY->value;
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

template<typename T> class Matrix {
    unsigned int width_, height_;
    T* data_;
public:
    class Element {
    public:
        class ElementProxy {
        public:
            ElementProxy(const Matrix<T>* src, unsigned int m, unsigned int n) {
                src_ = src;
                m_ = m;
                n_ = n;
            }
            ElementProxy& operator=(ElementProxy& right) {
                src_->data()[(m_ - 1)*src_->width() + x] = right.data()[(m_ - 1) * src_.width() + x];
                return *this;
            }
            ElementProxy& operator=(T value) {
                src_->data()[(m_ - 1)*src_->width() + n_ - 1] = value;
                return *this;
            }
            T& operator++(int) {
                src_->data()[(m_ - 1)*src_->width() + n_ - 1] ++;
                return src_->data()[(m_ - 1) * src_->width() + n_ - 1];
            }
            /* T operator[](int y)  {
                 throw;
                 std::cout << "y" << std::endl;
                 return T(0);
             }*/
            operator T() const {
                return  src_->data()[(m_ - 1) * src_->width() + n_ - 1];
            }
        private:
            const Matrix<T>* src_;
            unsigned int m_;
            unsigned int n_;
        };
    public:
        Element(const Matrix<T>* src, int m) {
            src_ = src;
            m_ = m;
        }
        ElementProxy operator[](int n) {
            assert(n >= 1);
            return ElementProxy(src_, m_, n);
        }
        const ElementProxy operator[](int n) const {
            assert(n >= 1);
            return ElementProxy(src_, m_, n);
        }
    private:
        const Matrix<T>* src_;
        int m_;
    };
    Element operator[](int m)  {
        assert(m >= 1);// matrix index start with 1
        return Element(this, m);
    }
    const Element operator[](int m) const {
        assert(m >= 1);// matrix index start with 1
        return Element(this, m);
    }
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
    Matrix(unsigned int width, unsigned int height) {
        set_width(width);
        set_height(height);
        data_ = new T[width * height];
    }
    Matrix(unsigned int width, unsigned int height, T v) {
        set_width(width);
        set_height(height);
        data_ = new T[width * height];
        for(unsigned int i = 0; i < width * height; i++) {
            data_[i] = v;
        }
    }
    T GetMaxValue() const {
        T max = 0;
        for(unsigned int i = 0; i < width()*height(); i++) {
            if(data()[i] > max) {
                max = data()[i];
            }
        }
        return max;
    }
    ~Matrix() {
        if(data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
    }
};

template<typename T> class Histogram2D {
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
public:
    Histogram2D(const Histogram2D<T>& src) {
        set_width(src.width());
        set_height(src.height());
        data_ = new T[width() * height()];
        memcpy(data_, src.data(), sizeof(T)*width()*height());
    }
    Histogram2D(unsigned int width, unsigned int height) {
        set_width(width);
        set_height(height);
        data_ = new T[width * height];
    }
    Histogram2D(unsigned int width, unsigned int height, T v) {
        set_width(width);
        set_height(height);
        data_ = new T[width * height];
        for(unsigned int i = 0; i < width * height; i++) {
            data_[i] = v;
        }
    }
    T GetMaxValue() const {
        T max = 0;
        for(unsigned int i = 0; i < width()*height(); i++) {
            if(data()[i] > max) {
                max = data()[i];
            }
        }
        return max;
    }
    ~Histogram2D() {
        if(data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
        }
    }
    void AddAt(unsigned int x, unsigned int y, T value) {
        data_[y * width() + x] += value;
    }
    T GetValue(unsigned int x, unsigned int y) const {
        return data_[y * width() + x];
    }
    friend std::ostream& operator<<(std::ostream& os, Histogram2D& histogram) {
        for(unsigned int y = 0; y < histogram.height(); y++) {
            for(unsigned int x = 0; x < histogram.width(); x++) {
                os << histogram.GetValue(x, y) << " ";
            }
            os << std::endl;
        }
        return os;
    }
};

enum class RGB2BWAlgorithm {
    CustomA
};
enum class Histogram2D2GrayImageAlgorithm {
    Normal
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
std::vector<unsigned int> HorizontalOverlapping(Image<BW>& bwImage) {
    std::vector<unsigned int> overlapped(bwImage.height(), 0);
    for(unsigned int y = 0; y < bwImage.height(); y++) {
        for(unsigned int x = 0; x < bwImage.width(); x++) {
            if(bwImage.GetPixel(x, y).color == BW::Color::Black)
                overlapped[y] += 1;
        }
    }
    return overlapped;
}
template<typename T> T GetConvertedImage(const Image<RGB<unsigned char>>& src, RGB2BWAlgorithm algorithm) {
    if(typeid(T) == typeid(Image<BW>) && algorithm == RGB2BWAlgorithm::CustomA) {
        Image<BW> ret(src.width(), src.height());
        for(unsigned int y = 0; y < ret.height(); y++) {
            for(unsigned int x = 0; x < ret.width(); x++) {
                RGB<unsigned char> srcColor = src.GetPixel(x, y);
                BW bwColor;
                if(srcColor.r + srcColor.g + srcColor.b <= 190 * 3) {
                    bwColor.color = BW::Color::Black;
                } else {
                    bwColor.color = BW::Color::White;
                }
                ret.SetPixel(x, y, bwColor);
            }
        }
        return ret;
    } else {
        throw;
    }
}
template<typename T> T GetConvertedImage(const Histogram2D<unsigned int>& histogram, Histogram2D2GrayImageAlgorithm algorithm) {
    if(typeid(T) == typeid(Image<Gray<unsigned char>>) && algorithm == Histogram2D2GrayImageAlgorithm::Normal) {
        Image<Gray<unsigned char>> image(histogram.width(), histogram.height());
        unsigned int max = histogram.GetMaxValue();
        for(unsigned int y = 0; y < image.height() ; y++  ) {
            for(unsigned int x = 0 ; x < image.width(); x++ ) {
                Gray<unsigned char> color;
                double value = (double)histogram.GetValue(x, y) / max * 255;
                color.value = (int)value;
                image.SetPixel(x, y, color);
            }
        }
        return image;
    } else {
        throw;
    }
}

NAMESPACE_CLUSTER_BEGIN

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

NAMESPACE_CLUSTER_END

NAMESPACE_FEATURE_EXTRACTION_BEGIN

Histogram2D<unsigned int> Line(const Image<BW>& image) {
    Histogram2D<unsigned int> histogram(360 / 45, 9, 0);
    for(unsigned int y = 0; y < image.height(); y++) {
        for(unsigned int x = 0; x < image.width(); x++) {
            if(image.GetPixel(x, y).color == BW::Color::Black) {
                for(unsigned int theta = 0; theta < 360; theta += 45) {
                    for(unsigned int r = 0; r < 9; r++) {
                        double costheta = cos(theta * 3.1415926 / 180);
                        double sintheta = sin(theta * 3.1415926 / 180);
                        if(r == x * costheta + y * sintheta) {
                            histogram.AddAt(theta, r, 1);
                            std::cout << "(" << theta << "," << r << ")";
                        }
                    }
                }
            }
        }
    }
    std::cout << std::endl << histogram;
    return histogram;
}

NAMESPACE_FEATURE_EXTRACTION_END
NAMESPACE_IMAGEPP_END