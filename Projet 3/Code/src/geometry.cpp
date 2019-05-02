#include <cmath>
#include <cstdint>
#include <cassert>
#include <limits>
#include <utility>

#include "headers/geometry.hpp"

using namespace std;

/* Basic structure */

/*********/
/* POINT */
/*********/

void Point::shift(Point p) {
	x = x + p.x;
	y = y + p.y;
}

void Point::rotate(Point p, float angle) {
	float tmp_x = x;

	angle = angle * M_PI / 180.0;

	x = (x - p.x) * cos(angle) - (y - p.y) * sin(angle) + p.x;
	y = (tmp_x - p.x) * sin(angle) + (y - p.y) * cos(angle) + p.y;
}

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

bool Shape::is_fill() const {
	return fill;
}

void Shape::set_color(float r, float g, float b) {
	uint8_t red, green, blue;

	red = uint8_t(round(r * 255));
	green = uint8_t(round(g * 255));
	blue = uint8_t(round(b * 255));

	color = Color(red, green, blue);
	fill = true;
}

Color Shape::get_color() const {
	assert(fill);

	return color;
}

Point Shape::get_named_point(string name) const {
	auto e = named_points.find(name);

	assert(e != named_points.end());

	return e->second;
}

void Shape::shift(Point p) {
	for(auto& named_point : named_points)
		named_point.second.shift(p);
}

void Shape::rotate(Point p, float angle) {
	for(auto& named_point : named_points)
		named_point.second.rotate(p, angle);
}

bool Shape::is_inside(Point polygon[], int n, Point p) const { 
	if(n < 3)
		return false;

	Point extreme(numeric_limits<float>::max(), p.y);

	int count = 0, i = 0;

	do {
		int next = (i + 1) % n;

		if(do_intersect(polygon[i], polygon[next], p, extreme)) {
			if(orientation(polygon[i], p, polygon[next]) == 0)
				return on_segment(polygon[i], p, polygon[next]);

			count++;
		}

		i = next;
	} while(i != 0);

	return (count % 2 == 1);
}

bool Shape::on_segment(Point p, Point q, Point r) const {
	if(q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) && q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true; 

	return false; 
}

int Shape::orientation(Point p, Point q, Point r) const {
	float val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

	if(val == 0.0)
		return 0;

	return (val > 0.0) ? 1 : 2;
}

bool Shape::do_intersect(Point p1, Point q1, Point p2, Point q2) const { 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if(o1 != o2 && o3 != o4)
		return true; 

	if(o1 == 0 && on_segment(p1, p2, q1))
		return true; 

	if(o2 == 0 && on_segment(p1, q2, q1))
		return true; 

	if(o3 == 0 && on_segment(p2, p1, q2))
		return true; 

	if(o4 == 0 && on_segment(p2, q1, q2))
		return true; 

	return false;
}

/* Primitive shapes */

/********/
/* CIRC */
/********/

Circ::Circ(string name, Point c, float radius) {
	Circ::name = name;
	_radius = radius;

	float incr = (sqrt(2) / 2) * radius;

	named_points.insert(pair<string, Point>("c", c));
	named_points.insert(pair<string, Point>("nw", Point(c.x - incr, c.y + incr)));
	named_points.insert(pair<string, Point>("n", Point(c.x, c.y + radius)));
	named_points.insert(pair<string, Point>("ne", Point(c.x + incr, c.y + incr)));
	named_points.insert(pair<string, Point>("e", Point(c.x + radius, c.y)));
	named_points.insert(pair<string, Point>("se", Point(c.x + incr, c.y - incr)));
	named_points.insert(pair<string, Point>("s", Point(c.x, c.y - radius)));
	named_points.insert(pair<string, Point>("sw", Point(c.x - incr, c.y - incr)));
	named_points.insert(pair<string, Point>("w", Point(c.x - radius, c.y)));
}

bool Circ::is_in(Point p) const {
	Point c = get_named_point("c");

	if(pow(p.x - c.x, 2) + pow(p.y - c.y, 2) <= pow(_radius, 2))
		return true;

	return false;
}

/********/
/* ELLI */
/********/

