#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include <cstdint>
#include <cstring>
#include <utility>
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
    Image() : width(0), height(0) { }
    Image(size_t w, size_t h);

    size_t get_width() const { return width; }
    size_t get_height() const { return height; }

    std::pair<bool, Color>& operator()(size_t x, size_t y);

private:
    size_t width, height;
    std::unique_ptr<std::pair<bool, Color>[]> pixels;
};

/*********/
/* OTHER */
/*********/

std::ostream& operator<<(std::ostream& out, const Color& c);
std::ostream& operator<<(std::ostream& out, Image& img);

#endif
