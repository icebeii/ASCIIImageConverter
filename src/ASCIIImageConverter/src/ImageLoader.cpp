#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

constexpr int RGB_CHANNELS = 3;
constexpr int RED_IDX = 0;
constexpr int GREEN_IDX = 1;
constexpr int BLUE_IDX = 2;

// Loads an image from a file using stb_image
Image ImageLoader::loadFromFile(const std::string& filename) {
	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
	if (!data) {
		throw std::runtime_error("Cannot load image: " + filename);
	}

	Image image;
	image.width = width;
	image.height = height;
	image.channels = RGB_CHANNELS;
	image.pixels.resize(width * height);

	// Copy raw image data into the Image pixel vector
	int size = width * height;
	for (int i = 0; i < size; i++) {
		image.pixels[i].r = data[i * RGB_CHANNELS + RED_IDX];
		image.pixels[i].g = data[i * RGB_CHANNELS + GREEN_IDX];
		image.pixels[i].b = data[i * RGB_CHANNELS + BLUE_IDX];
	}

	stbi_image_free(data);
	return image;
}