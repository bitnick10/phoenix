#include <vector>
#include <list>
#include <algorithm>
#include "Map.h"
#include "phoenix.h"

class AStar {
private:
    enum class OpenClosed {
        Open,
        Closed,
        Neither
    };
    enum class Direction {
        Up,
        Down,
        Left,
        Right,
        None
    };
public:
    std::vector<Point<unsigned int>> GetPath(const Map<bool>& passableMap, const Point<unsigned int>& begin, const Point<unsigned int>& end) {
        assert(begin.x < passableMap.__Width__());
        assert(begin.y < passableMap.__Height__());
        assert(end.x < passableMap.__Width__());
        assert(end.y < passableMap.__Height__());
        Map<bool> passableMapCopy(passableMap);
        Map<unsigned int> mapF((unsigned int)0, passableMapCopy.__Width__(), passableMapCopy.__Height__());
        std::list<Point<unsigned int>> openList;
        openList.push_back(begin);
        Map<Direction> direcitonMap(Direction::None, passableMapCopy.__Width__(), passableMapCopy.__Height__());
        std::vector<Point<unsigned int>> path;

        int currentG = 0;
        while (!openList.empty()) {
            Point<unsigned int> now = min(mapF, openList);
            openList.remove(now);
            passableMapCopy.SetValueAt(now, false);
            currentG++;
            if (now.y != passableMapCopy.__Height__() - 1) {
                Point<unsigned int> up;
                up.x = now.x;
                up.y = now.y + 1;
                if (passableMapCopy.GetValueAt(up) != false) {
                    mapF.SetValueAt(up, currentG + distance(up, end));
                    openList.push_back(up);
                    direcitonMap.SetValueAt(up, Direction::Up);
                    if (up == end)
                        break;
                }
            }
            if (now.y != 0) {
                Point<unsigned int> down;
                down.x = now.x;
                down.y = now.y - 1;
                if (passableMapCopy.GetValueAt(down) != false) {
                    mapF.SetValueAt(down, currentG + distance(down, end));
                    openList.push_back(down);
                    direcitonMap.SetValueAt(down, Direction::Down);
                    if (down == end)
                        break;
                }
            }
            if (now.x != 0) {
                Point<unsigned int> left;
                left.x = now.x - 1;
                left.y = now.y;
                if (passableMapCopy.GetValueAt(left) != false) {
                    mapF.SetValueAt(left, currentG + distance(left, end));
                    openList.push_back(left);
                    direcitonMap.SetValueAt(left, Direction::Left);
                    if (left == end)
                        break;
                }
            }
            if (now.x != passableMapCopy.__Width__() - 1) {
                Point<unsigned int> right;
                right.x = now.x + 1;
                right.y = now.y;

                if (passableMapCopy.GetValueAt(right) != false) {
                    mapF.SetValueAt(right, currentG + distance(right, end));
                    openList.push_back(right);
                    direcitonMap.SetValueAt(right, Direction::Right);
                    if (right == end)
                        break;
                }
            }
        }
        return GetPath(direcitonMap, end);
    }
    std::vector<Point<unsigned int>> GetPath(const Map<Direction>& direcitonMap, const Point<unsigned int>& end) {
        std::vector<Point<unsigned int>> path;
        path.push_back(end);
        while (true) {
            auto lastOne = path.back();
            Direction d = direcitonMap.GetValueAt(lastOne);
            if (d == Direction::None) {
                path.pop_back();
                break;
            }
            switch (d) {
            case AStar::Direction::Up: {
                Point<unsigned int> p;
                p.x = lastOne.x;
                p.y = lastOne.y - 1;
                path.push_back(p);
            }
            break;
            case AStar::Direction::Down: {
                Point<unsigned int> p;
                p.x = lastOne.x;
                p.y = lastOne.y + 1;
                path.push_back(p);
            }
            break;
            case AStar::Direction::Left: {
                Point<unsigned int> p;
                p.x = lastOne.x + 1;
                p.y = lastOne.y;
                path.push_back(p);
            }
            break;
            case AStar::Direction::Right: {
                Point<unsigned int> p;
                p.x = lastOne.x - 1;
                p.y = lastOne.y;
                path.push_back(p);
            }
            break;
            default:
                break;
            }
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    unsigned int distance(const Point<unsigned int>& point1, const Point<unsigned int>& point2) {
        return abs((int)point1.x - (int)point2.x) + abs((int)point1.y - (int)point2.y);
    }
    Point<unsigned int> min(const Map<unsigned int>& mapF, const std::list<Point<unsigned int>>& openList) {
        unsigned int min = UINT_MAX;
        Point<unsigned int> ret;
        for (const Point<unsigned int>& p : openList) {
            auto F = mapF.GetValueAt(p);
            if (F < min) {
                min = F;
                ret = p;
            }
        }
        return ret;
    }
};