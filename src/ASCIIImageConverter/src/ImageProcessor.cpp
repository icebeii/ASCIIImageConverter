#include "ImageProcessor.h"
#include <float.h>

constexpr int MAX_COLOR = 255;
constexpr float MAX_COLOR_FLOAT = 255.0f;
constexpr float MID_COLOR = 128.0f;
constexpr double RED_WEIGHT = 0.3;
constexpr double GREEN_WEIGHT = 0.59;
constexpr double BLUE_WEIGHT = 0.11;
constexpr float DEFAULT_BRIGHTNESS = 0.0f;
constexpr float DEFAULT_CONTRAST = 1.0f;
constexpr int KERNEL_RADIUS = 1;
constexpr float PI = 3.14159265f;
constexpr float RAD_TO_DEG = 180.0f / PI;
constexpr float CHANNEL_COUNT = 3.0f;
constexpr int HISTOGRAM_BINS = 256;
constexpr float LOW_THRESHOLD_RATIO = 0.5f;
constexpr int NEIGHBOR_RADIUS = 1;

// Constructor 
ImageProcessor::ImageProcessor(const IImage& img) {
	image = img.clone();
}

std::unique_ptr<IImage> ImageProcessor::processArgs(const Arguments& args) {
	if (args.width > 0) {
		image = resize(*image, args.width);
	}
	if (args.brightness != DEFAULT_BRIGHTNESS) {
		image = adjustBrightness(*image, args.brightness);
	}
	if (args.contrast != DEFAULT_CONTRAST) {
		image = adjustContrast(*image, args.contrast);
	}

	for (const std::string& filter : args.filters) {
		if (filter == "invert") {
			image = invert(*image);
		}
		else if (filter == "sepia") {
			image = sepia(*image);
		}
		else if (filter == "blur") {
			image = blur(*image);
		}
		else if (filter == "sharpen") {
			image = sharpen(*image);
		}
		else if (filter == "edges") {
			image = detectEdges(*image);
		}
		else if (filter == "grayscale") {
			image = grayscale(*image);
		}
	}
	return std::move(image);
}

std::unique_ptr<IImage> ImageProcessor::processInteractive(const InteractiveCommand& cmd) {
	if (cmd.name == "width") {
		int value = std::stoi(cmd.value);
		image = resize(*image, value);
	}
	else if (cmd.name == "brightness") {
		float value = std::stof(cmd.value);
		image = adjustBrightness(*image, value);
	}
	else if (cmd.name == "contrast") {
		float value = std::stof(cmd.value);
		image = adjustContrast(*image, value);
	}
	else if (cmd.name == "filter") {
		if (cmd.value == "invert") {
			image = invert(*image);
		}
		else if (cmd.value == "sepia") {
			image = sepia(*image);
		}
		else if (cmd.value == "blur") {
			image = blur(*image);
		}
		else if (cmd.value == "sharpen") {
			image = sharpen(*image);
		}
		else if (cmd.value == "edges") {
			image = detectEdges(*image);
		}
		else if (cmd.value == "grayscale") {
			image = grayscale(*image);
		}
	}
	return std::move(image);
}

std::unique_ptr<IImage> ImageProcessor::grayscale(const IImage& img)
{
	auto gray = img.clone();
	int width = img.getWidth();
	int height = img.getHeight();
	const auto& src = img.getPixels();
	auto& tgt = gray->getPixels();

	for (int i = 0; i < width * height; i++) {
		RGB p = src[i];
		int g = static_cast<int>(RED_WEIGHT * p.r + GREEN_WEIGHT * p.g + BLUE_WEIGHT * p.b);
		tgt[i] = { (unsigned char)g, (unsigned char)g, (unsigned char)g };
		}
	return gray;
}

int ImageProcessor::calculateSum(const std::vector<std::vector<int>>& kernel)
{
	int sum = 0;

	for (const auto& row : kernel) {
		for (int value : row) {
			sum += value;
		}
	}
	return sum;
}

