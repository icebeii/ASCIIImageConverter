#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>

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
	image.channels = 3;
	image.pixels.resize(width * height);

	// Copy raw image data into the Image pixel vector
	int size = width * height;
	for (int i = 0; i < size; i++) {
		image.pixels[i].r = data[i * 3 + 0];
		image.pixels[i].g = data[i * 3 + 1];
		image.pixels[i].b = data[i * 3 + 2];
	}

	stbi_image_free(data);
	return image;
}