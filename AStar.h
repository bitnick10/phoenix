#include <vector>
#include "Map.h"
#include "ipp.h"

class AStar {
private:
    enum class OpenClosed {
        Open,
        Closed,
        Neither
    };
public:
    std::vector<Point<unsigned int>> GetPath(const Map<bool>& passableMap, const Point<int>& begin, const Point<int>& end) {
        unsigned int uiZero = 0;
        Map<unsigned int> MapG(uiZero, passableMap.__Width__(), passableMap.__Height__());
        Map<unsigned int> MapH(uiZero, passableMap.__Width__(), passableMap.__Height__());
        Map<OpenClosed> MapOpenClosed(OpenClosed::Neither, passableMap.__Width__(), passableMap.__Height__());
        std::vector<Point<unsigned int>> path;

        if (begin.y != passableMap.__Height__() - 1) {
            Point<unsigned int> up;
            up.x = begin.x;
            up.y = begin.y;
            MapG.SetValueAt(up, 1);
        }

        return path;
    }

};