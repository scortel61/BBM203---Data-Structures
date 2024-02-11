#ifndef EDGE_DETECTOR_H
#define EDGE_DETECTOR_H

#include "ImageMatrix.h"
#include "Convolution.h"
#include <vector>

class EdgeDetector {
public:
    EdgeDetector();

    ~EdgeDetector();

    std::vector<std::pair<int, int>> detectEdges(const ImageMatrix &input_image);

private:
    double ** g_x;
    double ** g_y;
};

#endif // EDGE_DETECTOR_H


