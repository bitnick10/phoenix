#include "Image.h"

//NAMESPACE_IMAGEPP_BEGIN
//std::vector<unsigned int> VerticalOverlapping(Image<BW>& bwImage) {
//    std::vector<unsigned int> overlapped(bwImage.width(), 0);
//    for(unsigned int x = 0; x < bwImage.width(); x++) {
//        for(unsigned int y = 0; y < bwImage.height(); y++) {
//            if(bwImage.GetPixel(x, y).color == BW::Color::Black)
//                overlapped[x] += 1;
//        }
//    }
//    return overlapped;
//}
//std::vector<unsigned int> HorizontalOverlapping(Image<BW>& bwImage) {
//    std::vector<unsigned int> overlapped(bwImage.height(), 0);
//    for(unsigned int y = 0; y < bwImage.height(); y++) {
//        for(unsigned int x = 0; x < bwImage.width(); x++) {
//            if(bwImage.GetPixel(x, y).color == BW::Color::Black)
//                overlapped[y] += 1;
//        }
//    }
//    return overlapped;
//}
//NAMESPACE_FEATURE_EXTRACTION_BEGIN

//bool IsImageHasLine(const Image<BW>& image,
//                    float min_r, float max_r, float r_step,
//                    unsigned int min_theta, unsigned int max_theta, unsigned int theta_step,
//                    unsigned int line_min_count) {
//    Counter<Line<float, unsigned int>> counter;
//    Line<float, unsigned int> line;
//    for(line.theta = min_theta; line.theta < max_theta; line.theta += theta_step) {
//        for(line.r = min_r; line.r < max_r; line.r += r_step) {
//            auto linePoint = line.PointOnTheImage(image);
//            for(auto& p : linePoint) {
//                if(image.GetPixel(p).color == BW::Color::Black) {
//                    counter.Count(line, 1);
//                }
//            }
//        }
//    }
//    if(counter.is_empty())
//        return false;
//    if(counter.GetMaxCount() >= line_min_count)
//        return true;
//    else
//        return false;
//}
//std::vector<Point<unsigned int>> CheckPointAround(Image<BW>& image, std::vector<Point<unsigned int>> points) {
//    std::vector<Point<unsigned int>> newPoints;
//    for(Point<unsigned int>& p : points) {
//        auto leftPointer = image.GetLeftPixelPointer(p);
//        if(leftPointer != nullptr) {
//            if(leftPointer->color == BW::Color::White) {
//                leftPointer->color = BW::Color::Unknown3;
//                newPoints.push_back(Point<unsigned int>(p.x - 1, p.y));
//            }
//        }
//        auto rightPointer = image.GetRightPixelPointer(p);
//        if(rightPointer != nullptr) {
//            if(rightPointer->color == BW::Color::White) {
//                rightPointer->color = BW::Color::Unknown3;
//                newPoints.push_back(Point<unsigned int>(p.x + 1, p.y));
//            }
//        }
//        auto upPointer = image.GetUpPixelPointer(p);
//        if(upPointer != nullptr) {
//            if(upPointer->color == BW::Color::White) {
//                upPointer->color = BW::Color::Unknown3;
//                newPoints.push_back(Point<unsigned int>(p.x , p.y + 1));
//            }
//        }
//        auto bottomPointer = image.GetBottomPixelPointer(p);
//        if(bottomPointer != nullptr) {
//            if(bottomPointer->color == BW::Color::White) {
//                bottomPointer->color = BW::Color::Unknown3;
//                newPoints.push_back(Point<unsigned int>(p.x , p.y - 1));
//            }
//        }
//    }
//    return newPoints;
//}
//Image<BW> FillOutside(const Image<BW>& image) {
//    std::vector<Point<unsigned int>> points;
//    Image<BW> newImage(image);
//    for(unsigned int y = 0; y < image.height(); y++) {
//        Point<unsigned int> pt(0, y);
//        if(image.GetPixel(pt).color == BW::Color::White)
//            points.push_back(Point<unsigned int>(0, y));
//    }
//    for(unsigned int y = 0; y < image.height(); y++) {
//        Point<unsigned int> pt(image.width() - 1, y);
//        if(image.GetPixel(pt).color == BW::Color::White)
//            points.push_back(pt);
//    }
//    for(unsigned int x = 1; x < image.width() - 1; x++) {
//        Point<unsigned int> pt(x, 0);
//        if(image.GetPixel(pt).color == BW::Color::White)
//            points.push_back(pt);
//    }
//    for(unsigned int x = 1; x < image.width() - 1; x++) {
//        Point<unsigned int> pt(x, image.height() - 1);
//        if(image.GetPixel(pt).color == BW::Color::White)
//            points.push_back(pt);
//    }
//    for(Point<unsigned int>& p : points) {
//        unsigned char* pp = (unsigned char*)newImage.GetPixelPointer(p);
//        *pp = 3;
//    }
//    std::vector<Point<unsigned int>> nextPoints = CheckPointAround(newImage, points);
//    while(nextPoints.size() > 0) {
//        nextPoints = CheckPointAround(newImage, nextPoints);
//    }
//    return newImage;
//}
//bool IsImageHasClosedSharp(const Image<BW>& image) {
//    auto newImage = FillOutside(image);
//    return newImage.HasPixel(BW(BW::Color::White));
//}

