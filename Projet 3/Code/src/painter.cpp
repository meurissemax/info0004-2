#include <string>
#include <cstdlib>
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>
#include <fstream>

#include "headers/geometry.hpp"
#include "headers/graphics.hpp"
#include "headers/parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	const string VALID_EXT = ".paint";

	size_t i = 0, width, height;
	string input, filename, extension, outfilename;
	vector<shared_ptr<Shape>> shapes;

	if(argc < 2 || argc > 2) {
		cerr << "Usage : " << argv[0] << " INPUT_FILE" << endl;

		return 1;
	}

	input = argv[1];
	filename = input.substr(0, input.length() - VALID_EXT.size());
	extension = input.substr(input.length() - VALID_EXT.size(), VALID_EXT.size());

	if(extension != VALID_EXT) {
		cerr << "Extension of the input file must be '" << VALID_EXT << "'." << endl;

		return 1;
	}

	auto start = chrono::steady_clock::now();

	Parser parser(input);

	cout << "Parsing file...";

	parser.parse_file();

	cout << " DONE" << endl;

	parser.print_stats();

	width = parser.get_width();
	height = parser.get_height();
	shapes = parser.get_shapes();

	for(i = filename.length() - 1; i >= 0; i--) {
		if(filename.at(i) == '/') {
			i++;

			break;
		}
	}

	outfilename = filename.substr(i, filename.length());

	ofstream outfile("outputs/" + outfilename + ".ppm");

	if(!outfile) {
		cerr << "Unable to create output file" << endl;

		return 1;
	}

	cout << "Creating PPM file...";

	Image img(width, height);

	for(size_t i = 0; i < width; i++) {
		for(size_t j = 0; j < height; j++) {
			for(auto e = shapes.rbegin(); e != shapes.rend(); e++) {
				if((*e)->is_fill() && (*e)->contains(Point(i + 0.5, j + 0.5))) {
					img(i, j) = (*e)->get_color();

					break;
				}
			}
		}
	}

	outfile << img;
	outfile.close();

	cout << " DONE" << endl;

	auto end = chrono::steady_clock::now();
    auto diff = end - start;
    auto time = chrono::duration <double, milli> (diff).count();

	cout << "Elapsed time is " << time << " ms" << endl;

	return 0;
}
