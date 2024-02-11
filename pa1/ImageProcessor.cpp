#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() = default;

ImageProcessor::~ImageProcessor() = default;


std::string ImageProcessor::decodeHiddenMessage(const ImageMatrix &img) {
    EdgeDetector edgeDetector;
    auto edges = edgeDetector.detectEdges(img);
    DecodeMessage decodeMessage;
    return decodeMessage.decodeFromImage(img, edges);
}

ImageMatrix ImageProcessor::encodeHiddenMessage(const ImageMatrix &img, const std::string &message) {

}
