// EdgeDetector.cpp

#include "EdgeDetector.h"
#include <cmath>

// Default constructor
EdgeDetector::EdgeDetector() {
    this->g_x = new double *[3];
    this->g_x[0] = new double[3]{-1, 0, 1};
    this->g_x[1] = new double[3]{-2, 0, 2};
    this->g_x[2] = new double[3]{-1, 0, 1};

    this->g_y = new double *[3];
    this->g_y[0] = new double[3]{-1, -2, -1};
    this->g_y[1] = new double[3]{0, 0, 0};
    this->g_y[2] = new double[3]{1, 2, 1};

}

// Destructor
EdgeDetector::~EdgeDetector() = default;

// Detect Edges using the given algorithm
std::vector<std::pair<int, int>> EdgeDetector::detectEdges(const ImageMatrix &input_image) {
    ImageMatrix newMatrix(input_image.get_height(), input_image.get_width());

    Convolution convolution_x(this->g_x, 3, 3, 1, true);
    Convolution convolution_y(this->g_y, 3, 3, 1, true);

    ImageMatrix i_x = convolution_x.convolve(input_image);
    ImageMatrix i_y = convolution_y.convolve(input_image);
    double average = 0.0;
    for (int i = 0; i < input_image.get_height(); ++i) {
        for (int j = 0; j < input_image.get_width(); ++j) {
            double value = sqrt(pow(i_x.get_data()[i][j], 2) + pow(i_y.get_data()[i][j], 2));
            average += value;
            newMatrix.get_data()[i][j] = value;
        }
    }
//    newMatrix.printData();
    average = average / ((input_image.get_height() * input_image.get_width()) / 1.0);
    std::vector<std::pair<int, int>> result;
    for (int i = 0; i < input_image.get_height(); ++i) {
        for (int j = 0; j < input_image.get_width(); ++j) {
            if (newMatrix.get_data()[i][j] > average) {
                std::pair<int, int> temp(i, j);
                result.push_back(temp);
            }
        }
    }
    return result;
}

