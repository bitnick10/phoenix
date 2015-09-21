#pragma once

#include <FreeImage.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <numeric>
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include "phoenix.h"

#define NAMESPACE_PHOENIX_BEGIN namespace phoenix {
#define NAMESPACE_PHOENIX_END }
#define NAMESPACE_CLUSTER_BEGIN namespace cluster {
#define NAMESPACE_CLUSTER_END }
#define NAMESPACE_FEATURE_EXTRACTION_BEGIN namespace feature_extraction {
#define NAMESPACE_FEATURE_EXTRACTION_END }
#define NAMESPACE_FEATURE_DETECTION_BEGIN namespace feature_detection {
#define NAMESPACE_FEATURE_DETECTION_END }

NAMESPACE_PHOENIX_BEGIN

template<typename T> bool IsEqual(const T& lhs, const T& rhs) {
    if(typeid(T) == typeid(float)) {
        return  std::fabs(float(lhs - rhs)) < std::numeric_limits<T>::epsilon();
    } else if(typeid(T) == typeid(double)) {
        return std::fabs(double(lhs - rhs)) < std::numeric_limits<T>::epsilon();
    } else {
        return lhs == rhs;
    }
}

//bool operator==(const Point<unsigned int>& lhs, const Point<unsigned int>& rhs) {
//    return true;
//}
template<typename T> class Counter {
    template<typename T, typename TC> struct countpair {
        countpair(T data, TC count) {
            this->data = data;
            this->count = count;
        }
        T data;
        TC count;
    };
    std::vector<countpair<T, unsigned int>> pairs;
public:
    bool is_empty() const {
        return pairs.empty();
    }
    void Count(const T& t, int i) {
        for(auto& p : pairs) {
            if(t == p.data) {
                p.count += i;
                return ;
            }
        }
        countpair<T, unsigned int> newPair(t, i);
        pairs.push_back(newPair);
    }
    unsigned int GetMaxCount() {
        assert(pairs.size() > 0);
        auto max = std::max_element(pairs.begin(), pairs.end(), [](const countpair<T, unsigned int>& lhs, const countpair<T, unsigned int>& rhs) -> bool{
            return lhs.count < rhs.count;
        });
        return (*max).count;
    }
    std::vector<countpair<T, unsigned int>> GetSortedData() {
        std::sort(pairs.begin(), pairs.end(), [](const countpair<T, unsigned int>& lhs, const countpair<T, unsigned int>& rhs) -> bool {
            return lhs.count > rhs.count;
        });
        return pairs;
    }
};

