#include "Image.h"

NAMESPACE_IMAGEPP_BEGIN
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
NAMESPACE_FEATURE_EXTRACTION_BEGIN

bool IsImageHasLine(const Image<BW>& image,
                    float min_r, float max_r, float r_step,
                    unsigned int min_theta, unsigned int max_theta, unsigned int theta_step,
                    unsigned int line_min_count) {
    Counter<Line<float, unsigned int>> counter;
    Line<float, unsigned int> line;
    for(line.theta = min_theta; line.theta < max_theta; line.theta += theta_step) {
        for(line.r = min_r; line.r < max_r; line.r += r_step) {
            auto linePoint = line.PointOnTheImage(image);
            for(auto& p : linePoint) {
                if(image.GetPixel(p).color == BW::Color::Black) {
                    counter.Count(line, 1);
                }
            }
        }
    }
    if(counter.is_empty())
        return false;
    if(counter.GetMaxCount() >= line_min_count)
        return true;
    else
        return false;
}
Counter<Line<float, unsigned int>> ExtractLine(const Image<BW>& image) {
    //theta [0,¦Ð)
    auto theta_step = 15;
    auto r_step = 0.1;
    unsigned int height = (unsigned int)(9 / r_step) * 2;

    //Histogram2D<unsigned int> histogram(180 / 15, height, 0);
    /*std::vector<Point<unsigned int>> blackPoints;
    for(unsigned int y = 0; y < image.height(); y++) {
        for(unsigned int x = 0; x < image.width(); x++) {
            if(image.GetPixel(x, y).color == BW::Color::Black) {
                blackPoints.push_back(Point<unsigned int>(x, y));
            }
        }
    }*/
    Counter<Line<float, unsigned int>> counter;
    Line<float, unsigned int> line;
    for(line.theta = 0; line.theta < 180; line.theta += 15) {
        for(line.r = -9.0f; line.r < 9; line.r += 0.1f) {
            auto linePoint = line.PointOnTheImage(image);
            for(auto& p : linePoint) {
                if(image.GetPixel(p).color == BW::Color::Black) {
                    counter.Count(line, 1);
                }
            }
        }
    }
    return counter;
}
NAMESPACE_FEATURE_EXTRACTION_END
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
NAMESPACE_IMAGEPP_END