Elli::Elli(string name, Point c, float a, float b) {
	Elli::name = name;
	_a = a;
	_b = b;
	_angle = 0;

	float incr = sqrt(2) / 2;
	float d_f = sqrt(pow(a, 2) - pow(b, 2));

	named_points.insert(pair<string, Point>("c", c));
	named_points.insert(pair<string, Point>("nw", Point(c.x - a * incr, c.y + b * incr)));
	named_points.insert(pair<string, Point>("n", Point(c.x, c.y + b)));
	named_points.insert(pair<string, Point>("ne", Point(c.x + a * incr, c.y + b * incr)));
	named_points.insert(pair<string, Point>("e", Point(c.x + a, c.y)));
	named_points.insert(pair<string, Point>("se", Point(c.x + a * incr, c.y - b * incr)));
	named_points.insert(pair<string, Point>("s", Point(c.x, c.y - b)));
	named_points.insert(pair<string, Point>("sw", Point(c.x - a * incr, c.y - b * incr)));
	named_points.insert(pair<string, Point>("w", Point(c.x - a, c.y)));
	named_points.insert(pair<string, Point>("f1", Point(c.x + d_f, c.y)));
	named_points.insert(pair<string, Point>("f2", Point(c.x - d_f, c.y)));
}

void Elli::rotate(Point p, float angle) {
	_angle = angle * M_PI / 180.0;

	Shape::rotate(p, angle);
}

bool Elli::is_in(Point p) const {
	Point c = get_named_point("c");

	float sin_a = sin(_angle), cos_a = cos(_angle);

	float num_1 = pow((p.x - c.x) * cos_a + (p.y - c.y) * sin_a, 2);
	float num_2 = pow((p.x - c.x) * sin_a - (p.y - c.y) * cos_a, 2);

	if(num_1 / pow(_a, 2) + num_2 / pow(_b, 2) <= 1.0)
		return true;

	return false;
}

/********/
/* RECT */
/********/

Rect::Rect(string name, Point c, float width, float height) {
	Rect::name = name;
	_width = width;
	_height = height;

	float mid_width = width / 2, mid_height = height / 2;

	named_points.insert(pair<string, Point>("c", c));
	named_points.insert(pair<string, Point>("nw", Point(c.x - mid_width, c.y + mid_height)));
	named_points.insert(pair<string, Point>("n", Point(c.x, c.y + mid_height)));
	named_points.insert(pair<string, Point>("ne", Point(c.x + mid_width, c.y + mid_height)));
	named_points.insert(pair<string, Point>("e", Point(c.x + mid_width, c.y)));
	named_points.insert(pair<string, Point>("se", Point(c.x + mid_width, c.y - mid_height)));
	named_points.insert(pair<string, Point>("s", Point(c.x, c.y - mid_height)));
	named_points.insert(pair<string, Point>("sw", Point(c.x - mid_width, c.y - mid_height)));
	named_points.insert(pair<string, Point>("w", Point(c.x - mid_width, c.y)));
}

bool Rect::is_in(Point p) const {
	Point polygon[] = {get_named_point("nw"), get_named_point("ne"), get_named_point("se"), get_named_point("sw")};

	return is_inside(polygon, 4, p);
}

/*******/
/* TRI */
/*******/

Tri::Tri(string name, Point v0, Point v1, Point v2) {
	Tri::name = name;
	
	named_points.insert(pair<string, Point>("c", Point((v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3)));
	named_points.insert(pair<string, Point>("v0", v0));
	named_points.insert(pair<string, Point>("v1", v1));
	named_points.insert(pair<string, Point>("v2", v2));
	named_points.insert(pair<string, Point>("s01", Point((v0.x + v1.x) / 2, (v0.y + v1.y) / 2)));
	named_points.insert(pair<string, Point>("s02", Point((v0.x + v2.x) / 2, (v0.y + v2.y) / 2)));
	named_points.insert(pair<string, Point>("s12", Point((v1.x + v2.x) / 2, (v1.y + v2.y) / 2)));
}

bool Tri::is_in(Point p) const {
	Point polygon[] = {get_named_point("v0"), get_named_point("v1"), get_named_point("v2")};

	return is_inside(polygon, 3, p);
}
