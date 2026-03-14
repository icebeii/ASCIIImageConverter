#pragma once

#include "Image.h"
#include <string>
#include <ostream>
#include <unordered_map>

// Class responsible for converting an image into ASCII representation
class ASCIIConverter {
public:
	// Constructor that takes a reference to an Image object
	// The image will be used as the source for ASCII conversion
	explicit ASCIIConverter(const Image& img);

	// Converts an image to an ASCII string using the specified character set
	//	charsetName - name of the character set used for conversion
	//  color - if true, ANSI color codes will be added 
	// Returns a string containing an ASCII representation of the image
	std::string convert(const std::string& charsetName, bool color) const;

	// Prints the ASCII result to the provided output stream
	//	result - ASCII string produced by convert()
	//	out - output stream (std::cout or file stream)
	void print(const std::string& result, std::ostream& out) const;

private:
	const Image& image;	// Reference to the source image

	// Unordered map containing predefined character sets for ASCII conversion
	//	key - charset name
	//	value - string of characters ordered by brightness
	static const std::unordered_map<std::string, std::string> charsets;

	// Returns the character set corresponding to the given name
	// If the name is not found throws an error
	static const std::string getCharset(const std::string& title);
};