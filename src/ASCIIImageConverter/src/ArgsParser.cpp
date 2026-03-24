#include "ArgsParser.h"

#include <string>
#include <stdexcept>
#include <cstdlib>

const std::string ArgsParser::dataPrefix = "DATADIR";

// Returns true if an argument starts with "--"
bool ArgsParser::isFlag(const std::string& arg) {
	return arg.rfind("--", 0) == 0;
}

// Parses command-line arguments and returns an Argument struct
Arguments ArgsParser::parse(int argc, char** argv) {
	if (argc < 2) {
		throw std::invalid_argument("No input file specified");
	}

	Arguments args;	
	const std::vector<std::string> charsets = {
		"default", "detailed", "alpha", "num", "minimalist", "alnum"
	};
	std::string inputPath = argv[1];	// The first argument is the input file
	if (inputPath.rfind(dataPrefix, 0) == 0) {
		args.inputFile = std::string(DATA_DIR) + "/" + inputPath.substr(dataPrefix.size());
	}
	else {
		args.inputFile = inputPath;
	}

	// Handle other arguments
	for (int i = 2; i < argc; i++) {
		std::string current = argv[i];
		if (!isFlag(current)) {
			throw std::invalid_argument("Unexpected argument: " + current);
		}

		// width
		if (current == "--width") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--width requires a value");
			}
			try {
				args.width = std::stoi(argv[++i]);
			}
			catch (const std::exception&) {
				throw std::invalid_argument("--width value must be an integer");
			}
			if (args.width <= 0) {
				throw std::invalid_argument("--width value must be positive");
			}
		}

		// brightness
		else if (current == "--brightness") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--brightness requires a value");
			}
			try {
				args.brightness = std::stoi(argv[++i]);
			}
			catch (const std::exception&) {
				throw std::invalid_argument("--brightness value must be an integer");
			}
		}

		// contrast
		else if (current == "--contrast") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--contrast requires a value");
			}
			try {
				args.contrast = std::stof(argv[++i]);
			}
			catch (const std::exception&) {
				throw std::invalid_argument("--contrast value must be a number");
			}
			if (args.contrast <= 0.0f) {
				throw std::invalid_argument("--contrast value must be positive");
			}
		}

		// filter
		else if (current == "--filter") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--filter requires a value");
			}
			args.filters.push_back(argv[++i]);
		}

		// charset
		else if (current == "--set") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--set requires a value");
			}
			std::string charset = argv[++i];

			if (std::find(charsets.begin(), charsets.end(), charset) == charsets.end()) {
				throw std::invalid_argument("Invalid charset: " + charset);
			}
			args.charset = charset;
		}

		// color
		else if (current == "--color") {
			args.color = true;
		}
		
		// interactive mode
		else if (current == "--interactive") {
			args.interactive = true;
		}

		// output file
		else if (current == "--output") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--output requires a value");
			}
			args.outputFile = argv[++i];
		}

		// invalid 
		else {
			throw std::invalid_argument("Unknown parameter: " + current);
		}
	}

	return args;
}