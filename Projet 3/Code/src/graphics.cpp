#include <cassert>

#include "headers/graphics.hpp"

using namespace std;

/*********/
/* IMAGE */
/*********/

Image::Image(size_t w, size_t h) {
	width = w;
	height = h;
	pixels = make_unique<pair<bool, Color>[]>(w * h);
}

pair<bool, Color>& Image::operator()(size_t x, size_t y) {
	assert(x < width);
	assert(y < height);

	return pixels[y * width + x];
}

pair<bool, Color> Image::operator()(size_t x, size_t y) const {
	assert(x < width);
	assert(y < height);

	return pixels[y * width + x];
}

/*********/
/* OTHER */
/*********/

ostream& operator<<(ostream& out, const Color& c) {
	return out << c.red << c.green << c.blue;
}

ostream& operator<<(ostream& out, Image& img) {
	size_t width = img.get_width(), height = img.get_height();

	assert(width > 0);
	assert(height > 0);

	out << "P6 " << width << " " << height << " 255\n";

	for(size_t y = 0; y < height; y++)
		for(size_t x = 0; x < width; x++)
			out << img(x, height - y - 1).second;

	return out;
}
