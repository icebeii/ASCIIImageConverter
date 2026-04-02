#include "ArgsParser.h"

#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

const std::string ArgsParser::dataPrefix = "DATADIR";

const std::string ArgsParser::cout = "STDCOUT";

const std::vector<std::string> ArgsParser::charsets = {
		"default", "detailed", "alpha", "num", "minimalist", "alnum"
};

const std::vector<std::string> ArgsParser::filters = {
	"invert", "grayscale", "sepia", "blur", "sharpen", "edges"
};

// Returns true if an argument starts with "--"
bool ArgsParser::isFlag(const std::string& arg) {
	return arg.rfind("--", 0) == 0;
}

InteractiveCommand ArgsParser::parseInteractive(const std::string& line, Arguments& args)
{
	std::istringstream iss(line);
	std::string value;
	std::string cmd;
	std::string extra;
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
				float temp = std::stof(value);
				args.brightness += temp;
			}
			catch (const std::exception&) {
				throw std::invalid_argument("brightness value must be a number");
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
				float temp = std::stof(value);
				if (temp < 0.0f) {
					throw std::invalid_argument("contrast value must be non negative");
				}
				args.contrast *= temp;
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
			throw std::invalid_argument("color requires an on/off parameter");
		}
		else {
			if (value == "on") {
				args.color = true;
			}
			else if (value == "off") {
				args.color = false;
			}
			else {
				throw std::invalid_argument("color requires an on/off parameter");
			}
		}
	}

	/// output
	else if (cmd == "output") {
		if (!(iss >> value)) {
			throw std::invalid_argument("please specify the output filename or \"" + cout + "\" for console output");
		}
		if (value == cout) {
			value = "";
		}
		args.outputFile = value;
	}

	///	undo/redo
	else if (cmd == "undo" || cmd == "redo") {
		return { cmd, "" };
	}

	/// invalid
	else {
		throw std::invalid_argument("unknown command: " + cmd);
	}
	 
	if (iss >> extra) {
		throw std::invalid_argument("too many arguments for command: " + cmd);
	}
	return { cmd, value };
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
				args.brightness = std::stof(argv[++i]);
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
				throw std::invalid_argument("--output requires an output filename or \"" + cout + "\" for console output");
			}
			std::string filename = argv[++i];
			if (filename == cout) {
				filename = "";
			}
			args.outputFile = filename;
		}

		/// invalid 
		else {
			throw std::invalid_argument("unknown parameter: " + current);
		}
	}

	return args;
}
