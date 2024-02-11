#include "ImageMatrix.h"
#include <iostream>


// Default constructor
ImageMatrix::ImageMatrix() {
    this->data = new double *[0];
    this->height = 0;
    this->width = 0;
}


// Parameterized constructor for creating a blank image of given size
ImageMatrix::ImageMatrix(int imgHeight, int imgWidth) : height(imgHeight), width(imgWidth) {
    this->data = new double *[this->height];
    for (int i = 0; i < this->height; ++i) {
        this->data[i] = new double[this->width];
    }
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            this->data[i][j] = 0.0;
        }
    }
}

// Parameterized constructor for loading image from file. PROVIDED FOR YOUR CONVENIENCE
ImageMatrix::ImageMatrix(const std::string &filepath) {
    // Create an ImageLoader object and load the image
    ImageLoader imageLoader(filepath);

    // Get the dimensions of the loaded image
    height = imageLoader.getHeight();
    width = imageLoader.getWidth();

    // Allocate memory for the matrix
    data = new double *[height];
    for (int i = 0; i < height; ++i) {
        data[i] = new double[width];
    }

    // Copy data from imageLoader to data
    double **imageData = imageLoader.getImageData();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; j++) {
            data[i][j] = imageData[i][j];
        }
    }
}


// Destructor
ImageMatrix::~ImageMatrix() {
    for (int i = 0; i < this->height; ++i) {
        if (this->data != nullptr) delete[] this->data[i];
    }
    delete[] this->data;
}

// Parameterized constructor - direct initialization with 2D matrix
ImageMatrix::ImageMatrix(const double **inputMatrix, int imgHeight, int imgWidth) : height(imgHeight), width(imgWidth) {
    this->data = new double *[this->height];
    for (int i = 0; i < this->height; ++i) {
        this->data[i] = new double[this->width];
    }
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            this->data[i][j] = inputMatrix[i][j];
        }
    }
}

// Copy constructor
ImageMatrix::ImageMatrix(const ImageMatrix &other) : height(other.height), width(other.width) {
    this->data = new double *[this->height];
    for (int i = 0; i < this->height; ++i) {
        this->data[i] = new double[this->width];
    }
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            this->data[i][j] = other.data[i][j];
        }
    }
}

// Copy assignment operator
ImageMatrix &ImageMatrix::operator=(const ImageMatrix &other) {
    for (int i = 0; i < this->height; ++i) {
        if (this->data != nullptr) delete[] this->data[i];
    }
    delete[] this->data;

    this->height = other.height;
    this->width = other.width;
    this->data = new double *[this->height];
    for (int i = 0; i < this->height; ++i) {
        this->data[i] = new double[this->width];
    }
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            this->data[i][j] = other.data[i][j];
        }
    }
    return *this;
}



// Overloaded operators

// Overloaded operator + to add two matrices
ImageMatrix ImageMatrix::operator+(const ImageMatrix &other) const {
    ImageMatrix result(height, width); // Create a new matrix for the result
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = this->data[i][j] + other.data[i][j];
//            printf("%lf = %lf + %lf\n", result.data[i][j], this->data[i][j], other.data[i][j]);
        }
    }
    return result;
}

// Overloaded operator - to subtract two matrices
ImageMatrix ImageMatrix::operator-(const ImageMatrix &other) const {
    ImageMatrix result(height, width); // Create a new matrix for the result
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = this->data[i][j] - other.data[i][j];
//            printf("%lf = %lf - %lf\n", result.data[i][j], this->data[i][j], other.data[i][j]);
        }
    }
    return result;
}

// Overloaded operator * to multiply a matrix with a scalar
ImageMatrix ImageMatrix::operator*(const double &scalar) const {
    ImageMatrix result(height, width); // Create a new matrix for the result
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            result.data[i][j] = data[i][j] * scalar;
//            printf("%lf = %lf * %lf\n", result.data[i][j], this->data[i][j], scalar);
        }
    }
    return result;
}


// Getter function to access the data in the matrix
double **ImageMatrix::get_data() const {
    return this->data;
}

// Getter function to access the data at the index (i, j)
double ImageMatrix::get_data(int i, int j) const {
    return this->data[i][j];
}

//void printttMatrix(double **matrix, int rows, int cols) {
//    for (int i = 0; i < rows; ++i) {
//        for (int j = 0; j < cols; ++j) {
//            std::cout << matrix[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;
//}

void ImageMatrix::addPadding(int paddingAmount, double paddingValue) {
    int newHeight = this->height + (2 * paddingAmount);
    int newWidth = this->width + (2 * paddingAmount);

    auto **newData = new double *[newHeight];
    for (int i = 0; i < newHeight; ++i) {
        newData[i] = new double[newWidth];
        for (int j = 0; j < newWidth; ++j) {
            if (i < paddingAmount || i >= newHeight - paddingAmount || j < paddingAmount ||
                j >= newWidth - paddingAmount) {
                newData[i][j] = paddingValue;
            } else {
                newData[i][j] = this->data[i - paddingAmount][j - paddingAmount];
            }
        }
    }
//    printttMatrix(newData, newHeight, newWidth);
    for (int i = 0; i < this->height; ++i) {
        delete[] this->data[i];
    }
    delete[] this->data;

    this->data = newData;
    this->height = newHeight;
    this->width = newWidth;
}

int ImageMatrix::get_height() const {
    return height;
}

int ImageMatrix::get_width() const {
    return width;
}

ImageMatrix::ImageMatrix(double **inputMatrix, int imgHeight, int imgWidth) : height(imgHeight), width(imgWidth) {
    this->data = new double *[this->height];
    for (int i = 0; i < this->height; ++i) {
        this->data[i] = new double[this->width];
    }
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            this->data[i][j] = inputMatrix[i][j];
        }
    }
}

void ImageMatrix::clip(int clipTo) {
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            if (this->data[i][j] > clipTo) {
                this->data[i][j] = clipTo;
            }
        }
    }
}

void ImageMatrix::printData() const {
    for (int i = 0; i < this->height; ++i) {
        for (int j = 0; j < this->width; ++j) {
            std::cout << this->data[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}




