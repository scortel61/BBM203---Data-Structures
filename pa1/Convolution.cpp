#include <iostream>

#include "Convolution.h"

// Default constructor 
Convolution::Convolution() = default;

// Parametrized constructor for custom kernel and other parameters
Convolution::Convolution(double **customKernel, int kh, int kw, int stride_val, bool pad) {
    this->customKernel = new double * [kh];
    for (int i = 0; i < kh; ++i) {
        this->customKernel[i] = new double [kw];
    }
    for (int i = 0; i < kh; ++i) {
        for (int j = 0; j < kw; ++j) {
            this->customKernel[i][j] = customKernel[i][j];
        }
    }
    this->kh = kh;
    this->kw = kw;
    this->stride_val = stride_val;
    this->pad = pad;
}

// Destructor
Convolution::~Convolution() {
    if (this->customKernel != nullptr) {
        for (int i = 0; i < this->kh; ++i) {
            delete[] this->customKernel[i];
        }
        delete[] this->customKernel;
    }
}

// Copy constructor
Convolution::Convolution(const Convolution &other) {
    this->kh = other.kh;
    this->kw = other.kw;
    this->stride_val = other.stride_val;
    this->pad = other.pad;

    this->customKernel = new double *[this->kh];
    for (int i = 0; i < this->kh; ++i) {
        this->customKernel[i] = new double[this->kw];
    }
    for (int i = 0; i < this->kh; ++i) {
        for (int j = 0; j < this->kw; ++j) {
            this->customKernel[i][j] = other.customKernel[i][j];
        }
    }
}

// Copy assignment operator
Convolution &Convolution::operator=(const Convolution &other) {
    for (int i = 0; i < this->kh; ++i) {
        delete[] this->customKernel[i];
    }
    delete[] this->customKernel;

    this->kh = other.kh;
    this->kw = other.kw;
    this->stride_val = other.stride_val;
    this->pad = other.pad;

    this->customKernel = new double *[this->kh];
    for (int i = 0; i < this->kh; ++i) {
        this->customKernel[i] = new double[this->kw];
    }
    for (int i = 0; i < this->kh; ++i) {
        for (int j = 0; j < this->kw; ++j) {
            this->customKernel[i][j] = other.customKernel[i][j];
        }
    }
    return *this;
}


//void printtMatrix(double **matrix, int rows, int cols) {
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
//}

// Convolve Function: Responsible for convolving the input image with a kernel and return the convolved image.
ImageMatrix Convolution::convolve(const ImageMatrix &input_image) const {
    int paddingAmount = this->pad ? 1 : 0;
    double paddingValue = 0.0;
    auto *inputMatrixCopy = new ImageMatrix(input_image);
    if (this->pad) {
        inputMatrixCopy->addPadding(paddingAmount, paddingValue);
    }
    double **data = inputMatrixCopy->get_data();
    int inputHeight = inputMatrixCopy->get_height();
    int inputWidth = inputMatrixCopy->get_width();
    int resultHeight = ((inputHeight - this->kh + (2 * paddingAmount)) / this->stride_val) + 1;
    int resultWidth = ((inputWidth - this->kw + (2 * paddingAmount)) / this->stride_val) + 1;
    if (this->pad) {
        resultHeight -= 2 * paddingAmount;
        resultWidth -= 2 * paddingAmount;
    }
    auto **resultMatrix = new double *[resultHeight];
    for (int i = 0; i < resultHeight; ++i) {
        resultMatrix[i] = new double[resultWidth];
    }
    for (int i = 0; i < resultHeight; ++i) {
        for (int j = 0; j < resultWidth; ++j) {
            resultMatrix[i][j] = paddingValue;
        }
    }
    int heightPointer = 0;
    int widthPointer = 0;
    for (int i = 0; i < inputHeight; i += this->stride_val) {
        for (int j = 0; j < inputWidth; j += this->stride_val) {
            if (i + this->kh > inputHeight || j + this->kw > inputWidth) {
                widthPointer++;
                continue;
            }
            double total = 0;
            for (int k = 0; k < this->kh; ++k) {
                for (int l = 0; l < this->kw; ++l) {
                    total += data[i + k][j + l] * this->customKernel[k][l];
                }
            }
            resultMatrix[heightPointer][widthPointer] = total;
//            printtMatrix(resultMatrix, resultHeight, resultWidth);
            widthPointer++;
        }
        heightPointer++;
        widthPointer = 0;
    }

//    for (int i = 0; i < resultHeight; ++i) {
//        for (int j = 0; j < resultWidth; ++j) {
//            if (resultMatrix[i][j] > 255) {
//                resultMatrix[i][j] = 255;
//            }
//        }
//    }

    delete inputMatrixCopy;
    auto *resultImageMatrixObject = new ImageMatrix(resultMatrix, resultHeight, resultWidth);
    for (int i = 0; i < resultHeight; ++i) {
        delete[] resultMatrix[i];
    }
    delete[] resultMatrix;
    return *resultImageMatrixObject;
}
