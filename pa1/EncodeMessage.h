#ifndef ENCODE_MESSAGE_H
#define ENCODE_MESSAGE_H

#include <string>
#include <vector>
#include "ImageMatrix.h"

class EncodeMessage {
public:
    EncodeMessage();

    ~EncodeMessage();

    ImageMatrix encodeMessageToImage(const ImageMatrix &img, const std::string &message,
                                     const std::vector<std::pair<int, int>> &positions);


private:
    // Any private helper functions or variables if necessary


    static std::string applyRightCircularShift(const std::string &str);

    static int fibonacci(int index);

    static std::string transformGivenIndexesWithFibonacci(const std::vector<int> &indexes, const std::string &str);

    static bool isPrime(int n);

    static std::vector<int> findPrimeIndexes(const std::string &str);
};

#endif // ENCODE_MESSAGE_H
