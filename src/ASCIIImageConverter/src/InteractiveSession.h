#pragma once

#include "Arguments.h"
#include "ImageProcessor.h"

class InteractiveSession {
public:
	InteractiveSession(const Image& img, const Arguments& initial);
	void run();

private:
	Image original;
	Image current;
	std::vector<Arguments> history;
	int idx = 0;

	void apply(const InteractiveCommand& cmd);
	void undo();
	void redo();
	void rebuild();
};