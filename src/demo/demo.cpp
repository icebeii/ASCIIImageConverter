#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main()
{
    std::string filename = "example.jpg";
    std::string path = std::string(DATA_DIR) + "/" + filename;
	int width, height, channels;
	unsigned char* img = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!img) {
        std::cerr << "Error: failed to load image: " << filename << std::endl;
        return 1;
    }

    std::cout << "Image uploaded" << std::endl;
    std::cout << "File: " << filename << std::endl;
    std::cout << "Size: " << width << " x " << height << std::endl;
    std::cout << "Number of channels: " << channels << std::endl;

    stbi_image_free(img);

    return 0;
}
