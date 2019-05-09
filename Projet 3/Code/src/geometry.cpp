#include <cmath>
#include <cassert>

#include "headers/geometry.hpp"

/* Basic structure */

/*********/
/* POINT */
/*********/

void Point::shift(Point p) {
	x += p.x;
	y += p.y;
}

void Point::rotate(Point p, double angle) {
	double tmp_x = x, sin_a, cos_a;

	angle *= (M_PI / 180.0);
	sin_a = sin(angle);
	cos_a = cos(angle);

	x = (x - p.x) * cos_a - (y - p.y) * sin_a + p.x;
	y = (tmp_x - p.x) * sin_a + (y - p.y) * cos_a + p.y;
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
/* CIRC */
/********/

Circ::Circ(std::string name, Point c, double radius) {
	Circ::name = name;
	_c = c;
	_radius = radius;
}

Point Circ::get_named_point(std::string name) const {
	double incr = (M_SQRT2 / 2) * _radius;

	if(name == "c")
		return _c;
	else if(name == "nw")
		return Point(_c.x - incr, _c.y + incr);
	else if(name == "n")
		return Point(_c.x, _c.y + _radius);
	else if(name == "ne")
		return Point(_c.x + incr, _c.y + incr);
	else if(name == "e")
		return Point(_c.x + _radius, _c.y);
	else if(name == "se")
		return Point(_c.x + incr, _c.y - incr);
	else if(name == "s")
		return Point(_c.x, _c.y - _radius);
	else if(name == "sw")
		return Point(_c.x - incr, _c.y - incr);
	else if(name == "w")
		return Point(_c.x - _radius, _c.y);
	else
		throw std::invalid_argument("Point doesn't exist.");
}

bool Circ::contains(Point p) const {
	if(pow(p.x - _c.x, 2) + pow(p.y - _c.y, 2) <= pow(_radius, 2))
		return true;

	return false;
}

/********/
/* ELLI */
/********/

Elli::Elli(std::string name, Point c, double a, double b) {
	Elli::name = name;
	_c = c;
	_a = a;
	_b = b;
}

Point Elli::get_named_point(std::string name) const {
	double incr = M_SQRT2 / 2, d_f = sqrt(pow(_a, 2) - pow(_b, 2));

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
	else if(name == "f1")
		return Point(_c.x + d_f, _c.y);
	else if(name == "f2")
		return Point(_c.x - d_f, _c.y);
	else
		throw std::invalid_argument("Point doesn't exist.");
}

bool Elli::contains(Point p) const {
	if(pow(p.x - _c.x, 2) / pow(_a, 2) + pow(_c.y - p.y, 2) / pow(_b, 2) <= 1.0)
		return true;

	return false;
}

/********/
/* RECT */
/********/

Rect::Rect(std::string name, Point c, double width, double height) {
	Rect::name = name;
	_c = c;
	_width = width;
	_height = height;
}

Point Rect::get_named_point(std::string name) const {
	double mid_width = _width / 2, mid_height = _height / 2;

	if(name == "c")
		return _c;
	else if(name == "nw")
		return Point(_c.x - mid_width, _c.y + mid_height);
	else if(name == "n")
		return Point(_c.x, _c.y + mid_height);
	else if(name == "ne")
		return Point(_c.x + mid_width, _c.y + mid_height);
	else if(name == "e")
		return Point(_c.x + mid_width, _c.y);
	else if(name == "se")
		return Point(_c.x + mid_width, _c.y - mid_height);
	else if(name == "s")
		return Point(_c.x, _c.y - mid_height);
	else if(name == "sw")
		return Point(_c.x - mid_width, _c.y - mid_height);
	else if(name == "w")
		return Point(_c.x - mid_width, _c.y);
	else
		throw std::invalid_argument("Point doesn't exist.");
}

bool Rect::contains(Point p) const {
	double mid_width = _width / 2, mid_height = _height / 2;

	if(p.x >= _c.x - mid_width && p.x <= _c.x + mid_width && p.y >= _c.y - mid_height && p.y <= _c.y + mid_height)
		return true;

	return false;
}

/*******/
/* TRI */
/*******/

Tri::Tri(std::string name, Point v0, Point v1, Point v2) {
	Tri::name = name;
	_v0 = v0;
	_v1 = v1;
	_v2 = v2;
}

Point Tri::get_named_point(std::string name) const {
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
		throw std::invalid_argument("Point doesn't exist.");
}

bool Tri::contains(Point p) const {
	double eps = 0.00001;

	double by_cy = _v1.y - _v2.y;
	double cx_bx = _v2.x - _v1.x;
	double bxcy_cxby = _v1.x * _v2.y - _v2.x * _v1.y;
	double cy_ay = _v2.y - _v0.y;
	double ax_cx = _v0.x - _v2.x;
	double cxay_axcy = _v2.x * _v0.y - _v0.x * _v2.y;
	double ay_by = _v0.y - _v1.y;
	double bx_ax = _v1.x - _v0.x;
	double axby_bxay = _v0.x * _v1.y - _v1.x * _v0.y;

	double d = (_v0.x * by_cy + _v1.x * cy_ay + _v2.x * ay_by);

	if(fabs(d) < eps)
		return false;

	d = 1 / d;

	double a1 = (p.x * by_cy + p.y * cx_bx + bxcy_cxby) * d;
	double a2 = (p.x * cy_ay + p.y * ax_cx + cxay_axcy) * d;
	double a3 = (p.x * ay_by + p.y * bx_ax + axby_bxay) * d;

	return a1 >= 0 && a2 >= 0 && a3 >= 0 && a1 + a2 + a3 <= 1 + eps;
}

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

Shift::Shift(std::string name, Point p, std::shared_ptr<Shape> ref_shape) {
	Shift::name = name;
	_p = p;
	_ref_shape = ref_shape;
}

Point Shift::get_named_point(std::string name) const {
	Point p = _ref_shape->get_named_point(name);
	p.shift(_p);

	return p;
}

bool Shift::contains(Point p) const {
	Point inv(- _p.x, - _p.y);
	p.shift(inv);

	return _ref_shape->contains(p);
}

/*******/
/* ROT */
/*******/

Rot::Rot(std::string name, double angle, Point p, std::shared_ptr<Shape> ref_shape) {
	Rot::name = name;
	_angle = angle;
	_p = p;
	_ref_shape = ref_shape;
}

Point Rot::get_named_point(std::string name) const {
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

Union::Union(std::string name, std::vector<std::shared_ptr<Shape>> shapes) {
	Union::name = name;
	_shapes = shapes;
}

Point Union::get_named_point(std::string name) const {
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

Diff::Diff(std::string name, std::vector<std::shared_ptr<Shape>> shapes) {
	Diff::name = name;
	_shapes = shapes;
}

Point Diff::get_named_point(std::string name) const {
	return _shapes.at(0)->get_named_point(name);
}

bool Diff::contains(Point p) const {
	bool in_first = false, in_other = false;

	if(_shapes.at(0)->contains(p))
		in_first = true;

	for(auto e = _shapes.cbegin() + 1; e != _shapes.cend(); e++)
		if((*e)->contains(p))
			in_other = true;

	if(in_first && !in_other)
		return true;

	return false;
}