//std::vector<Point<float>> ClosedSharpCenters(const Image<BW>& image) {
//    Image<BW> filledImage = FillOutside(image);
//    std::vector<Point<float>> ret;
//    for(auto index = filledImage.IndexOf(BW(BW::Color::White)); index.x != -1; index = filledImage.IndexOf(BW(BW::Color::White))) {
//        std::vector<Point<unsigned int>> points;
//        std::vector<Point<unsigned int>> nextPoints;
//        Point<unsigned int> pt;
//        pt.x = index.x;
//        pt.y = index.y;
//        nextPoints.push_back(pt);
//        filledImage.GetPixelPointer(pt)->color = BW::Color::Unknown3;
//        points.insert(points.end(), nextPoints.begin(), nextPoints.end());
//        while(nextPoints.size() > 0) {
//            nextPoints = CheckPointAround(filledImage, nextPoints);
//            points.insert(points.end(), nextPoints.begin(), nextPoints.end());
//        }
//        Point<unsigned int> acc(0, 0);
//        for(Point<unsigned int>& p : points) {
//            acc.x += p.x;
//            acc.y += p.y;
//        }
//        Point<float> av;
//        av.x = (float)acc.x / points.size();
//        av.y = (float)acc.y / points.size();
//        ret.push_back(av);
//    }
//    return ret;
//}
//unsigned int NumberOfBodyIntersections(const Image<BW>& image, float r, unsigned int theta) {
//    bool lastInBlack = false;
//    unsigned int number = 0;
//    Line<float, unsigned int> line(r, theta);
//    auto linePoint = line.PointOnTheImage(image);
//    for(auto& p : linePoint) {
//        if(image.GetPixel(p).color == BW::Color::Black) {
//            if(lastInBlack) {
//                continue;
//            }
//            number++;
//            lastInBlack = true;
//        } else {
//            lastInBlack = false;
//        }
//    }
//    return number;
//}
//unsigned int NumberOfEdgeIntersections(const Image<BW>& image, float r, unsigned int theta) {
//    unsigned int number = 0;
//    Line<float, unsigned int> line(r, theta);
//    auto linePoint = line.PointOnTheImage(image);
//    for(auto& p : linePoint) {
//        if(image.GetPixel(p).color == BW::Color::Black) {
//            number++;
//        }
//    }
//    return number;
//}
//Counter<Line<float, unsigned int>> ExtractLine(const Image<BW>& image) {
//    //theta [0,¦Ð)
//    auto theta_step = 15;
//    auto r_step = 0.1;
//    unsigned int height = (unsigned int)(9 / r_step) * 2;
//
//    //Histogram2D<unsigned int> histogram(180 / 15, height, 0);
//    /*std::vector<Point<unsigned int>> blackPoints;
//    for(unsigned int y = 0; y < image.height(); y++) {
//        for(unsigned int x = 0; x < image.width(); x++) {
//            if(image.GetPixel(x, y).color == BW::Color::Black) {
//                blackPoints.push_back(Point<unsigned int>(x, y));
//            }
//        }
//    }*/
//    Counter<Line<float, unsigned int>> counter;
//    Line<float, unsigned int> line;
//    for(line.theta = 0; line.theta < 180; line.theta += 15) {
//        for(line.r = -9.0f; line.r < 9; line.r += 0.1f) {
//            auto linePoint = line.PointOnTheImage(image);
//            for(auto& p : linePoint) {
//                if(image.GetPixel(p).color == BW::Color::Black) {
//                    counter.Count(line, 1);
//                }
//            }
//        }
//    }
//    return counter;
//}
//NAMESPACE_FEATURE_EXTRACTION_END
//NAMESPACE_CLUSTER_BEGIN

//unsigned int ClusterCount(std::vector<unsigned int> nums) {
//    unsigned int count = 0;
//    bool in_cluster = true;
//    for(unsigned int& n : nums) {
//        if(in_cluster) {
//            if(n > 0) {
//                continue;
//            } else {
//                in_cluster = false;
//            }
//        }
//        if(!in_cluster) {
//            if(n > 0) {
//                in_cluster = true;
//                count++;
//            } else {
//                continue;
//            }
//        }
//    }
//    return count;
//}
//std::vector<std::pair<unsigned int, unsigned int>> GetBorders(std::vector<unsigned int> nums) {
//    std::vector<std::pair<unsigned int, unsigned int>> borders;
//    bool in_cluster = false;
//    for(unsigned int i = 0; i < nums.size(); i++) {
//        unsigned int n = nums[i];
//        if(in_cluster) {
//            if(n > 0) {
//                continue;
//            } else {
//                in_cluster = false;
//                borders[borders.size() - 1].second = i - 1;
//            }
//        }
//        if(!in_cluster) {
//            if(n > 0) {
//                std::pair<unsigned int, unsigned int> pair;
//                pair.first = i ;
//                in_cluster = true;
//                borders.push_back(pair);
//            } else {
//                continue;
//            }
//        }
//    }
//    return borders;
//}

//NAMESPACE_CLUSTER_END
//NAMESPACE_IMAGEPP_END