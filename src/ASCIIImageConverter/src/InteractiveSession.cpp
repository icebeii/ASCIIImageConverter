#include "InteractiveSession.h"
#include "ASCIIConverter.h"

InteractiveSession::InteractiveSession(const IImage& img, const Arguments& initial)
{
	original = img.clone();
	current = img.clone();
	history.push_back(initial);
	ImageProcessor processor(*current);
	current = processor.processArgs(initial);
}

/// Hnadles interactive mode
void InteractiveSession::run()
{
	std::cout << "Running interactive mode. Type 'exit' to quit." << std::endl;
	std::string line;
	ArgsParser parser;

	while (true) {
		std::getline(std::cin, line);
		if (line == "exit") {
			break;
		}

		try {
			Arguments args = history[idx];
			InteractiveCommand cmd = parser.parseInteractive(line, args);
			apply(cmd);

			if (cmd.name != "undo" && cmd.name != "redo") {
				history.resize(idx + 1);
				history.push_back(args);
				idx++;
			}

			ASCIIConverter converter(*current);
			std::string result = converter.convert(history[idx]);
			std::ostream& out = converter.getOutputStream(history[idx].outputFile);
			converter.print(result, out);
		}
		catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
}

/// Applies the interactive command
void InteractiveSession::apply(const InteractiveCommand & cmd)
{
	if (cmd.name == "color" || cmd.name == "charset" || cmd.name == "output") {
		return;
	}
	else if (cmd.name == "undo") {
		undo();
	}
	else if (cmd.name == "redo") {
		redo();
	}
	else {
		ImageProcessor processor(*current);
		current = processor.processInteractive(cmd);
	}
}

void InteractiveSession::undo() {
	if (idx <= 0) {
		return;
	}
	else {
		idx--;
		rebuild();
	}
}

void InteractiveSession::redo() {
	if (idx + 1 >= history.size()) {
		return;
	}
	else {
		idx++;
		rebuild();
	}
}

void InteractiveSession::rebuild() {
	ImageProcessor processor(*original);
	current = processor.processArgs(history[idx]);
}
