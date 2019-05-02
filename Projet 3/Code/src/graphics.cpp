#include <cassert>

#include "headers/graphics.hpp"

using namespace std;

/*********/
/* IMAGE */
/*********/

/* Public members */

Image::Image(size_t width, size_t height) {
	_width = width;
	_height = height;

	_pixels = std::make_unique<Color[]>(width * height);
}

size_t Image::width() const {
	return _width;
}

size_t Image::height() const {
	return _height;
}

Color Image::operator()(size_t x, size_t y) const {
	assert(x < _width);
	assert(y < _height);

	return _pixels[y * _width + x];
}

Color& Image::operator()(size_t x, size_t y) {
	assert(x < _width);
	assert(y < _height);

	return _pixels[y * _width + x];
}

/*********/
/* OTHER */
/*********/

ostream& operator<<(ostream& out, const Color& c) {
	return out << c.red << c.green << c.blue;
}

ostream& operator<<(ostream& out, const Image& img) {
	size_t width = img.width(), height = img.height();

	assert(width > 0);
	assert(height > 0);

	out << "P6 " << width << " " << height << " 255\n";

	for(size_t y = 0; y < height; y++)
		for(size_t x = 0; x < width; x++)
			out << img(x, height - y - 1);

	return out;
}