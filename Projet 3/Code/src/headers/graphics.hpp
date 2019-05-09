#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cstdint>
#include <cstring>
#include <memory>
#include <iostream>

/*********/
/* COLOR */
/*********/

struct Color {
    Color() : red(0), green(0), blue(0) { }
    Color(uint8_t r, uint8_t g, uint8_t b) : red(r), green(g), blue(b) { }

    uint8_t red, green, blue;
};

/*********/
/* IMAGE */
/*********/

class Image {
public:
    Image() : _width(0), _height(0) { }
    Image(size_t width, size_t height);

    size_t width() const { return _width; }
    size_t height() const { return _height; }

    Color operator()(size_t x, size_t y) const;
    Color& operator()(size_t x, size_t y);

private:
    size_t _width, _height;
    
    std::unique_ptr<Color[]> _pixels;
};

/*********/
/* OTHER */
/*********/

std::ostream& operator<<(std::ostream& out, const Color& c);
std::ostream& operator<<(std::ostream& out, const Image& img);

#endif
