#pragma once

#include "Image.h"
#include "ArgsParser.h"
#include <string>
#include <vector>
#include <cmath>

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

	// Applies a Gaussian blur effect to the image
	//	img - input image
	// Returns a blurred image
	Image blur(const Image& img);

	// Applies a sharpening effect to the image
	//	img - input image
	// Returns a sharpened image
	Image sharpen(const Image& img);

	Image adjustBrightness(const Image& img, float factor);

	Image adjustContrast(const Image& img, float factor);

	// Canny
	Image detectEdges(const Image& img);

	// Returns a reference to the processed image
	Image& getImage();

private:
	Image& image;	// Reference to the image

	Image grayscale(const Image& img);

	// Applies a convolution kernel to the image
	//	img - input image
	//	kernel - 2D vector which represents the filter matrix
	//	divisor - sum of kernel elements 
	// Returns an iamge after applying the kernel
	Image applyKernel(const Image& img, const std::vector<std::vector<int>>& kernel, int divisor);

	void computeGradient(const Image& img, std::vector<float>& magnitude, std::vector<float>& direction);
	std::vector<float> suppress(const std::vector<float>& magnitude, const std::vector<float>& direction, int width, int height);
	Image doubleThreshold(const std::vector<float>& magnitude, int width, int height);
};