// Applies a convolution kernel to the image
// Each pixel is replaced by a weighted sum of its neighbors defined by kernel
std::unique_ptr<IImage> ImageProcessor::applyKernel(const IImage& img, const std::vector<std::vector<int>>& kernel)
{
	auto result = img.clone();
	int divisor = calculateSum(kernel);
	int width = img.getWidth();
	int height = img.getHeight();
	const auto& src = img.getPixels();
	auto& tgt = result->getPixels();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int sumR = 0;
			int sumG = 0;
			int sumB = 0;

			for (int dy = KERNEL_RADIUS * -1; dy < KERNEL_RADIUS; dy++) {
				for (int dx = KERNEL_RADIUS * -1; dx < KERNEL_RADIUS; dx++) {
					int nx = x + dx;
					int ny = y + dy;

					// Stay inside the image boundaries
					nx = std::max(0, std::min(nx, width - 1));
					ny = std::max(0, std::min(ny, height - 1));

					int weight = kernel[dy+1][dx+1];
					const RGB& p = src[ny * width + nx];

					sumR += p.r * weight;
					sumG += p.g * weight;
					sumB += p.b * weight;
				}
			}

			RGB& np = tgt[y * width + x];

			if (divisor != 0) {
				sumR /= divisor;
				sumG /= divisor;
				sumB /= divisor;
			}
			

			np.r = std::min(MAX_COLOR, std::max(0, sumR));
			np.g = std::min(MAX_COLOR, std::max(0, sumG));
			np.b = std::min(MAX_COLOR, std::max(0, sumB));
		}
	}
	return result;
}

// Computes gradient magnitude and direction
// Converts image to grayscale and computes intensity changes
void ImageProcessor::computeGradient(const IImage& img, std::vector<float>& magnitude, std::vector<float>& direction)
{
	int width = img.getWidth();
	int height = img.getHeight();
	const auto& pixels = img.getPixels();
	magnitude.resize(width * height);
	direction.resize(width * height);

	// Sobel kernels for horizontal and vertical gradients
	int Gx[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	int Gy[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}
	};

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			float sumX = 0; 
			float sumY = 0;

			for (int dy = KERNEL_RADIUS * -1; dy <= KERNEL_RADIUS; dy++) {
				for (int dx = KERNEL_RADIUS * -1; dx <= KERNEL_RADIUS; dx++) {
					const RGB& p = pixels[(y + dy) * width + (x + dx)];
					float gray = (p.r + p.g + p.b) / CHANNEL_COUNT;
					sumX += gray * Gx[dy + 1][dx + 1];
					sumY += gray * Gy[dy + 1][dx + 1];
				}
			}
			int idx = y * width + x;
			magnitude[idx] = std::sqrt(sumX * sumX + sumY * sumY);
			direction[idx] = std::atan2(sumY, sumX);
		}
	}
}

// Non-maximum suppression
// Keeps only pixels that are local maxima along the gradient directon
std::vector<float> ImageProcessor::suppress(const std::vector<float>& magnitude, const std::vector<float>& direction, int width, int height)
{
	std::vector<float> result(width * height, 0);

	for (int y = 1; y < height - 1; y++) {
		for (int x = 1; x < width - 1; x++) {
			int i = y * width + x;

			float angle = direction[i] * RAD_TO_DEG;
			if (angle < 0) {
				angle += 180;
			}
			float m = magnitude[i];
			float m1 = 0;
			float m2 = 0;

			// Select neighboring pixels based on gradient dircetion
			if ((angle >= 0 && angle <= 22.5) || (angle >= 157.5 && angle <= 180)) {
				m1 = magnitude[i - 1];
				m2 = magnitude[i + 1];
			}
			else if (angle >= 22.5 && angle < 67.5) {
				m1 = magnitude[(y - 1) * width + (x + 1)];
				m2 = magnitude[(y + 1) * width + (x - 1)];
			}
			else if (angle >= 67.5 && angle < 112.5) {
				m1 = magnitude[(y - 1) * width + x];
				m2 = magnitude[(y + 1) * width + x];
			}
			else if (angle >= 112.5 && angle < 157.5) {
				m1 = magnitude[(y - 1) * width + (x - 1)];
				m2 = magnitude[(y + 1) * width + (x + 1)];
			}

			if (m >= m1 && m >= m2) {
				result[i] = m;
			}
			else {
				result[i] = 0;
			}
		}
	}
	return result;
}

/// Builds histogram
std::vector<int> ImageProcessor::buildHistogram(const std::vector<float>& data, int bins, float& minValue, float& maxValue) {
	std::vector<int> histogram(bins, 0);
	minValue = FLT_MAX;
	maxValue = FLT_MIN;
	for (float v : data) {
		if (v > 0.0f) {
			minValue = std::min(minValue, v);
			maxValue = std::max(maxValue, v);
		}
	}

	if (maxValue == minValue) {
		return histogram;
	}
	for (float v : data) {
		if (v > 0.0f) {
			int bin = (int)((v - minValue) / (maxValue - minValue) * (bins - 1));
			histogram[bin]++;
		}
	}
	return histogram;
}

