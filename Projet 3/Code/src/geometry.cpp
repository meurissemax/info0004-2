#include <cmath>
#include <cassert>
#include <cstdlib>

#include "headers/geometry.hpp"

using namespace std;

/* Basic structure */

/*********/
/* POINT */
/*********/

Point Point::operator+(Point p) const {
	return Point(x + p.x, y + p.y);
}

Point Point::operator-(Point p) const {
	return Point(x - p.x, y - p.y);
}

Point Point::operator*(double n) const {
	return Point(x * n, y * n);
}

Point Point::operator/(double n) const {
	return Point(x / n, y / n);
}

void Point::shift(Point p) {
	x = x + p.x;
	y = y + p.y;
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

string Shape::get_name() const {
	return name;
}

bool Shape::is_fill() const {
	return fill;
}

void Shape::set_color(Color c) {
	color = c;
	fill = true;
}

Color Shape::get_color() const {
	assert(fill);

	return color;
}

Point Shape::get_named_point(string name) const {
	bool find = false;
	Point p;

	for(const auto e : named_points) {
		if(e.first == name) {
			p = e.second;
			find = true;

			break;
		}
	}

	if(!find) {
		cerr << "Point " << name << " doesn't exist in shape " << Shape::name << endl;

		exit(EXIT_FAILURE);
	}

	return p;
}

bool Shape::contains_polygon(vector<Point> vertices, Point p) const {
	int n = vertices.size(), i, j = n - 1;
	double v_i_x, v_i_y, v_j_x, v_j_y;
	bool odd_nodes = false;

	for(i = 0; i < n; i++) {
		v_i_x = vertices.at(i).x;
		v_i_y = vertices.at(i).y;
		v_j_x = vertices.at(j).x;
		v_j_y = vertices.at(j).y;

		if(((v_i_y < p.y && v_j_y >= p.y) || (v_j_y < p.y && v_i_y >= p.y)) && (v_i_x <= p.x || v_j_x <= p.x))
			if(v_i_x + (p.y - v_i_y) / (v_j_y - v_i_y) * (v_j_x - v_i_x) < p.x)
				odd_nodes = !odd_nodes;

		j = i;
	}

	return odd_nodes;
}

/* Primitive shapes */

/********/
/* CIRC */
/********/

Circ::Circ(string name, Point c, double radius) {
	Circ::name = name;
	_radius = radius;

	double incr = (sqrt(2) / 2) * radius;

	named_points.push_back(make_pair("c", c));
	named_points.push_back(make_pair("nw", Point(c.x - incr, c.y + incr)));
	named_points.push_back(make_pair("n", Point(c.x, c.y + radius)));
	named_points.push_back(make_pair("ne", Point(c.x + incr, c.y + incr)));
	named_points.push_back(make_pair("e", Point(c.x + radius, c.y)));
	named_points.push_back(make_pair("se", Point(c.x + incr, c.y - incr)));
	named_points.push_back(make_pair("s", Point(c.x, c.y - radius)));
	named_points.push_back(make_pair("sw", Point(c.x - incr, c.y - incr)));
	named_points.push_back(make_pair("w", Point(c.x - radius, c.y)));
}

bool Circ::contains(Point p) const {
	Point c = get_named_point("c");

	if((p.x - c.x) * (p.x - c.x) + (p.y - c.y) * (p.y - c.y) <= (_radius * _radius))
		return true;

	return false;
}

/********/
/* ELLI */
/********/

Elli::Elli(string name, Point c, double a, double b) {
	Elli::name = name;
	_a = a;
	_b = b;

	double incr = sqrt(2) / 2;
	double d_f = sqrt((a * a) - (b * b));

	named_points.push_back(make_pair("c", c));
	named_points.push_back(make_pair("nw", Point(c.x - a * incr, c.y + b * incr)));
	named_points.push_back(make_pair("n", Point(c.x, c.y + b)));
	named_points.push_back(make_pair("ne", Point(c.x + a * incr, c.y + b * incr)));
	named_points.push_back(make_pair("e", Point(c.x + a, c.y)));
	named_points.push_back(make_pair("se", Point(c.x + a * incr, c.y - b * incr)));
	named_points.push_back(make_pair("s", Point(c.x, c.y - b)));
	named_points.push_back(make_pair("sw", Point(c.x - a * incr, c.y - b * incr)));
	named_points.push_back(make_pair("w", Point(c.x - a, c.y)));
	named_points.push_back(make_pair("f1", Point(c.x + d_f, c.y)));
	named_points.push_back(make_pair("f2", Point(c.x - d_f, c.y)));
}

bool Elli::contains(Point p) const {
	Point c = get_named_point("c");

	if(((p.x - c.x) * (p.x - c.x)) / (_a * _a) + ((c.y - p.y) * (c.y - p.y)) / (_b * _b) <= 1.0)
		return true;

	return false;
}

/********/
/* RECT */
/********/

Rect::Rect(string name, Point c, double width, double height) {
	Rect::name = name;
	_width = width;
	_height = height;

	double mid_width = width / 2, mid_height = height / 2;

	named_points.push_back(make_pair("c", c));
	named_points.push_back(make_pair("nw", Point(c.x - mid_width, c.y + mid_height)));
	named_points.push_back(make_pair("n", Point(c.x, c.y + mid_height)));
	named_points.push_back(make_pair("ne", Point(c.x + mid_width, c.y + mid_height)));
	named_points.push_back(make_pair("e", Point(c.x + mid_width, c.y)));
	named_points.push_back(make_pair("se", Point(c.x + mid_width, c.y - mid_height)));
	named_points.push_back(make_pair("s", Point(c.x, c.y - mid_height)));
	named_points.push_back(make_pair("sw", Point(c.x - mid_width, c.y - mid_height)));
	named_points.push_back(make_pair("w", Point(c.x - mid_width, c.y)));
}

bool Rect::contains(Point p) const {
	vector<Point> vertices {get_named_point("nw"), get_named_point("ne"), get_named_point("se"), get_named_point("sw")};

	return contains_polygon(vertices, p);
}

/*******/
/* TRI */
/*******/

Tri::Tri(string name, Point v0, Point v1, Point v2) {
	Tri::name = name;
	
	named_points.push_back(make_pair("c", Point((v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3)));
	named_points.push_back(make_pair("v0", v0));
	named_points.push_back(make_pair("v1", v1));
	named_points.push_back(make_pair("v2", v2));
	named_points.push_back(make_pair("s01", Point((v0.x + v1.x) / 2, (v0.y + v1.y) / 2)));
	named_points.push_back(make_pair("s02", Point((v0.x + v2.x) / 2, (v0.y + v2.y) / 2)));
	named_points.push_back(make_pair("s12", Point((v1.x + v2.x) / 2, (v1.y + v2.y) / 2)));
}

bool Tri::contains(Point p) const {
	vector<Point> vertices {get_named_point("v0"), get_named_point("v1"), get_named_point("v2")};

	return contains_polygon(vertices, p);
}

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

Shift::Shift(string name, Point p, shared_ptr<Shape> ref_shape) {
	Shift::name = name;
	_p = p;
	_ref_shape = ref_shape;
}

Point Shift::get_named_point(string name) const {
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

Diff::Diff(string name, vector<shared_ptr<Shape>> shapes) {
	Diff::name = name;
	_shapes = shapes;
}

Point Diff::get_named_point(string name) const {
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
