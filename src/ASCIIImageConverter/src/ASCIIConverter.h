#pragma once

#include "Image.h"
#include <string>
#include <ostream>
#include <fstream>
#include <iostream>
#include <unordered_map>

/// Class responsible for converting an image into ASCII representation
class ASCIIConverter {
public:
	/** 
	* Constructor that takes a reference to an Image object
	* The image will be used as the source for ASCII conversion
	*/
	explicit ASCIIConverter(const Image& img);

	/**
	* Converts an image to an ASCII string using the specified character set
	* 
	* @param charsetName - name of the character set used for conversion
	* @param color - if true, ANSI color codes will be added 
	* @return a string containing an ASCII representation of the image
	*/
	std::string convert(const std::string& charsetName, bool color) const;

	/**
	* Get the output stream depending on the argument
	* 
	* @param filename - empty if standart output should be used,
	*					otherwise is filename
	* @returns output stream
	* 
	* @throw  std::runtime_error if output file cannot be opened
	*/
	std::ostream& getOutputStream(const std::string& filename);

	/**
	* Prints the ASCII result to the provided output stream
	* 
	* @param result - ASCII string produced by convert()
	* @param out - output stream (std::cout or file stream)
	*/
	void print(const std::string& result, std::ostream& out) const;

private:
	/// Reference to the source image
	const Image& image;

	/**
	* Unordered map containing predefined character sets for ASCII conversion
	* key - title of the character set
	* value - string of characters ordered by brightness
	*/
	static const std::unordered_map<std::string, std::string> charsets;

	/**
	* Find the character set by its title
	* 
	* @param title - string with a name of the character set
	* @return found string containing the character set
	* 
	* @throw std::runtime_error if the character set is not found
	*/
	static const std::string getCharset(const std::string& title);
};