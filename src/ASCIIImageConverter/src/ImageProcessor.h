#pragma once

#include "Image.h"
#include <string>

// Class responsible for processing and modifiying an image
class ImageProcessor {
public:
	// Constructor that takes a reference to an Image object
	// The processor will operate directly on this image
	explicit ImageProcessor(Image& img);

	// Returns a reference to the processed image
	Image& getImage();

private:
	Image& image;	// Reference to the image
};