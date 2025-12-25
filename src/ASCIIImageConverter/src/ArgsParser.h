#pragma once

#include <string>
#include <vector>

struct Arguments {
	std::string inputFile;
	int width = 0;
	float contrast = 1.0f;
	std::vector<std::string> filters;
	std::string charset = "default";
	bool color = false;
	bool interactive = false;
	std::string outputFile;
};

class ArgsParser {
public:
	static Arguments parse(int argc, char** argv);

private:
	static bool isFlag(const std::string& arg);
};