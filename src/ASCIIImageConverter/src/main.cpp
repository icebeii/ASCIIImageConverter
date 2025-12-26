#include <iostream>

#include "ArgsParser.h"

static void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

int main(int argc, char** argv)
{
    try {
        // Parse command-line arguments
        Arguments args = ArgsParser::parse(argc, argv);
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
}
