#include "ipp.h"
template<typename T>
class Map {
private:
    unsigned int width, height;
    T* data;
public:
    auto __Width__() const -> unsigned int {
        return width;
    }
    void __Width__(unsigned int value) {
        width = value;
    }
    auto __Height__() const ->unsigned int {
        return height;
    }
    void __Height__(unsigned int value) {
        height = value;
    }
public:
    Map():
        data(nullptr),
        width(0),
        height(0) {
    }
    Map(const Map<T>& map) {
        __Width__(map.width);
        __Height__(map.height);
        data = new T[width * height];
        memcpy(this->data, map.data, width * height * sizeof(T));
    }
    Map(const T* data, int width, int height) {
        __Width__(width);
        __Height__(height);
        this->data = new T[width * height];
        memcpy(this->data, data, width * height * sizeof(T));
    }
    Map(const T& initData, int width, int height) {
        __Width__(width);
        __Height__(height);
        this->data = new T[width * height];
        T* p = this->data;
        T* pEnd = this->data + __Width__() * __Height__();
        while (p < pEnd) {
            *p = initData;
            p++;
        }
    }
    ~Map() {
        if (data != nullptr) {
            delete []data;
            data = nullptr;
        }
    }
public:
    void SetValueAt(const Point<unsigned int>& point, const T& value) {
        SetValueAt(point.x, point.y, value);
    }
    void SetValueAt(unsigned int x, unsigned int y, const T& value) {
        data[y * __Width__() + x] = value;
    }
    T GetValueAt(const Point<unsigned int>& point) const {
        return GetValueAt(point.x, point.y);
    }
    T GetValueAt(unsigned int x, unsigned int y) const {
        return data[y * __Width__() + x];
    }
    T operator[](const Point<unsigned int>& point) {
        return data[point.y * __Width__() + point.x];
    }
};