/// Otsu method 
int ImageProcessor::otsu(const std::vector<int>& histogram) {
	int top = histogram.size();
	int total = 0;
	for (int h : histogram) {
		total += h;
	}

	float sumB = 0.0f;
	int wB = 0;
	int level = 0;
	float maximum = 0.0f;
	float sum1 = 0.0f;
	for (int i = 0; i < top; i++) {
		sum1 += i * histogram[i];
	}

	for (int j = 0; j < top; j++) {
		wB += histogram[j];
		int wF = total - wB;
		sumB += j * histogram[j];

		if (wB > 0 && wF > 0) {
			float mB = sumB / wB;
			float mF = (sum1 - sumB) / wF;
			float value = (float)wB * wF * (mB - mF) * (mB - mF);

			if (value >= maximum) {
				level = j;
				maximum = value;
			}
		}
	}
	return level;
}

/// Find optimal threshold
float ImageProcessor::calculateThreshold(const std::vector<float>& magnitude) {
	float minValue;
	float maxValue;
	int bins = HISTOGRAM_BINS;
	std::vector<int> histogram = buildHistogram(magnitude, bins, minValue, maxValue);
	int level = otsu(histogram);
	float threshold = minValue + (float(level) / (bins - 1)) * (maxValue - minValue);
	return threshold;
}

// Hysteresis thresholding
// Weak edges are kept only if connected to strong ones
std::unique_ptr<IImage> ImageProcessor::hysteresis(const IImage& img, const std::vector<float>& magnitude)
{
	int width = img.getWidth();
	int height = img.getHeight();
	auto result = img.create(width, height);
	auto& tgt = result->getPixels();

	// Threshold values
	float high = calculateThreshold(magnitude);
	float low = high * LOW_THRESHOLD_RATIO;

	std::vector<int> state(width * height, 0); // 0 - non-edge, 1 - weak, 2 - strong

	for (int i = 0; i < width * height; i++) {
		if (magnitude[i] >= high) {
			state[i] = 2;
		}
		else if (magnitude[i] >= low) {
			state[i] = 1;
		}
	}

	std::vector<int> stack;
	for (int i = 0; i < width * height; i++) {
		if (state[i] == 2) {
			stack.push_back(i);

			// DFS to connect weak edges
			while (!stack.empty()) {
				int curr = stack.back();
				stack.pop_back();
				int x = curr % width;
				int y = curr / width;

				for (int dy = NEIGHBOR_RADIUS * -1; dy <= NEIGHBOR_RADIUS; dy++) {
					for (int dx = NEIGHBOR_RADIUS * -1; dx <= NEIGHBOR_RADIUS; dx++) {
						int nx = x + dx;
						int ny = y + dy;

						if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
							int ni = ny * width + nx;
							if (state[ni] == 1) {
								state[ni] = 2;
								stack.push_back(ni);
							}
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < width * height; i++) {
		if (state[i] == 2) {
			tgt[i] = { MAX_COLOR, MAX_COLOR, MAX_COLOR };
		}
		else {
			tgt[i] = {0, 0, 0};
		}
	}
	return result;
}

// Resizes an image using bilinear interpolation
std::unique_ptr<IImage> ImageProcessor::resize(const IImage& img, int newWidth)
{
	int width = img.getWidth();
	int height = img.getHeight();
	const auto& src = img.getPixels();

	float factor = static_cast<float>(height) / width; // aspect ratio factor
	int newHeight = static_cast<int>(newWidth * factor);

	auto result = img.create(newWidth, newHeight);
	auto& tgt = result->getPixels();

	for (int y = 0; y < newHeight; y++) {
		for (int x = 0; x < newWidth; x++) {
			float srcX = x * (float)width / newWidth;
			float srcY = y * (float)height / newHeight;

			// Coordinates of surrounding pixels in the original image
			int x0 = int(srcX);
			int y0 = int(srcY);
			int x1 = std::min(x0 + 1, width - 1);
			int y1 = std::min(y0 + 1, height - 1);

			// Neighbouring pixels
			const RGB& p00 = src[y0 * width + x0];
			const RGB& p01 = src[y0 * width + x1];
			const RGB& p10 = src[y1 * width + x0];
			const RGB& p11 = src[y1 * width + x1];

			// Interpolation weights
			float dx = srcX - x0;
			float dy = srcY - y0;
			RGB newP;

			// Bilinear interpolation for each channel
			newP.r = p00.r * (1 - dx) * (1 - dy) +
				p10.r * dx * (1 - dy) +
				p01.r * (1 - dx) * dy +
				p11.r * dx * dy;

			newP.g = p00.g * (1 - dx) * (1 - dy) +
				p10.g * dx * (1 - dy) +
				p01.g * (1 - dx) * dy +
				p11.g * dx * dy;

			newP.b = p00.b * (1 - dx) * (1 - dy) +
				p10.b * dx * (1 - dy) +
				p01.b * (1 - dx) * dy +
				p11.b * dx * dy;

			tgt[y * newWidth + x] = newP;
		}
	}
	return result;
} 

// Creates a negative version of an image by inverting each color channel
std::unique_ptr<IImage> ImageProcessor::invert(const IImage& img)
{
	auto result = img.clone();
	int width = img.getWidth();
	int height = img.getHeight();
	auto& tgt = result->getPixels();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++){
			RGB& p = tgt.at(y * width + x);
			p.r = MAX_COLOR - p.r;
			p.g = MAX_COLOR - p.g;
			p.b = MAX_COLOR - p.b;
		}
	}
	return result;
}

// Applies a sepia filter to an image
std::unique_ptr<IImage> ImageProcessor::sepia(const IImage& img)
{
	auto result = img.clone();
	int width = img.getWidth();
	int height = img.getHeight();
	auto& tgt = result->getPixels();

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			RGB& p = tgt.at(y * width + x);
			int newR = static_cast<int>(p.r * 0.393 + p.g * 0.769 + p.b * 0.189);
			int newG = static_cast<int>(p.r * 0.349 + p.g * 0.686 + p.b * 0.168);
			int newB = static_cast<int>(p.r * 0.272 + p.g * 0.534 + p.b * 0.131);

			if (newR > MAX_COLOR) {
				newR = MAX_COLOR;
			}
			if (newG > MAX_COLOR) {
				newG = MAX_COLOR;
			}
			if (newB > MAX_COLOR) {
				newB = MAX_COLOR;
			}

			p.r = newR;
			p.g = newG;
			p.b = newB;
		}
	}
	return result;
}

