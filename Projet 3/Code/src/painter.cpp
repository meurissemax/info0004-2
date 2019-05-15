#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <chrono>

#include "headers/geometry.hpp"
#include "headers/graphics.hpp"
#include "headers/parser.hpp"

using namespace std;

int main(int argc, char* argv[]) {
	/* Variable declaration */

	const string VALID_EXT = ".paint"; /// valid extension of a paint file
	const string SEPARATOR = "------------";

	size_t width, height; /// dimension of the image
	string input, filename, extension; /// data about input of the user
	vector<shared_ptr<Shape>> fills; /// shapes to draw

	/* Input verification */

	/// Number of argument (only one paint file is accepted)
	if(argc < 2 || argc > 2) {
		cerr << "Usage : " << argv[0] << " INPUT_FILE" << endl;

		return 1;
	}

	input = argv[1];

	/// Length of the input (at least the length of the extension)
	if(input.length() <= VALID_EXT.length()) {
		cerr << "Invalid input file name." << endl;

		return 1;
	}

	extension = input.substr(input.length() - VALID_EXT.length(), VALID_EXT.length());

	/// Validity of the extension
	if(extension != VALID_EXT) {
		cerr << "Extension of the input file must be '" << VALID_EXT << "'." << endl;

		return 1;
	}

	filename = input.substr(0, input.find_last_of('.'));

	/* Parsing the input */

	Parser parser(input);

	cout << "Parsing file... " << flush;

	auto start = chrono::steady_clock::now();

	parser.parse_file();

	auto end = chrono::steady_clock::now();
	auto diff = end - start;
	auto time_parser = chrono::duration <double, milli> (diff).count();

	cout << "DONE in " << time_parser << " ms" << endl;
	cout << SEPARATOR << endl;

	parser.print_stats();

	cout << SEPARATOR << endl;

	/* Filling the image */

	width = parser.get_width();
	height = parser.get_height();
	fills = parser.get_fills();

	ofstream outfile(filename + ".ppm", ios::binary);

	/// If an error occurred while creatinf output file
	if(!outfile) {
		cerr << "Unable to create output file" << endl;

		return 1;
	}

	cout << "Creating PPM file... " << flush;

	start = chrono::steady_clock::now();

	Image img(width, height);

	int x_min, x_max, y_min, y_max;
	domain dom;

	/// For each shape, we check pixels that are in the domain of the shape.
	for(auto shape = fills.rbegin(); shape != fills.rend(); shape++) {
		dom = (*shape)->get_domain();

		x_min = max(int(dom[0].x), int(0));
		y_min = max(int(dom[0].y), int(0));
		x_max = min(int(++dom[1].x), int(width));
		y_max = min(int(++dom[1].y), int(height));

		for(int x = x_min; x < x_max; x++) {
			for(int y = y_min; y < y_max; y++) {
				if(!img(size_t(x), size_t(y)).first && (*shape)->contains(Point(double(x) + 0.5, double(y) + 0.5))) {
					img(size_t(x), size_t(y)).first = true;
					img(size_t(x), size_t(y)).second = (*shape)->get_color();
				}
			}
		}
	}

	outfile << img;
	outfile.close(); 

	end = chrono::steady_clock::now();
	diff = end - start;
	auto time_painter = chrono::duration <double, milli> (diff).count();

	cout << "DONE in " << time_painter << " ms" << endl;
	
	return 0;
}
