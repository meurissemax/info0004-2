#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <chrono>

#include "headers/geometry.hpp"
#include "headers/graphics.hpp"
#include "headers/parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	const string VALID_EXT = ".paint";
	const string SEP = "------------";

	size_t width, height;
	string input, filename, extension;
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

	cout << SEP << endl;
	cout << "Parsing file..." << flush;

	parser.parse_file();

	cout << " DONE" << endl;
	cout << SEP << endl;

	parser.print_stats();

	cout << SEP << endl;

	width = parser.get_width();
	height = parser.get_height();
	shapes = parser.get_shapes();

	ofstream outfile(filename + ".ppm", ios::binary);

	if(!outfile) {
		cerr << "Unable to create output file" << endl;

		return 1;
	}

	cout << "Creating PPM file..." << flush;

	Image img(width, height);

	for(size_t i = 0; i < width; i++) {
		for(size_t j = 0; j < height; j++) {
			Point pixel(i + 0.5, j + 0.5);

			for(auto e = shapes.rbegin(); e != shapes.rend(); e++) {
				if((*e)->is_fill() && (*e)->contains(pixel)) {
					img(i, j) = (*e)->get_color();

					break;
				}
			}
		}
	}

	outfile << img;
	outfile.close();

	auto end = chrono::steady_clock::now();
    auto diff = end - start;
    auto time = chrono::duration <double, milli> (diff).count();

	cout << " DONE in " << time << " ms" << endl;
	cout << SEP << endl;

	return 0;
}
