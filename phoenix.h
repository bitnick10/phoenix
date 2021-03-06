#pragma once

#include <string>
#include "ImageRGB24.h"

template<typename T> struct Point {
    T x, y;
    Point() {}
    Point(const Point<T>& point) {
        this->x = point.x;
        this->y = point.y;
    }
    Point(T x, T y) {
        this->x = x;
        this->y = y;
    }
    bool operator==(const Point<T>& rhs) const {
        return (this->x == rhs.x && this->y == rhs.y);
    }
};

struct Error {
    std::string* info;
public:
    Error() {
        info = nullptr;
    }
    bool operator==(void* p) {
        return info == p;
    }
};

// https://en.wikipedia.org/wiki/Fraction_(mathematics)#Mixed_numbers
struct MixedFraction {
    unsigned int numerator;
    unsigned int denominator;
    int integer;
};