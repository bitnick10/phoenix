#include <gtest/gtest.h>
#include "../../AStar.h"
TEST(AStarTest, SimpleOne) {
    AStar astar;
    bool passableData[9] = {
        1, 1, 1,
        1, 1, 1,
        1, 1, 1
    };
    Map<bool> passableMap(passableData, 3, 3);
    Point<unsigned int> begin = { 0, 1 };
    Point<unsigned int> end = { 2, 1 };
    std::vector<Point<unsigned int>> path = astar.GetPath(passableMap, begin, end);

    EXPECT_EQ(2, path.size());
    EXPECT_EQ(Point<unsigned int>(1, 1), path[0]);
    EXPECT_EQ(Point<unsigned int>(2, 1), path[1]);
}
TEST(AStarTest, SimpleOne2) {
    AStar astar;
    bool passableData[42] = {
        1, 1, 1, 1, 1, 1, 1,
        1, 0, 1, 0, 1, 1, 1,
        1, 1, 1, 0, 1, 1, 1,
        1, 1, 1, 0, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1
    };
    int result[42] = { // map upside down
        1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 1, 1, 1,
        1, 9, 1, 0, 1, 1, 1,
        1, 0, 2, 0, 6, 9, 1,
        1, 1, 3, 4, 5, 6, 1
    };
    Map<bool> passableMap(passableData, 7, 6);
    Point<unsigned int> begin = { 1, 2 };
    Point<unsigned int> end = { 5, 1 };
    std::vector<Point<unsigned int>> path = astar.GetPath(passableMap, begin, end);

    EXPECT_EQ(7, path.size());
    EXPECT_EQ(Point<unsigned int>(2, 2), path[0]);
    EXPECT_EQ(Point<unsigned int>(2, 1), path[1]);
    EXPECT_EQ(Point<unsigned int>(2, 0), path[2]);
    EXPECT_EQ(Point<unsigned int>(3, 0), path[3]);
    EXPECT_EQ(Point<unsigned int>(4, 0), path[4]);
    EXPECT_EQ(Point<unsigned int>(5, 1), path[6]);
}