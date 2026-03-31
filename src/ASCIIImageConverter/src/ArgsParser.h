#pragma once

#include <string>
#include <vector>
#include "Arguments.h"

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
	* Parses input commands in interactive mode
	*
	* @param line - input line
	* @param args - current arguments that should be modified by new command
	*
	* @throw std::invalid_argument if an invalid command is found
	*/
	InteractiveCommand parseInteractive(const std::string& line, Arguments& args);

private:
	/**
	* Checks if a string is a command-line flag (starts with "--")
	* 
	* @param arg - input string
	* @return true if arg is a flag, false otherwise
	*/
	static bool isFlag(const std::string& arg);

	/// data folder path prefix
	static const std::string dataPrefix;

	/// represents the console output
	static const std::string cout;

	/// valid character set titles
	static const std::vector<std::string> charsets;

	/// valid filters
	static const std::vector<std::string> filters;
};