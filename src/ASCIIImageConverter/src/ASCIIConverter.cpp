#include "ASCIIConverter.h"

// Predefined character sets used for ASCII conversion
// Characters are ordered from darkest to lightest
const std::unordered_map<std::string, std::string> ASCIIConverter::charsets = {
	{"default", "@%#*+=-:. "},
    {"detailed", "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\" ^ `'."},
    {"alpha", "ABCDEFGHIJKLMNOPQRSTUVWXYZ"},
    {"num", "0896532471"},
    {"minimalist", "#+-."},
    {"alnum", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0896532471abcderghijklmnopqrstuvwxyz"}
};

// Constructor
ASCIIConverter::ASCIIConverter(const Image& img) : image(img){}

// Converts an image to ASCII string, adding ANSI codes if color is true
std::string ASCIIConverter::convert(const std::string& charsetName, bool color) const {
    // Size of one block of pixels that will be reprsesented by one ASCII character
	const int blockWidth = 4;
	const int blockHeight = 8;
	std::string result;

    std::string charset = getCharset(charsetName);

    // Iterate over an image in blocks
	for (int y = 0; y < image.height; y += blockHeight) {
		for (int x = 0; x < image.width; x += blockWidth) {

            int sumGray = 0;
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;

            int count = 0;
            // Iterate through pixels inside the current block
            for (int dy = 0; dy < blockHeight; dy++)
            {
                for (int dx = 0; dx < blockWidth; dx++)
                {
                    int px = x + dx;
                    int py = y + dy;
                    
                    if (px >= image.width || py >= image.height) {
                        continue;
                    }
                    const RGB& p = image.pixels[py * image.width + px];
                    int gray = 0.3 * p.r + 0.59 * p.g + 0.11 * p.b; // Luminosity method
                    sumGray += gray;
                    sumR += p.r;
                    sumG += p.g;
                    sumB += p.b;
                    count++;
                }
            }
            // Average brightness and color for block
            int avgGray = sumGray / count;
            int avgR = sumR / count;
            int avgG = sumG / count;
            int avgB = sumB / count;

            int index = avgGray * (charset.size() - 1) / 255;
            char asciiChar = charset[index];

            if (color) {
                result += "\033[38;2;";
                result += std::to_string(avgR) + ";";
                result += std::to_string(avgG) + ";";
                result += std::to_string(avgB) + "m";
                result += asciiChar;
                result += "\033[0m";
            }
            else {
                result += asciiChar;
            }
		}
        result += '\n';
	}
	return result;
}

std::ostream& ASCIIConverter::getOutputStream(const std::string& filename)
{
    static std::ofstream file;
    if (filename.empty()) {
        return std::cout;
    }
    if (file.is_open()) {
        file.close();
    }

    file.open(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    return file;
}

// Prints the ASCII result to the given output stream
void ASCIIConverter::print(const std::string& result, std::ostream& out) const {
    out << result;
}

// Returns the character set for the given name
// Throws an exception if the charset does not exist
const std::string ASCIIConverter::getCharset(const std::string& title)
{
    auto it = charsets.find(title);
    if (it != charsets.end()) {
        std::string value = it->second;
        return value;
    }
    else {
        throw new std::runtime_error("Invalid charset");
    }
}

