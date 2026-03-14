#include <iostream>

#include "ArgsParser.h"
#include "Image.h"
#include "ImageLoader.h"
#include "ImageProcessor.h"
#include "ASCIIConverter.h"

static void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

int main(int argc, char** argv)
{
    /*
    try {
        // Parse command-line arguments
        Arguments args = ArgsParser::parse(argc, argv);
        Image image = ImageLoader::loadFromFile(args.inputPath);
        ImageProcessor processor(image);
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
    */
    try {
        std::string filename = "example.jpg";
        std::string path = std::string(DATA_DIR) + "/" + filename;

        Image img = ImageLoader::loadFromFile(path.c_str());

        std::cout << "Image uploaded" << std::endl;
        std::cout << "Size: " << img.width << " x " << img.height << std::endl;
        ImageProcessor processor(img);
        Image newImg = processor.resize(img, 800);
        //newImg = processor.sepia(newImg);
        ASCIIConverter converter(newImg);
        std::string res = converter.convert("default", true);
        converter.print(res, std::cout);
    }
    catch (const std::exception& e) {
        std::cout << e.what();
        return 1;
    }
   
    return 0;
}
