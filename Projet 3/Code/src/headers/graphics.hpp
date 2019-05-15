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

    /// All values are between 0 and 255.
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

    /// 'img(x, y)' returns the pixel on row 'x' and column 'y'.
    std::pair<bool, Color>& operator()(size_t x, size_t y);
    std::pair<bool, Color> operator() (size_t x, size_t y) const;

private:
    size_t width, height;

    /// Each color is represented by a pair of boolean and Color.
    /// The boolean value indicating if the pixel is filled by a shape or not.
    /// The Color is the color of the pixel.
    std::unique_ptr<std::pair<bool, Color>[]> pixels;
};

/*********/
/* OTHER */
/*********/

/// Write a color as three consecutive bytes (red, green and blue).
inline std::ostream& operator<<(std::ostream& out, const Color& c);

/// Write image in binary 8-bit PPM format.
std::ostream& operator<<(std::ostream& out, Image& img);

#endif
