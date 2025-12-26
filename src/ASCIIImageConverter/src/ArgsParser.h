#pragma once

#include <string>
#include <vector>

// Structure to store command-line arguments
struct Arguments {
	std::string inputFile;				// Input file name
	int width = 0;						// Output width
	float contrast = 1.0f;				// Image contrast
	std::vector<std::string> filters;	// List of filters to apply
	std::string charset = "default";	// Character set for output
	bool color = false;					// Flag to enable color output
	bool interactive = false;			// Flag to enable interactive mode
	std::string outputFile;				// Output file name
};

// Class for parsing command-line arguments
class ArgsParser {
public:
	// Parses command-line arguments
	//	argc - argument count
	//	argv - argument values
	// Returns an Argument struct with all values set
	static Arguments parse(int argc, char** argv);

private:
	// Checks if a string is a command-line flag (starts with "--")
	//	arg - input string
	// Returns true if arg is a flag, false otherwise
	static bool isFlag(const std::string& arg);
};