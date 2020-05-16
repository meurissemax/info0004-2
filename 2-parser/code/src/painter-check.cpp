/**
 * Object-oriented programming projects - Project 2
 * Parsing a painting language
 *
 * This file is the main file of the program.
 * It reads the user's entries and creates the parser.
 * The parser parse the file and displays its statistics if no error has occurred.
 *
 * @author Maxime Meurisse (m.meurisse@student.uliege.be) - 20161278
 * @version 2019.05.04
 */

#include <string>
#include <iostream>

#include "parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	const string VALID_EXT = ".paint";

	string filename, extension;

	/// We check that the user has only given one argument.
	if(argc < 2 || argc > 2) {
		cerr << "Usage : " << argv[0] << " INPUT_FILE" << endl;

		return 1;
	}

	filename = argv[1];
	extension = filename.substr(filename.length() - VALID_EXT.size(), VALID_EXT.size());

	/// We check that the extension of the file is valid.
	if(extension != VALID_EXT) {
		cerr << "Extension of the input file must be '" << VALID_EXT << "'." << endl;

		return 1;
	}

	/// We instantiate a parser.
	Parser parser(filename);

	/// We parse the file.
	parser.parse_file();

	/// If there were no errors, the statistics of the parse are displayed.
	parser.print_stats();

	return 0;
}
