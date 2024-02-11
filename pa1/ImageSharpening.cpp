#include <iostream>
#include "ImageSharpening.h"

//void printtttMatrix(double **matrix, int rows, int cols) {
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
//}

// Default constructor
ImageSharpening::ImageSharpening() {
    this->kernel = new double *[this->size];
    for (int i = 0; i < this->size; ++i) {
        this->kernel[i] = new double[this->size]{
                1.0 / 9.0,
                1.0 / 9.0,
                1.0 / 9.0
        };
    }
}

ImageSharpening::~ImageSharpening() {
    if (this->kernel != nullptr) {
        for (int i = 0; i < this->size; ++i) {
            delete[] this->kernel[i];
        }
        delete[] this->kernel;
    }
}

ImageMatrix ImageSharpening::sharpen(const ImageMatrix &input_image, double k) {
    Convolution convolution(
            this->kernel,
            this->size,
            this->size,
            1,
            true
    );
    ImageMatrix copyOfInput(input_image);

    ImageMatrix noisyMatrix = convolution.convolve(input_image);
//    std::cout << "noisy" << std::endl;
//    noisyMatrix.printData();

//    std::cout << "input_image" << std::endl;
//    input_image.printData();

    ImageMatrix temp0 = input_image - noisyMatrix;
//    std::cout << "input_image - noisyMatrix" << std::endl;
//    temp0.printData();
    ImageMatrix temp1 = temp0 * k;
//    std::cout << "(input_image - noisyMatrix) * k" << std::endl;
//    temp1.printData();

    ImageMatrix sharpMatrix = copyOfInput + temp1;
//    std::cout << "sharp" << std::endl;
//    sharpMatrix.printData();

    sharpMatrix.clip(255);

    return sharpMatrix;
}
