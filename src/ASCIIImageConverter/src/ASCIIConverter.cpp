#include "ASCIIConverter.h"

constexpr int BLOCK_WIDTH = 4;
constexpr int BLOCK_HEIGHT = 8;
constexpr double RED_WEIGHT = 0.3;
constexpr double GREEN_WEIGHT = 0.59;
constexpr double BLUE_WEIGHT = 0.11;
constexpr int MAX_BRIGHTNESS = 255;

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
ASCIIConverter::ASCIIConverter(const IImage& img) : image(img){}

// Converts an image to ASCII string, adding ANSI codes if color is true
std::string ASCIIConverter::convert(const Arguments& args) const {
    // Size of one block of pixels that will be reprsesented by one ASCII character
	const int blockWidth = BLOCK_WIDTH;
	const int blockHeight = BLOCK_HEIGHT;
	std::string result;

    std::string charset = getCharset(args.charset);
    int width = image.getWidth();
    int height = image.getHeight();

    // Iterate over an image in blocks
	for (int y = 0; y < height; y += blockHeight) {
		for (int x = 0; x < width; x += blockWidth) {

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
                    
                    if (px >= width || py >= height) {
                        continue;
                    }
                    const auto& pixels = image.getPixels();
                    const RGB& p = pixels[py * width + px];
                    int gray = RED_WEIGHT * p.r + GREEN_WEIGHT * p.g + BLUE_WEIGHT * p.b; // Luminosity method
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

            int index = avgGray * (charset.size() - 1) / MAX_BRIGHTNESS;
            char asciiChar = charset[index];

            if (args.color && args.outputFile == "") {
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
    static std::string current;

    if (filename.empty()) {
        return std::cout;
    }

    if (!file.is_open() || current != filename) {
        if (file.is_open()) {
            file.close();
        }

        file.open(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
        current = filename;
    }
    return file;
}

// Prints the ASCII result to the given output stream
void ASCIIConverter::print(const std::string& result, std::ostream& out) const {
    out << result;
    out.flush();
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
        throw std::runtime_error("Invalid charset");
    }
}

