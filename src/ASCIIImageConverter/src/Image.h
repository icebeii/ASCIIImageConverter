#pragma once

# include <vector>

/// Structure representing a single RGB pixel
struct RGB {
	unsigned char r;	/// Red component
	unsigned char g;	/// Green component
	unsigned char b;	/// Blue component 
};

/// Class representing an image
class Image {
public:
	int width = 0;				/// Image width in pixels
	int height = 0;				/// Image height in pixels
	int channels = 3;			/// Number of color channels

	Image() = default;
	Image(const Image& other) : width(other.width), height(other.height),
								channels(other.channels), pixels(other.pixels)
	{}

	/// Vector storing all pixels of the image
	/// Contains current pixel data
	std::vector<RGB> pixels;	
};
