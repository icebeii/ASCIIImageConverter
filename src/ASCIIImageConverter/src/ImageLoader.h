#pragma once

# include "Image.h"
# include <string>

/// Class responsible for loading images from files
class ImageLoader {
public:
	/**
	* Loads an image from file
	* 
	* @param filename - path to an image file
	* @return an Image object containing uploaded image
	* 
	* @throws std::runtime_error if input file cannot be open
	*/
	static Image loadFromFile(const std::string& filename);
};