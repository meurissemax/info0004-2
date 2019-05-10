#include <cmath>
#include <cassert>

#include "headers/geometry.hpp"

using namespace std;

/* Basic structure */

/*********/
/* POINT */
/*********/

void Point::shift(Point p) {
	x += p.x;
	y += p.y;
}

void Point::rotate(Point p, double angle) {
	double d_x = x - p.x, d_y = y - p.y;
	double sin_a, cos_a;

	angle *= (M_PI / 180.0);
	sin_a = sin(angle);
	cos_a = cos(angle);

	x = d_x * cos_a - d_y * sin_a + p.x;
	y = d_x * sin_a + d_y * cos_a + p.y;
}

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

void Shape::set_color(Color c) {
	color = c;
	fill = true;
}

Color Shape::get_color() const {
	assert(fill);

	return color;
}

/* Primitive shapes */

/********/
/* ELLI */
/********/

Elli::Elli(string name, Point c, double a, double b) {
	Elli::name = name;
	_c = c;
	_a = a;
	_b = b;
}

Point Elli::get_named_point(string name) const {
	double incr = M_SQRT2 / 2;

	if(name == "c")
		return _c;
	else if(name == "nw")
		return Point(_c.x - _a * incr, _c.y + _b * incr);
	else if(name == "n")
		return Point(_c.x, _c.y + _b);
	else if(name == "ne")
		return Point(_c.x + _a * incr, _c.y + _b * incr);
	else if(name == "e")
		return Point(_c.x + _a, _c.y);
	else if(name == "se")
		return Point(_c.x + _a * incr, _c.y - _b * incr);
	else if(name == "s")
		return Point(_c.x, _c.y - _b);
	else if(name == "sw")
		return Point(_c.x - _a * incr, _c.y - _b * incr);
	else if(name == "w")
		return Point(_c.x - _a, _c.y);
	else if(_a != _b) {
		double d_f = sqrt(pow(_a, 2) - pow(_b, 2));

		if(name == "f1")
			return Point(_c.x + d_f, _c.y);
		else if(name == "f2")
			return Point(_c.x - d_f, _c.y);
		else
			throw invalid_argument("Point doesn't exist.");
	} else
		throw invalid_argument("Point doesn't exist.");
}

bool Elli::contains(Point p) const {
	return pow((p.x - _c.x) / _a, 2) + pow((_c.y - p.y) / _b, 2) <= 1.0;
}

/********/
/* RECT */
/********/

Rect::Rect(string name, Point c, double width, double height) {
	Rect::name = name;
	_c = c;
	_width = width;
	_height = height;
	_mid_width = _width / 2;
	_mid_height = _height / 2;
}

Point Rect::get_named_point(string name) const {
	if(name == "c")
		return _c;
	else if(name == "nw")
		return Point(_c.x - _mid_width, _c.y + _mid_height);
	else if(name == "n")
		return Point(_c.x, _c.y + _mid_height);
	else if(name == "ne")
		return Point(_c.x + _mid_width, _c.y + _mid_height);
	else if(name == "e")
		return Point(_c.x + _mid_width, _c.y);
	else if(name == "se")
		return Point(_c.x + _mid_width, _c.y - _mid_height);
	else if(name == "s")
		return Point(_c.x, _c.y - _mid_height);
	else if(name == "sw")
		return Point(_c.x - _mid_width, _c.y - _mid_height);
	else if(name == "w")
		return Point(_c.x - _mid_width, _c.y);
	else
		throw invalid_argument("Point doesn't exist.");
}

bool Rect::contains(Point p) const {
	return abs(p.x - _c.x) <= _mid_width && abs(p.y - _c.y) <= _mid_height;
}

/*******/
/* TRI */
/*******/

Tri::Tri(string name, Point v0, Point v1, Point v2) {
	Tri::name = name;
	_v0 = v0;
	_v1 = v1;
	_v2 = v2;
}

Point Tri::get_named_point(string name) const {
	if(name == "c")
		return Point((_v0.x + _v1.x + _v2.x) / 3, (_v0.y + _v1.y + _v2.y) / 3);
	else if(name == "v0")
		return _v0;
	else if(name == "v1")
		return _v1;
	else if(name == "v2")
		return _v2;
	else if(name == "s01")
		return Point((_v0.x + _v1.x) / 2, (_v0.y + _v1.y) / 2);
	else if(name == "s02")
		return Point((_v0.x + _v2.x) / 2, (_v0.y + _v2.y) / 2);
	else if(name == "s12")
		return Point((_v1.x + _v2.x) / 2, (_v1.y + _v2.y) / 2);
	else
		throw invalid_argument("Point doesn't exist.");
}

bool Tri::contains(Point p) const {
	double d1 = (p.x - _v1.x) * (_v0.y - _v1.y) - (_v0.x - _v1.x) * (p.y - _v1.y);
	double d2 = (p.x - _v2.x) * (_v1.y - _v2.y) - (_v1.x - _v2.x) * (p.y - _v2.y);
	double d3 = (p.x - _v0.x) * (_v2.y - _v0.y) - (_v2.x - _v0.x) * (p.y - _v0.y);

	return !(((d1 < 0) || (d2 < 0) || (d3 < 0)) && ((d1 > 0) || (d2 > 0) || (d3 > 0)));
}

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

Shift::Shift(string name, Point p, shared_ptr<Shape> ref_shape) {
	Shift::name = name;
	_p = p;
	_p_inv = Point(- p.x, - p.y);
	_ref_shape = ref_shape;
}

Point Shift::get_named_point(string name) const {
	Point p = _ref_shape->get_named_point(name);
	p.shift(_p);

	return p;
}

bool Shift::contains(Point p) const {
	p.shift(_p_inv);

	return _ref_shape->contains(p);
}

/*******/
/* ROT */
/*******/

Rot::Rot(string name, double angle, Point p, shared_ptr<Shape> ref_shape) {
	Rot::name = name;
	_angle = angle;
	_p = p;
	_ref_shape = ref_shape;
}

Point Rot::get_named_point(string name) const {
	Point p = _ref_shape->get_named_point(name);
	p.rotate(_p, _angle);

	return p;
}

bool Rot::contains(Point p) const {
	p.rotate(_p, - _angle);

	return _ref_shape->contains(p);
}

/*********/
/* UNION */
/*********/

Union::Union(string name, vector<shared_ptr<Shape>> shapes) {
	Union::name = name;
	_shapes = shapes;
}

Point Union::get_named_point(string name) const {
	return _shapes.at(0)->get_named_point(name);
}

bool Union::contains(Point p) const {
	for(const auto& shape : _shapes)
		if(shape->contains(p))
			return true;

	return false;
}

/********/
/* DIFF */
/********/

Diff::Diff(string name, shared_ptr<Shape> shape_from, shared_ptr<Shape> shape_to) {
	Diff::name = name;
	_shape_from = shape_from;
	_shape_to = shape_to;
}

Point Diff::get_named_point(string name) const {
	return _shape_from->get_named_point(name);
}

bool Diff::contains(Point p) const {
	return _shape_from->contains(p) && !_shape_to->contains(p);
}
