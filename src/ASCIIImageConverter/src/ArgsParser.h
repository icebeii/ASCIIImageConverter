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
	std::string outputFile;				/// Output file name
};

/// Class for parsing command-line arguments
class ArgsParser {
public:
	/**
	* Parses command-line arguments
	* 
	* @param argc - argument count
	* @param argv - argument values
	* @return an Argument structure with all values set
	* 
	* @throw std::invalid_argument if an invalid argument is found
	*/
	static Arguments parse(int argc, char** argv);

	/**
	* handles interactive mode
	* 
	* @param args - command line arguments 
	*				that will be modified during the interactive mode
	*/
	void runInteractive(Arguments& args);

private:
	/**
	* Checks if a string is a command-line flag (starts with "--")
	* 
	* @param arg - input string
	* @return true if arg is a flag, false otherwise
	*/
	static bool isFlag(const std::string& arg);

	/**
	* Parses input commands in interactive mode
	* 
	* @param line - input line
	* @param args - current arguments that should be modified by new command
	* 
	* @throw std::invalid_argument if an invalid command is found
	*/
	void parseInteractive(const std::string& line, Arguments& args);

	/// data folder path prefix
	static const std::string dataPrefix;

	/// valid character set titles
	static const std::vector<std::string> charsets;

	/// valid filters
	static const std::vector<std::string> filters;
};