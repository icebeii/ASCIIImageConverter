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
        std::unique_ptr<IImage> newImg = processor.processArgs(args);

        /// Print output
        ASCIIConverter converter(*newImg);
        std::string res = converter.convert(args);
        std::ostream& out = converter.getOutputStream(args.outputFile);
        converter.print(res, out);

        /// Start interactive mode
        if (args.interactive) {
            InteractiveSession session(*newImg, args);
            session.run();
        }
    }
    catch (const std::exception& e) {
        printError(e.what());
        return 1;
    }
    return 0;
}