template<typename T> struct Rect {
    T x, y, width, height;
    Rect() {}
    Rect(T x, T y, T width, T height): x(x), y(y), width(width), height(height) {
    }
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
    double diagonal_length() {
        return sqrt(width() * width() + height() * height());
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
    bool HasPixel(const T& color) {
        for(T* p = data(); p < data() + width()*height(); p++) {
            if (*p == color)
                return true;
        }
        return false;
    }
    Point<int> IndexOf(const T& color) {
        Point<int> ret(-1, -1);
        for(unsigned int y = 0; y < this->height(); y++) {
            for(unsigned int x = 0; x < this->width(); x++) {
                if(GetPixel(x, y) == color) {
                    ret.x = x;
                    ret.y = y;
                    return ret;
                }
            }
        }
        return ret;
    }
    T GetPixel(unsigned int x, unsigned int y) const {
        assert(x < width());
        assert(y < height());
        return data_[y * width() + x];
    }
    T GetPixel(const Point<unsigned int>& point) const {
        assert(point.x < width());
        assert(point.y < height());
        return GetPixel(point.x, point.y);
    }
    T* GetPixelPointer(const Point<unsigned int>& point) const {
        assert(point.x < width());
        assert(point.y < height());
        return data() + point.y * width() + point.x;
    }
    T* GetLeftPixelPointer(const Point<unsigned int>& point) {
        assert(point.x < width());
        assert(point.y < height());
        if(point.x == 0)
            return nullptr;
        return data() + point.y * width() + point.x - 1;
    }
    T* GetRightPixelPointer(const Point<unsigned int>& point) {
        assert(point.x < width());
        assert(point.y < height());
        if(point.x == width() - 1)
            return nullptr;
        return data() + point.y * width() + point.x + 1;
    }
    T* GetUpPixelPointer(const Point<unsigned int>& point) {
        assert(point.x < width());
        assert(point.y < height());
        if(point.y == height() - 1)
            return nullptr;
        return data() + point.y * width() + point.x + width();
    }
    T* GetBottomPixelPointer(const Point<unsigned int>& point) {
        assert(point.x < width());
        assert(point.y < height());
        if(point.y == 0)
            return nullptr;
        return data() + point.y * width() + point.x - width();
    }
    unsigned int NumberOfColor(T& color) {
        unsigned int count = 0;
        for(T* p = data(); p < data() + width()*height(); p++) {
            if(*p == color) {
                count++;
            }
        }
        return count;
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
    ~Histogram2D() {
        if(data_ != nullptr) {
            delete []data_;
            data_ = nullptr;
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
    Point<unsigned int> GetMaxValuePosition() {
        T max = 0;
        Point<unsigned int> pos;
        for(unsigned int y = 0; y < height(); y++) {
            for(unsigned int x = 0; x < width(); x++) {
                if(GetValue(x, y) > max) {
                    max = GetValue(x, y);
                    pos.x = x;
                    pos.y = y;
                }
            }
        }
        return pos;
    }
    std::vector<Point<unsigned int>> GetSortedPositions() {
        for(unsigned int y = 0; y < height(); y++) {
            for(unsigned int x = 0; x < width(); x++) {
                Point<unsigned int> point(x, y);

            }
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

//std::vector<unsigned int> VerticalOverlapping(Image<BW>& bwImage);
//std::vector<unsigned int> HorizontalOverlapping(Image<BW>& bwImage);

//template<typename T> T GetConvertedImage(const Histogram2D<unsigned int>& histogram, Histogram2D2GrayImageAlgorithm algorithm) {
//    if(typeid(T) == typeid(Image<Gray<unsigned char>>) && algorithm == Histogram2D2GrayImageAlgorithm::Normal) {
//        Image<Gray<unsigned char>> image(histogram.width(), histogram.height());
//        unsigned int max = histogram.GetMaxValue();
//        for(unsigned int y = 0; y < image.height() ; y++  ) {
//            for(unsigned int x = 0 ; x < image.width(); x++ ) {
//                Gray<unsigned char> color;
//                double value = (double)histogram.GetValue(x, y) / max * 255;
//                color.value = (int)value;
//                image.SetPixel(x, y, color);
//            }
//        }
//        return image;
//    } else {
//        throw;
//    }
//}

NAMESPACE_CLUSTER_BEGIN

unsigned int ClusterCount(std::vector<unsigned int> nums);
std::vector<std::pair<unsigned int, unsigned int>> GetBorders(std::vector<unsigned int> nums);

NAMESPACE_CLUSTER_END
NAMESPACE_FEATURE_EXTRACTION_BEGIN

// if theta in [0,2¦Ð) r>=0 the TTheta should be something like unsingend float
template<typename TR, typename TTheta>
struct Line {
    TR r;
    TTheta theta;
    Line() {}
    Line(TR r, TTheta theta) {
        this->r = r;
        this->theta = theta;
    }
    template<typename T>
    std::vector<Point<unsigned int>> PointOnTheImage(const Image<T>& image) {
        if(theta == 0) {
            return PointOnTheImageTheta0(image);
        }
        std::vector<Point<unsigned int>> ret;
        for(unsigned int x = 0; x < image.width(); x++) {
            auto costheta = cos(theta * M_PI / 180);
            auto sintheta = sin(theta * M_PI / 180);
            double y = (r - x * costheta) / sintheta;
            if(y + 0.5 < 0)
                continue;
            unsigned int uiy = (unsigned int)(y + 0.5);
            if(uiy >= image.height())
                continue;
            Point<unsigned int> point(x, uiy);
            ret.push_back(point);
            //if(std::find(ret.begin(), ret.end(), point) == std::end(ret))
            //ret.push_back(point);
        }
        return ret;
    }
    bool operator==(const Line<TR, TTheta>& rhs) const {
        return IsEqual(this->r, rhs.r) && IsEqual(this->theta, rhs.theta);
    }
private:
    template<typename T>
    std::vector<Point<unsigned int>> PointOnTheImageTheta0(const Image<T>& image) {
        std::vector<Point<unsigned int>> ret;
        if(r < 0 || r + 0.5 >= image.width())
            return ret;
        for(unsigned int y = 0; y < image.height(); y++) {
            // costheta = 1;
            // sintheta = 0;
            double x = r;
            unsigned int uix = (unsigned int)(x + 0.5);
            Point<unsigned int> point(uix, y);
            ret.push_back(point);
        }
        return ret;
    }
};

//bool IsImageHasLine(const Image<BW>& image,
//                    float min_r, float max_r, float r_step,
//                    unsigned int min_theta, unsigned int max_theta, unsigned int theta_step,
//                    unsigned int line_min_count);
//bool IsImageHasClosedSharp(const Image<BW>& image);
//std::vector<Point<float>> ClosedSharpCenters(const Image<BW>& image) ;
//unsigned int NumberOfBodyIntersections(const Image<BW>& image, float r, unsigned int theta);
//unsigned int NumberOfEdgeIntersections(const Image<BW>& image, float r, unsigned int theta);
//Counter<Line<float, unsigned int>> ExtractLine(const Image<BW>& image);
NAMESPACE_FEATURE_EXTRACTION_END
NAMESPACE_FEATURE_DETECTION_BEGIN
//enum class EdgeDetectionAlgorithm {
//    BottomMinusUpNoNegative,
//    UpMinusBottomNoNegative,
//    RightMinusLeftNoNegative
//};
//template<typename T> T GetEdge(const Image<BW>& image, EdgeDetectionAlgorithm algorithm) {
//    if(typeid(T) == typeid(Image<BW>)) {
//        switch (algorithm) {
//        case EdgeDetectionAlgorithm::BottomMinusUpNoNegative: {
//            Image<BW> ret(image.width(), image.height());
//            for(unsigned int x = 0 ; x < ret.width(); x++ ) {
//                unsigned int y = ret.height() - 1;
//                ret.SetPixel(x, y, image.GetPixel(x, y));
//            }
//            for(unsigned int y = 0; y < ret.height() - 1 ; y++  ) {
//                for(unsigned int x = 0 ; x < ret.width(); x++ ) {
//                    int delta = image.GetPixel(x, y).color - image.GetPixel(x, y + 1).color;
//                    if(delta == -1)
//                        ret.SetPixel(x, y, BW(BW::Color::Black));
//                    else {
//                        ret.SetPixel(x, y,  BW(BW::Color::White));
//                    }
//                }
//            }
//            return ret;
//        }
//        case EdgeDetectionAlgorithm::UpMinusBottomNoNegative: {
//            Image<BW> ret(image.width(), image.height());
//            for(unsigned int x = 0 ; x < ret.width(); x++ ) {
//                unsigned int y = 0;
//                ret.SetPixel(x, y, image.GetPixel(x, y));
//            }
//            for(unsigned int y = 1; y < ret.height()  ; y++  ) {
//                for(unsigned int x = 0 ; x < ret.width(); x++ ) {
//                    int delta = image.GetPixel(x, y).color - image.GetPixel(x, y - 1).color;
//                    if(delta == -1)
//                        ret.SetPixel(x, y, BW(BW::Color::Black));
//                    else {
//                        ret.SetPixel(x, y,  BW(BW::Color::White));
//                    }
//                }
//            }
//            return ret;
//        }
//        case EdgeDetectionAlgorithm::RightMinusLeftNoNegative: {
//            Image<BW> ret(image.width(), image.height());
//            for(unsigned int y = 0 ; y < ret.height(); y++ ) {
//                unsigned int x = 0;
//                ret.SetPixel(x, y, image.GetPixel(x, y));
//            }
//            for(unsigned int x = 1; x < ret.width()  ; x++  ) {
//                for(unsigned int y = 0 ; y < ret.height(); y++ ) {
//                    int delta = image.GetPixel(x, y).color - image.GetPixel(x - 1, y ).color;
//                    if(delta == -1)
//                        ret.SetPixel(x, y, BW(BW::Color::Black));
//                    else {
//                        ret.SetPixel(x, y,  BW(BW::Color::White));
//                    }
//                }
//            }
//            return ret;
//        }
//        default:
//            throw;
//        }
//
//    } else {
//        throw;
//    }
//}
NAMESPACE_FEATURE_DETECTION_END
NAMESPACE_PHOENIX_END