// Applies a Gaussian blur
std::unique_ptr<IImage>ImageProcessor::blur(const IImage& img) {
	// 3x3 Gaussian kernel
	std::vector<std::vector<int>> kernel = {
		{1, 2, 1},		
		{2, 4, 2},
		{1, 2, 1}
	};

	return applyKernel(std::move(img), kernel);
}

// Applies a sharpening effect
std::unique_ptr<IImage> ImageProcessor::sharpen(const IImage& img)
{
	// 3x3 sharpening kernel
	std::vector<std::vector<int>> kernel = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};

	return applyKernel(std::move(img), kernel);
}

// Adjusts brightness
std::unique_ptr<IImage> ImageProcessor::adjustBrightness(const IImage& img, float factor) {
	auto result = img.clone();
	int width = img.getWidth();
	int height = img.getHeight();
	auto& tgt = result->getPixels();

	for (int i = 0; i < width * height; i++) {
		RGB& p = tgt[i];
		p.r = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, p.r * (1.0f + factor))));
		p.g = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, p.g * (1.0f + factor))));
		p.b = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, p.b * (1.0f + factor))));
	}
	return result;
}

// Adjusts contrast
std::unique_ptr<IImage> ImageProcessor::adjustContrast(const IImage& img, float factor)
{
	auto result = img.clone();
	int width = img.getWidth();
	int height = img.getHeight();
	auto& tgt = result->getPixels();

	for (int i = 0; i < width * height; i++) {
		RGB& p = tgt[i];
		p.r = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, (p.r - MID_COLOR) * factor + MID_COLOR)));
		p.g = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, (p.g - MID_COLOR) * factor + MID_COLOR)));
		p.b = static_cast<int>(std::min(MAX_COLOR_FLOAT, std::max(0.0f, (p.b - MID_COLOR) * factor + MID_COLOR)));
	}
	return result;
}

// Full Canny edge detection pipeline
std::unique_ptr<IImage> ImageProcessor::detectEdges(const IImage& img)
{
	auto blurred = blur(img);
	std::vector<float> magnitude;
	std::vector<float> direction;
	int width = img.getWidth();
	int height = img.getHeight();

	computeGradient(*blurred, magnitude, direction);
	std::vector<float> suppressed = suppress(magnitude, direction, width, height);
	return hysteresis(*blurred, suppressed);
}
