// DecodeMessage.cpp

#include "DecodeMessage.h"
#include <iostream>

// Default constructor
DecodeMessage::DecodeMessage() = default;
// Nothing specific to initialize here


// Destructor
DecodeMessage::~DecodeMessage() = default;
// Nothing specific to clean up


std::string
DecodeMessage::decodeFromImage(const ImageMatrix &image, const std::vector<std::pair<int, int>> &edgePixels) {
    std::string binaryString;

    for (const auto &pixelCoord: edgePixels) {
        int i = pixelCoord.first;
        int j = pixelCoord.second;
        int pixelValue = image.get_data()[i][j];
        binaryString += std::to_string(pixelValue & 1);
    }

    size_t paddingLength = 7 - binaryString.length() % 7;
    if (paddingLength != 7) {
        binaryString.insert(0, paddingLength, '0');
    }

    std::string message;
    for (size_t i = 0; i < binaryString.length(); i += 7) {
        std::string segment = binaryString.substr(i, 7);
        int decimalValue = stoi(segment, nullptr, 2);

        if (decimalValue <= 32) {
            decimalValue += 33;
        }
        if (decimalValue >= 127) {
            decimalValue = 126;
        }

        message += static_cast<char>(decimalValue);
    }

    return message;

    return message;
}


