#pragma once

#include <string>
#include <vector>

/// Structure to store command-line arguments
struct Arguments {
	std::string inputFile;				/// Input file name
	int width = 0;						/// Output width
	float contrast = 1.0f;				/// Image contrast
	float brightness = 0.0f;			/// Image brightness
	std::vector<std::string> filters;	/// List of filters to apply
	std::string charset = "default";	/// Character set for output
	bool color = false;					/// Flag to enable color output
	bool interactive = false;			/// Flag to enable interactive mode
	std::string outputFile = "";		/// Output file name
};

/// Structure to store a single interactive command
struct InteractiveCommand {
	std::string name;		/// name of the command
	std::string value;		/// corresponding value if needed
};