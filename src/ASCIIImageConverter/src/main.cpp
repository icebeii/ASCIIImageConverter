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
    try {
        // Parse command-line arguments
        Arguments args = ArgsParser::parse(argc, argv);
        Image image = ImageLoader::loadFromFile(args.inputFile);
        ImageProcessor processor(image);
        Image newImg = processor.processArgs(args);
        ASCIIConverter converter(newImg);
        std::string res = converter.convert("default", args.color);
        converter.print(res, std::cout);
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
}
