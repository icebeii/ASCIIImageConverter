#include "ArgsParser.h"

#include <string>
#include <stdexcept>
#include <cstdlib>

const std::string ArgsParser::dataPrefix = "DATADIR";

const std::vector<std::string> ArgsParser::charsets = {
		"default", "detailed", "alpha", "num", "minimalist", "alnum"
};

const std::vector<std::string> ArgsParser::filters = {
	"invert", "sepia", "blur", "sharpen", "edges"
};

// Returns true if an argument starts with "--"
bool ArgsParser::isFlag(const std::string& arg) {
	return arg.rfind("--", 0) == 0;
}

void ArgsParser::parseInteractive(const std::string& line, Arguments& args)
{
	std::istringstream iss(line);
	std::string value;
	iss >> cmd;

	/// width
	if (cmd == "width") {
		if (!(iss >> value)) {
			throw std::invalid_argument("width requires a value");
		}
		else {
			try {
				int temp = std::stoi(value);
				if (temp <= 0) {
					throw std::invalid_argument("width value must be positive");
				}
				else {
					args.width = temp;
				}
			}
			catch (const std::exception& e) {
				throw std::invalid_argument("width value must be an integer");
			}
		}
	}

	/// brightness
	else if (cmd == "brightness") {
		if (!(iss >> value)) {
			throw std::invalid_argument("brightness requires a value");
		}
		else {
			try {
				float temp = std::stoi(value);
				args.brightness = temp;
			}
			catch (const std::exception&) {
				throw std::invalid_argument("--brightness value must be a number");
			}
		}
	}

	/// contrast
	else if (cmd == "contrast") {
		if (!(iss >> value)) {
			throw std::invalid_argument("contrast requires a value");
		}
		else {
			try {
				float temp = std::stoi(value);
				if (temp < 0.0f) {
					throw std::invalid_argument("contrast value must be non negative");
				}
			}
			catch (const std::exception&) {
				throw std::invalid_argument("contrast value must be a number");
			}
		}
	}

	/// filter
	else if (cmd == "filter") {
		if (!(iss >> value)) {
			throw std::invalid_argument("filter requires a value");
		}
		else {
			if (std::find(filters.begin(), filters.end(), value) == filters.end()) {
				throw std::invalid_argument("invalid filter: " + value);
			}
			args.filters.push_back(value);
		}
	}

	/// charset
	else if (cmd == "set") {
		if (!(iss >> value)) {
			throw std::invalid_argument("set requires a value");
		}
		else {
			if (std::find(charsets.begin(), charsets.end(), value) == charsets.end()) {
				throw std::invalid_argument("invalid charset: " + value);
			}
			args.charset = value;
		}
	}

	/// color 
	else if (cmd == "color") {
		if (!(iss >> value)) {
			throw std::invalid_argument("color requires a boolean value");
		}
		else {
			if (value == "true") {
				args.color = true;
			}
			else if (value == "false") {
				args.color = false;
			}
			else {
				throw std::invalid_argument("color requires a boolean value");
			}
		}
	}

	/// output
	else if (cmd == "output") {
		if (!(iss >> value)) {
			throw std::invalid_argument("please specify the output filename");
		}
		args.outputFile = value;
	}

	/// invalid
	else {
		throw std::invalid_argument("unknown command: " + cmd);
	}
}

/// Parses command-line arguments and returns an Argument struct
Arguments ArgsParser::parse(int argc, char** argv) {
	if (argc < 2) {
		throw std::invalid_argument("No input file specified");
	}

	Arguments args;	
	std::string inputPath = argv[1];	/// The first argument is the input file
	if (inputPath.rfind(dataPrefix, 0) == 0) {
		args.inputFile = std::string(DATA_DIR) + "/" + inputPath.substr(dataPrefix.size());
	}
	else {
		args.inputFile = inputPath;
	}

	/// Handle other arguments
	for (int i = 2; i < argc; i++) {
		std::string current = argv[i];
		if (!isFlag(current)) {
			throw std::invalid_argument("Unexpected argument: " + current);
		}

		/// width
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

		/// brightness
		else if (current == "--brightness") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--brightness requires a value");
			}
			try {
				args.brightness = std::stoi(argv[++i]);
			}
			catch (const std::exception&) {
				throw std::invalid_argument("--brightness value must be a number");
			}
		}

		/// contrast
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
			if (args.contrast < 0.0f) {
				throw std::invalid_argument("--contrast value must be non negative");
			}
		}

		/// filter
		else if (current == "--filter") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--filter requires a value");
			}
			std::string filter = argv[++i];

			if (std::find(filters.begin(), filters.end(), filter) == filters.end()) {
				throw std::invalid_argument("invalid filter: " + filter);
			}
			args.filters.push_back(filter);
		}

		/// charset
		else if (current == "--set") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--set requires a value");
			}
			std::string charset = argv[++i];

			if (std::find(charsets.begin(), charsets.end(), charset) == charsets.end()) {
				throw std::invalid_argument("invalid charset: " + charset);
			}
			args.charset = charset;
		}

		/// color
		else if (current == "--color") {
			args.color = true;
		}
		
		/// interactive mode
		else if (current == "--interactive") {
			args.interactive = true;
		}

		/// output file
		else if (current == "--output") {
			if ((i + 1) >= argc) {
				throw std::invalid_argument("--output requires a value");
			}
			args.outputFile = argv[++i];
		}

		/// invalid 
		else {
			throw std::invalid_argument("Unknown parameter: " + current);
		}
	}

	return args;
}

void ArgsParser::runInteractive(Arguments& args)
{
	std::cout << "Running interactive mode. Type 'exit' to quit." << std::endl;

	std::string line;
	while (line != "exit") {
		std::getline(std::cin, line);
		try {
			parseInteractive(line, args);
		}
		catch(const std::exception& e){
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
}
