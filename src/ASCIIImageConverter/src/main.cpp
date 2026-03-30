#include <iostream>

#include "ArgsParser.h"
#include "Image.h"
#include "ImageLoader.h"
#include "ImageProcessor.h"
#include "ASCIIConverter.h"
#include "InteractiveSession.h"

static void printError(const std::string& message) {
    std::cerr << "Error: " << message << std::endl;
}

int main(int argc, char** argv)
{
    try {
        /// Parse command-line arguments
        Arguments args = ArgsParser::parse(argc, argv);
        Image image = ImageLoader::loadFromFile(args.inputFile);

        /// Apply changes
        ImageProcessor processor(image);
        Image newImg = processor.processArgs(args);

        /// Print output
        ASCIIConverter converter(newImg);
        std::string res = converter.convert(args.charset, args.color);
        std::ostream& out = converter.getOutputStream(args.outputFile);
        converter.print(res, out);

        /// Start interactive mode
        if (args.interactive) {
            InteractiveSession session(image, args);
            session.run();
        }
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
}
