#pragma once

#include "Arguments.h"
#include "ImageProcessor.h"

class InteractiveSession {
public:
	/// Constructor
	InteractiveSession(const Image& img, const Arguments& initial);

	/**
	* Handles interactive mode
	*/
	void run();

private:
	const Image original;			/// Original image
	Image current;					/// Current image
	std::vector<Arguments> history; /// Sequence of changes applied to the image
	int idx = 0;					/// Current position in history

	/**
	* Applies changes entered by interactive command to the current image
	* 
	* @param cmd - input command
	*/
	void apply(const InteractiveCommand& cmd);

	/**
	* Performs one step back in the image history
	*/
	void undo();

	/**
	* Performs one step forward in the image history
	*/
	void redo();

	/**
	* Rebuilds the image based on the current state of history
	*/
	void rebuild();
};