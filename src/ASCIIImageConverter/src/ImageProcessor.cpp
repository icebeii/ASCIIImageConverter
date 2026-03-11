#include "ImageProcessor.h"

// Constructor 
ImageProcessor::ImageProcessor(Image& img) : image(img){}

// Returns a reference to the image
Image& ImageProcessor::getImage() {
	return image;
}