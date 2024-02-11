#include "EncodeMessage.h"
#include <cmath>
#include <bitset>
#include <iostream>
#include <algorithm>

bool comparePairs(const std::pair<int, int> &a, const std::pair<int, int> &b) {
    if (a.first == b.first) {
        return a.second < b.second;
    }
    return a.first < b.first;
}

// Default Constructor
EncodeMessage::EncodeMessage() = default;

// Destructor
EncodeMessage::~EncodeMessage() = default;

// Function to encode a message into an image matrix
ImageMatrix EncodeMessage::encodeMessageToImage(const ImageMatrix &img, const std::string &message,
                                                const std::vector<std::pair<int, int>> &positions) {

    auto new_postitions = positions;
    std::sort(new_postitions.begin(), new_postitions.end(), comparePairs);

    const ImageMatrix &encodedImg(img);

    std::vector<int> primeIndexes = findPrimeIndexes(message);
    std::string transformed_message = transformGivenIndexesWithFibonacci(primeIndexes, message);

    std::string shiftedMessage = applyRightCircularShift(transformed_message);

    std::string binaryStrings;
    for (auto ch: shiftedMessage) {
//        std::string binaryString = std::bitset<7>(ch).to_string();
        std::string binaryString = std::bitset<sizeof(int) * 8>((int) ch).to_string();
        size_t pos = binaryString.find_first_not_of('0');
        if (pos != std::string::npos) {
            binaryString = binaryString.substr(pos);
        } else {
            binaryString = "0"; // If the number is 0, the binary string is "0"
        }
        binaryStrings += binaryString;
    }
//    std::cout << binaryStrings << std::endl;
    int str_pointer = 0;

    for (auto position: new_postitions) {
        int i = position.first;
        int j = position.second;
        int lsb = ((int) binaryStrings[str_pointer++]) & 1;
        int first_value = encodedImg.get_data()[i][j];
        int last_value = first_value & ~1;
        last_value = last_value | lsb;
        encodedImg.get_data()[i][j] = last_value;
    }

    return encodedImg;
}

std::vector<int> EncodeMessage::findPrimeIndexes(const std::string &str) {
    std::vector<int> primeIndexes;
    int length = str.length();

    for (int i = 2; i < length; i++) {
        if (isPrime(i)) {
            primeIndexes.push_back(i);
        }
    }

    return primeIndexes;
}

bool EncodeMessage::isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

std::string
EncodeMessage::transformGivenIndexesWithFibonacci(const std::vector<int> &indexes, const std::string &str) {

    std::vector<char> chars;
    for (char ch: str) {
        chars.push_back(ch);
    }
    for (int index: indexes) {
        int valueOfChar = (int) chars.at(index);
        int fibonacciValue = fibonacci(index);
        valueOfChar += fibonacciValue;
        if (valueOfChar <= 32) {
            valueOfChar += 33;
        }
        if (valueOfChar >= 127) {
            valueOfChar = 126;
        }
        chars[index] = (char) valueOfChar;
    }

    std::string result;
    for (auto ch: chars) {
        result += ch;
    }

    return result;
}

int EncodeMessage::fibonacci(int index) {
    if (index <= 0) return 0;
    if (index == 1) return 1;

    int a = 0;
    int b = 1;
    int fib = 1;

    for (int i = 2; i <= index; i++) {
        fib = a + b;
        a = b;
        b = fib;
    }

    return fib;
}

std::string EncodeMessage::applyRightCircularShift(const std::string &str) {
    int len = str.length();
    int shiftAmount = len / 2;
    std::string shiftedStr = str;

    for (int i = 0; i < len; i++) {
        int newIndex = (i + shiftAmount) % len;
        shiftedStr[newIndex] = str[i];
    }

    return shiftedStr;
}
