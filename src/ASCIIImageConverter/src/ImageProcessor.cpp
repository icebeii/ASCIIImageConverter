#include "ImageProcessor.h"

// Constructor 
ImageProcessor::ImageProcessor(Image& img) : image(img) {}

Image ImageProcessor::processArgs(const Arguments& args) {
	if (args.width > 0) {
		image = resize(image, args.width);
	}
	if (args.brightness != 0.0f) {
		image = adjustBrightness(image, args.brightness);
	}
	if (args.contrast != 1.0f) {
		image = adjustContrast(image, args.contrast);
	}

	for (const std::string& filter : args.filters) {
		if (filter == "invert") {
			image = invert(image);
		}
		else if (filter == "sepia") {
			image = sepia(image);
		}
		else if (filter == "blur") {
			image = blur(image);
		}
		else if (filter == "sharpen") {
			image = sharpen(image);
		}
		else if (filter == "edges") {
			image = detectEdges(image);
		}
	}
	return image;
}

// Returns a reference to the image
Image& ImageProcessor::getImage() {
	return image;
}

Image ImageProcessor::grayscale(const Image& img)
{
	Image gray = img;
	for (int y = 0; y < gray.height; y++) {
		for (int x = 0; x < gray.width; x++) {
			RGB& p = gray.pixels.at(y * gray.width + x);
			int g = static_cast<int>(0.3 * p.r + 0.59 * p.g + 0.11 * p.b);
			p.r = g;
			p.g = g;
			p.b = g;
		}
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
Image ImageProcessor::applyKernel(const Image& img, const std::vector<std::vector<int>>& kernel)
{
	Image result = img;
	int divisor = calculateSum(kernel);

	for (int y = 0; y < img.height; y++) {
		for (int x = 0; x < img.width; x++) {
			int sumR = 0;
			int sumG = 0;
			int sumB = 0;

			for (int dy = -1; dy < 1; dy++) {
				for (int dx = -1; dx < 1; dx++) {
					int nx = x + dx;
					int ny = y + dy;

					// Stay inside the image boundaries
					nx = std::max(0, std::min(nx, img.width - 1));
					ny = std::max(0, std::min(ny, img.height - 1));

					int weight = kernel[dy+1][dx+1];
					const RGB& p = img.pixels[ny * img.width + nx];

					sumR += p.r * weight;
					sumG += p.g * weight;
					sumB += p.b * weight;
				}
			}

			RGB& np = result.pixels[y * img.width + x];

			if (divisor != 0) {
				sumR /= divisor;
				sumG /= divisor;
				sumB /= divisor;
			}
			

			np.r = std::min(255, std::max(0, sumR));
			np.g = std::min(255, std::max(0, sumG));
			np.b = std::min(255, std::max(0, sumB));
		}
	}
	return result;
}

// Computes gradient magnitude and direction
// Converts image to grayscale and computes intensity changes
void ImageProcessor::computeGradient(const Image& img, std::vector<float>& magnitude, std::vector<float>& direction)
{
	int width = img.width;
	int height = img.height;
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

			for (int dy = -1; dy <= 1; dy++) {
				for (int dx = -1; dx <= 1; dx++) {
					const RGB& p = img.pixels[(y + dy) * width + (x + dx)];
					float gray = (p.r + p.g + p.b) / 3.0f;
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

			float angle = direction[i] * 180.0f / 3.14159265f;
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

// Hysteresis thresholding
// Weak edges are kept only if connected to strong ones
Image ImageProcessor::hysteresis(const std::vector<float>& magnitude, int width, int height)
{
	Image result;
	result.width = width;
	result.height = height;
	result.pixels.resize(width * height);

	// Threshold values
	float low = 30.0f;
	float high = 100.0f;

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

				for (int dy = -1; dy <= 1; dy++) {
					for (int dx = -1; dx <= 1; dx++) {
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
			result.pixels[i] = { 255, 255, 255 };
		}
		else {
			result.pixels[i] = {0, 0, 0};
		}
	}
	return result;
}

// Resizes an image using bilinear interpolation
Image ImageProcessor::resize(const Image& img, int newWidth)
{
	float factor = static_cast<float>(img.height) / img.width; // aspect ratio factor
	int newHeight = static_cast<int>(newWidth * factor);

	Image result;
	result.width = newWidth;
	result.height = newHeight;
	result.pixels.resize(newWidth * newHeight);

	for (int y = 0; y < newHeight; y++) {
		for (int x = 0; x < newWidth; x++) {
			float srcX = x * (float)img.width / newWidth;
			float srcY = y * (float)img.height / newHeight;

			// Coordinates of surrounding pixels in the original image
			int x0 = int(srcX);
			int y0 = int(srcY);
			int x1 = std::min(x0 + 1, img.width - 1);
			int y1 = std::min(y0 + 1, img.height - 1);

			// Neighbouring pixels
			const RGB& p00 = img.pixels[y0 * img.width + x0];
			const RGB& p01 = img.pixels[y0 * img.width + x1];
			const RGB& p10 = img.pixels[y1 * img.width + x0];
			const RGB& p11 = img.pixels[y1 * img.width + x1];

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

			result.pixels[y * newWidth + x] = newP;
		}
	}
	return result;
}

// Creates a negative version of an image by inverting each color channel
Image ImageProcessor::invert(const Image& img)
{
	Image result = img;
	for (int y = 0; y < img.height; y++) {
		for (int x = 0; x < img.width; x++){
			RGB& p = result.pixels.at(y * result.width + x);
			p.r = 255 - p.r;
			p.g = 255 - p.g;
			p.b = 255 - p.b;
		}
	}
	return result;
}

// Applies a sepia filter to an image
Image ImageProcessor::sepia(const Image& img)
{
	Image result = img;
	for (int y = 0; y < img.height; y++) {
		for (int x = 0; x < img.width; x++) {
			RGB& p = result.pixels.at(y * result.width + x);
			int newR = static_cast<int>(p.r * 0.393 + p.g * 0.769 + p.b * 0.189);
			int newG = static_cast<int>(p.r * 0.349 + p.g * 0.686 + p.b * 0.168);
			int newB = static_cast<int>(p.r * 0.272 + p.g * 0.534 + p.b * 0.131);

			if (newR > 255) {
				newR = 255;
			}
			if (newG > 255) {
				newG = 255;
			}
			if (newB > 255) {
				newB = 255;
			}

			p.r = newR;
			p.g = newG;
			p.b = newB;
		}
	}
	return result;
}

// Applies a Gaussian blur
Image ImageProcessor::blur(const Image& img) {
	// 3x3 Gaussian kernel
	std::vector<std::vector<int>> kernel = {
		{1, 2, 1},		
		{2, 4, 2},
		{1, 2, 1}
	};

	return applyKernel(img, kernel);
}

// Applies a sharpening effect
Image ImageProcessor::sharpen(const Image& img)
{
	// 3x3 sharpening kernel
	std::vector<std::vector<int>> kernel = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};

	return applyKernel(img, kernel);
}

// Adjusts brightness
Image ImageProcessor::adjustBrightness(const Image& img, float factor) {
	Image result = img;

	for (int i = 0; i < img.width * img.height; i++) {
		RGB& p = result.pixels[i];
		p.r = static_cast<int>(std::min(255.0f, std::max(0.0f, p.r * (1.0f + factor))));
		p.g = static_cast<int>(std::min(255.0f, std::max(0.0f, p.g * (1.0f + factor))));
		p.b = static_cast<int>(std::min(255.0f, std::max(0.0f, p.b * (1.0f + factor))));
	}
	return result;
}

// Adjusts contrast
Image ImageProcessor::adjustContrast(const Image& img, float factor)
{
	Image result = img;

	for (int i = 0; i < img.width * img.height; i++) {
		RGB& p = result.pixels[i];
		p.r = static_cast<int>(std::min(255.0f, std::max(0.0f, (p.r - 128.0f) * factor + 128.0f)));
		p.g = static_cast<int>(std::min(255.0f, std::max(0.0f, (p.g - 128.0f) * factor + 128.0f)));
		p.b = static_cast<int>(std::min(255.0f, std::max(0.0f, (p.b - 128.0f) * factor + 128.0f)));
	}
	return result;
}

// Full Canny edge detection pipeline
Image ImageProcessor::detectEdges(const Image& img)
{
	Image blurred = blur(img);
	std::vector<float> magnitude;
	std::vector<float> direction;

	computeGradient(blurred, magnitude, direction);
	std::vector<float> suppressed = suppress(magnitude, direction, img.width, img.height);
	return hysteresis(suppressed, img.width, img.height);
}
