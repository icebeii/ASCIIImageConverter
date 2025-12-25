#include <iostream>

#include "ArgsParser.h"

static void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

int main(int argc, char** argv)
{
    try {
        Arguments args = ArgsParser::parse(argc, argv);
        std::cout << "Input file: " << args.inputFile << std::endl;
        std::cout << "Width: " << args.width << std::endl;
        std::cout << "Contrast: " << args.contrast << std::endl;
        std::cout << "Filters count: " << args.filters.size() << std::endl;
        std::cout << "Charset: " << args.charset << std::endl;
        std::cout << "Color flag: " << args.color << std::endl;
        std::cout << "Interactive flag: " << args.interactive << std::endl;
        std::cout << "Output file: " << args.outputFile << std::endl;
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
}
