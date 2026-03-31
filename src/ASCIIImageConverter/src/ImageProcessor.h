#pragma once

#include "Image.h"
#include "ArgsParser.h"
#include "Arguments.h"
#include <string>
#include <cmath>

/// Class responsible for processing and modifiying an image
class ImageProcessor {
public:
	/**
	* Constructor that takes a reference to an Image object
	* The processor will operate directly on this image
	*/
	explicit ImageProcessor(const Image& img);

	Image processArgs(const Arguments& args);

	Image processInteractive(const InteractiveCommand& cmd);

	/**
	* Resizes an image to the specified width, height is computed automatically
	* 
	* @param img - input image
	* @param newWidth - new width in pixels
	* @retutn a resized image
	*/
	Image resize(const Image& img, int newWidth);

	/**
	* Inverts all RGB color channels
	* 
	* @param img - input image
	* @retutn a negative image
	*/
	Image invert(const Image& img);

	/**
	* Applies a sepia tone effect using a standart sepia color transform
	* 
	* @param img - input image
	* @return an image with sepia color filter
	*/
	Image sepia(const Image& img);

	/**
	* Applies a Gaussian blur effect to the image
	* 
	* @param img - input image
	* @return a blurred image
	*/
	Image blur(const Image& img);

	/**
	* Applies a sharpening effect to the image
	* 
	* @param img - input image
	* @return a sharpened image
	*/
	Image sharpen(const Image& img);

	/**
	* Adjusts image brightness
	* 
	* @param img - input image
	* @param factor > 0 to increase, factor < 0 to decrease
	* @return corrected image
	*/
	Image adjustBrightness(const Image& img, float factor);

	/**
	* Adjusts image contrast
	* 
	* @param img - input image
	* @param factor > 1 to increase, 0 < factor < 1 to decrease
	* @return corrected image
	*/
	Image adjustContrast(const Image& img, float factor);

	/**
	* Detects edges using Canny algorithm
	* 
	* @param img - input image
	* @return black image with white edges 
	*/
	Image detectEdges(const Image& img);

private:
	Image image;	

	Image grayscale(const Image& img);

	/**
	* Calculates sum of kernel elements
	* 
	* @param kernel - input matrix
	* @return sum of kernel elements
	*/
	int calculateSum(const std::vector<std::vector<int>>& kernel);

	/**
	* Applies a convolution kernel to the image
	* 
	* @param img - input image
	* @param kernel - 2D vector which represents the filter matrix
	* @return an iamge after applying the kernel
	*/
	Image applyKernel(const Image& img, const std::vector<std::vector<int>>& kernel);

	/**
	* Computes gradient magnitude and direction using Sobel operators
	* 
	* @param img - input image
	* @param magnitude - output vectors of gradient strengths
	* @param direction - output vector of gradient angles in radians
	*/
	void computeGradient(const Image& img, std::vector<float>& magnitude, std::vector<float>& direction);

	/**
	* Performs non-maximum suppression
	* 
	* @param magnitude - gradient magnitudes
	* @param direction - gradient directions
	* @param width, height - image dimensions
	* @return  a vector with suppressed edge magnitudes
	*/
	std::vector<float> suppress(const std::vector<float>& magnitude, const std::vector<float>& direction, int width, int height);

	std::vector<int> buildHistogram(const std::vector<float>& data, int bins, float& minValue, float& maxValue);
	float otsu(const std::vector<int>& histogram);
	float calculateThreshold(const std::vector<float>& magnitude);

	/**
	* Applies hysteresis thresholding to detect final edges
	* 
	* @param magnitude - suppressed gradient magnitudes
	* @param width, height - image dimensions
	* @return a binary image with detected edges
	*/
	Image hysteresis(const std::vector<float>& magnitude, int width, int height);
};