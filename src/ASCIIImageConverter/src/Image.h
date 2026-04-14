#pragma once

#include <vector>
#include <memory>

constexpr int CHANNELS = 3;

/// Structure representing a single RGB pixel
struct RGB {
	unsigned char r;	/// Red component
	unsigned char g;	/// Green component
	unsigned char b;	/// Blue component 
};

/// Interface representing an abstract image
class IImage {
public:
	virtual ~IImage() = default;

	/// Returns image width in pixels
	virtual int getWidth() const = 0;

	/// Returns image height in pixels
	virtual int getHeight() const = 0;

	/// Returns constant reference to pixel vector
	virtual const std::vector<RGB>& getPixels() const = 0;

	/// Returns mutable reference to pixel vector
	virtual std::vector<RGB>& getPixels() = 0;

	/// Creates a deep copy of the image
	virtual std::unique_ptr<IImage> clone() const = 0;

	/// Creates a new empty image with given size
	virtual std::unique_ptr<IImage> create(int width, int height) const = 0;
};

/// Class representing an image
class Image : public IImage {
public:
	int width = 0;					/// Image width in pixels
	int height = 0;					/// Image height in pixels
	int channels = CHANNELS;		/// Number of color channels

	Image() = default;
	Image(const Image& other) : width(other.width), height(other.height),
								channels(other.channels), pixels(other.pixels)
	{}

	/// Vector storing all pixels of the image
	/// Contains current pixel data
	std::vector<RGB> pixels;	

	/// Implementation of interface
	int getWidth() const override { 
		return width; 
	}

	int getHeight() const override { 
		return height; 
	}

	const std::vector<RGB>& getPixels() const override {
		return pixels;
	}

	std::vector<RGB>& getPixels() override { 
		return pixels; 
	}

	std::unique_ptr<IImage> clone() const override {
		return std::make_unique<Image>(*this);
	}

	std::unique_ptr<IImage> Image::create(int width, int height) const {
		auto img = std::make_unique<Image>();
		img->width = width;
		img->height = height;
		img->channels = CHANNELS;
		img->pixels.resize(width * height);
		return img;
	}
};
