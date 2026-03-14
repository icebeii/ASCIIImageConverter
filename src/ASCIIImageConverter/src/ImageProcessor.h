#pragma once

#include "Image.h"
#include "ArgsParser.h"
#include <string>

// Class responsible for processing and modifiying an image
class ImageProcessor {
public:
	// Constructor that takes a reference to an Image object
	// The processor will operate directly on this image
	explicit ImageProcessor(Image& img);

	void processArgs(const Arguments& args);

	// Resize an image to the specified width, height is computed automatically
	//	img - input image
	//	newWidth - new width in pixels
	// Returns a resized image
	Image resize(const Image& img, int newWidth);

	// Inverts all RGB color channels
	//	img - input image
	// Returns a negative image
	Image invert(const Image& img);

	// Applies a sepia tone effect using a standart sepia color transform
	//	img - input image
	// Returns an image with sepia color filter
	Image sepia(const Image& img);

	// Returns a reference to the processed image
	Image& getImage();

private:
	Image& image;	// Reference to the image

	Image grayscale(const Image& img);
	
};