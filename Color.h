#pragma once

#include <cmath>
#include <cassert>
namespace phoenix {
struct BW {
    enum Color : unsigned char {
        Black,
        White,
        Unknown2,
        Unknown3,
        Unknown4,
        Unknown5
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
//template<typename T> struct RGB {
//    T r, g, b;
//    RGB() {
//    }
//    RGB(T r, T g, T b) {
//        this->r = r;
//        this->g = g;
//        this->b = b;
//    }
//    bool operator==(const RGB<T>& rhs) const {
//        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
//    }
//    const RGB<T>& operator=(const BW& bw) {
//        this->r = bw.color * 255;
//        this->g = bw.color * 255;
//        this->b = bw.color * 255;
//        return *this;
//    }
//};
struct RGB24 {
    unsigned char r, g, b; // r（[0, 255] g（[0, 255] b（[0, 255]
    RGB24(unsigned char r, unsigned char g, unsigned char b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    bool operator==(const RGB24& rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
    }
};
struct RGB96 {
    float r = 0.0f; // r（[0,1]
    float g = 0.0f; // g（[0,1]
    float b = 0.0f; // b（[0,1]
    RGB96() {}
    RGB96(float r, float g, float b) {
        assert(0.0f <= r && r <= 1.0f);
        assert(0.0f <= g && g <= 1.0f);
        assert(0.0f <= b && b <= 1.0f);
        this->r = r;
        this->g = g;
        this->b = b;
    }
    bool operator==(const RGB96& rhs) const {
        return this->r == rhs.r && this->g == rhs.g && this->b == rhs.b;
    }
};

template<typename H, typename SB> struct HSB {
    H h;
    SB s, b;
    HSB() {}
    HSB(const HSB<H, SB>& rhs) {
        this->h = rhs.h;
        this->s = rhs.s;
        this->b = rhs.b;
    }
};
struct HSB96 {
    float h = 0.0; // h（[0, 360)
    float s = 0.0; // s（[0, 1]
    float b = 0.0; // b（[0, 1]
    HSB96() {}
    HSB96(float h, float s, float b) {
        assert(0.0f <= h && h < 360.0f);
        assert(0.0f <= s && s <= 1.0f);
        assert(0.0f <= b && b <= 1.0f);
        this->h = h;
        this->s = s;
        this->b = b;
    }
};
typedef HSB<unsigned short, unsigned char> HSB32; // h（[0,359] g（[0,100] b（[0,100]

template<typename T> struct ARGB {
    T a, r, g, b;
};

inline RGB96 HSB96ToRGB96(const HSB96& hsb) {
    int hi = (int)(hsb.h / 60) % 6;
    float f = hsb.h / 60 - hi;
    float p = hsb.b * (1 - hsb.s);
    float q = hsb.b * (1 - f * hsb.s);
    float t = hsb.b * (1 - (1 - f) * hsb.s);
    if (hi == 0) {
        RGB96 ret(hsb.b, t, p);
        return ret;
    } else if (hi == 1) {
        RGB96 ret(q, hsb.b, p);
        return ret;
    } else if (hi == 2) {
        RGB96 ret(p, hsb.b, t);
        return ret;
    } else if (hi == 3) {
        RGB96 ret(p, q, hsb.b);
        return ret;
    } else if (hi == 4) {
        RGB96 ret(t, p, hsb.b);
        return ret;
    } else {
        RGB96 ret(hsb.b, p, q);
        return ret;
    }
}

inline RGB24 RGB96ToRGB24(const RGB96& rgb) {
    RGB24 ret((unsigned char)(rgb.r * 255 + 0.5), (unsigned char)(rgb.g * 255 + 0.5), (unsigned char)(rgb.b * 255 + 0.5));
    return ret;
}
}