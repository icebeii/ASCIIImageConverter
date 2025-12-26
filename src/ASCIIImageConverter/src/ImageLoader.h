#pragma once

# include "Image.h"
# include <string>

// Class responsible for loading images from files
class ImageLoader {
public:
	// Loads an image from file
	//	filename - path to an image file
	// Returns an Image object
	static Image loadFromFile(const std::string& filename)
};