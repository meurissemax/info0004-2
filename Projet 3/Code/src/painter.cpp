#include <fstream>
#include <vector>
#include <memory>

#include "headers/geometry.hpp"
#include "headers/graphics.hpp"

using namespace std;

int main() {
	int width = 100, height = 100;
	ofstream outfile("outputs/test.ppm");

	if(!outfile) {
		cerr << "Unable to create output file" << endl;

		return 1;
	}

	Image img(width, height);
	vector<shared_ptr<Shape>> shapes;

	shapes.push_back(shared_ptr<Shape>(new Rect("rect", Point(50, 50), 60, 40)));

	shapes.at(0)->set_color(0, 0.5, 1);

	for(int i = 0; i < width; i++)
		for(int j = 0; j < height; j++)
			for(auto e : shapes)
				if(e->is_fill() && e->is_in(Point(i + 0.5, j + 0.5)))
					img(i, j) = e->get_color();

	outfile << img;
	outfile.close();

	return 0;
}
