#pragma once

#include "Image.h"
#include "ArgsParser.h"
#include "Arguments.h"
#include <string>
#include <cmath>
#include <memory>

/// Interface for image processing pipelines
class IImageProcessor {
public: 
	virtual ~IImageProcessor() = default;

	/**
	* Applies a full set of arguments to an image
	* 
	* @param args - processing parameters
	* @return new processed image 
	*/
	virtual std::unique_ptr<IImage> processArgs(const Arguments& args) = 0;

	/**
	* Applies a single interactive command
	* 
	* @param cmd - command to apply
	* @return new processed image 
	*/
	virtual std::unique_ptr<IImage> processInteractive(const InteractiveCommand& cmd) = 0;
};

/// Class responsible for processing and modifiying an image
class ImageProcessor : public IImageProcessor{
public:
	/**
	* Constructor 
	* 
	* @param img - source image which will be cloned
	*	Image copy is stored internaly and the original image is not modified
	*/
	explicit ImageProcessor(const IImage& img);

	/**
	* Processes Arguments and applies corresponging changes
	* 
	* @param args - input arguments
	* @return processed image
	*/
	std::unique_ptr<IImage> processArgs(const Arguments& args) override;

	/**
	* Processes an interactive command and changes the image
	* 
	* @param cmd - input command
	* @return processed image
	*/
	std::unique_ptr<IImage> processInteractive(const InteractiveCommand& cmd) override;

private:
	std::unique_ptr<IImage> image;

	/**
	* Transforms an image to grayscale
	* 
	* @param img - input image
	* @return a grayscale image
	*/
	std::unique_ptr<IImage> grayscale(const IImage& img);

	/**
	* Resizes an image to the specified width, height is computed automatically
	*
	* @param img - input image
	* @param newWidth - new width in pixels
	* @return a resized image
	*/
	std::unique_ptr<IImage> resize(const IImage& img, int newWidth);

	/**
	* Inverts all RGB color channels
	*
	* @param img - input image
	* @return a negative image
	*/
	std::unique_ptr<IImage> invert(const IImage& img);

	/**
	* Applies a sepia tone effect using a standart sepia color transform
	*
	* @param img - input image
	* @return an image with sepia color filter
	*/
	std::unique_ptr<IImage> sepia(const IImage& img);

	/**
	* Applies a Gaussian blur effect to the image
	*
	* @param img - input image
	* @return a blurred image
	*/
	std::unique_ptr<IImage> blur(const IImage& img);

	/**
	* Applies a sharpening effect to the image
	*
	* @param img - input image
	* @return a sharpened image
	*/
	std::unique_ptr<IImage> sharpen(const IImage& img);

	/**
	* Adjusts image brightness
	*
	* @param img - input image
	* @param factor > 0 to increase, factor < 0 to decrease
	* @return corrected image
	*/
	std::unique_ptr<IImage> adjustBrightness(const IImage& img, float factor);

	/**
	* Adjusts image contrast
	*
	* @param img - input image
	* @param factor > 1 to increase, 0 < factor < 1 to decrease
	* @return corrected image
	*/
	std::unique_ptr<IImage> adjustContrast(const IImage& img, float factor);

	/**
	* Detects edges using Canny algorithm
	*
	* @param img - input image
	* @return binary edge image
	*/
	std::unique_ptr<IImage> detectEdges(const IImage& img);

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
	std::unique_ptr<IImage> applyKernel(const IImage& img, const std::vector<std::vector<int>>& kernel);

	/**
	* Computes gradient magnitude and direction using Sobel operators
	* 
	* @param img - input image
	* @param magnitude - output vectors of gradient strengths
	* @param direction - output vector of gradient angles in radians
	*/
	void computeGradient(const IImage& img, std::vector<float>& magnitude, std::vector<float>& direction);

	/**
	* Performs non-maximum suppression
	* 
	* @param magnitude - gradient magnitudes
	* @param direction - gradient directions
	* @param width, height - image dimensions
	* @return  a vector with suppressed edge magnitudes
	*/
	std::vector<float> suppress(const std::vector<float>& magnitude, const std::vector<float>& direction, int width, int height);

	/**
	* Builds histogram representing the distribution of gradient magnitudes
	* 
	* @param data - input array of values
	* @param bins -	number of histogram bins
	* @param minValue, maxValue - output min and max values in data
	* @return histogram as vector of bin counts
	*/
	std::vector<int> buildHistogram(const std::vector<float>& data, int bins, float& minValue, float& maxValue);

	/**
	* Calculates optimal threshold
	* 
	* @param histogram - input histogram
	* @return bin index with optimal threshold level
	*/
	int otsu(const std::vector<int>& histogram);

	/**
	* Calculates threshold value for edge detection
	* 
	* @param magnitude - gradient magnitudes
	* @return threshold value
	*/
	float calculateThreshold(const std::vector<float>& magnitude);

	/**
	* Applies hysteresis thresholding to detect final edges
	* 
	* @param magnitude - suppressed gradient magnitudes
	* @param width, height - image dimensions
	* @return a binary image with detected edges
	*/
	std::unique_ptr<IImage> hysteresis(const IImage& img, const std::vector<float>& magnitude);
};