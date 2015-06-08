#pragma once

#include <cmath>

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

template<typename T> struct RGB {
    T r, g, b;
    RGB() {
    }
    RGB(T r, T g, T b) {
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
typedef RGB<unsigned char> RGB24; // RGB<byte> r（[0,255] g（[0,255] b（[0,255]
typedef RGB<float> RGB96;         // RGB<float> r（[0,1] g（[0,1] b（[0,1]

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
typedef HSB<unsigned short, unsigned char> HSB32; // h（[0,359] g（[0,100] b（[0,100]
typedef HSB<float, float> HSB96;// h（[0,360) s（[0,1] b（[0,1]

template<typename T> struct ARGB {
    T a, r, g, b;
};

RGB96 HSB96ToRGB96(const HSB96& hsb) {
    int hi = (int)(hsb.h / 60) % 6;
    float f = hsb.h / 60 - hi;
    float p = hsb.b * (1 - hsb.s);
    float q = hsb.b * (1 - f * hsb.s);
    float t = hsb.b * (1 - (1 - f) * hsb.s);
    if (hi == 0) {
        RGB96 ret(hsb.b, t, p);
        return ret;
    } else if(hi == 1) {
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

RGB24 RGB96ToRGB24(const RGB96& rgb) {
    RGB24 ret((unsigned char)(rgb.r * 255 + 0.5), (unsigned char)(rgb.g * 255 + 0.5), (unsigned char)(rgb.b * 255 + 0.5));
    return ret;
}