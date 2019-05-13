#include "headers/geometry.hpp"

using namespace std;

/* Basic structure */

/*********/
/* POINT */
/*********/

Point Point::shift(Point p) const {
	return Point(x + p.x, y + p.y);
}

Point Point::rotate(Point p, double angle) const {
	double d_x = x - p.x, d_y = y - p.y;
	double sin_a, cos_a;

	angle *= (M_PI / 180.0);
	sin_a = sin(angle);
	cos_a = cos(angle);

	return Point(d_x * cos_a - d_y * sin_a + p.x, d_x * sin_a + d_y * cos_a + p.y);
}

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

domain Shape::get_domain(vector<Point> vertices) const {
	Point lb = vertices.at(0), ur = vertices.at(0);

	for(auto e = vertices.cbegin() + 1; e != vertices.cend(); e++) {
		if((*e).x < lb.x)
			lb.x = (*e).x;

		if((*e).x > ur.x)
			ur.x = (*e).x;

		if((*e).y < lb.y)
			lb.y = (*e).y;

		if((*e).y > ur.y)
			ur.y = (*e).y;
	}

	return {lb, ur};
}

/* Primitive shapes */

/********/
/* ELLI */
/********/

Point Elli::get_named_point(string name) const {
	double incr = M_SQRT2 / 2;
	double d_f = sqrt(pow(a, 2) - pow(b, 2));

	if(name == "c")
		return c;
	else if(name == "nw")
		return Point(c.x - a * incr, c.y + b * incr);
	else if(name == "n")
		return Point(c.x, c.y + b);
	else if(name == "ne")
		return Point(c.x + a * incr, c.y + b * incr);
	else if(name == "e")
		return Point(c.x + a, c.y);
	else if(name == "se")
		return Point(c.x + a * incr, c.y - b * incr);
	else if(name == "s")
		return Point(c.x, c.y - b);
	else if(name == "sw")
		return Point(c.x - a * incr, c.y - b * incr);
	else if(name == "w")
		return Point(c.x - a, c.y);
	else if(name == "f1")
		return Point(c.x + d_f, c.y);
	else if(name == "f2")
		return Point(c.x - d_f, c.y);
	else
		throw invalid_argument("Point doesn't exist.");
}

domain Elli::get_domain() const {
	return {Point(c.x - a, c.y - b), Point(c.x + a, c.y + b)};
}

bool Elli::contains(Point p) const {
	return pow((p.x - c.x) * b, 2) + pow((c.y - p.y) * a, 2) <= ab2;
}

/********/
/* CIRC */
/********/

Point Circ::get_named_point(string name) const {
	if(name == "f1" || name == "f2")
		throw invalid_argument("Point doesn't exist.");

	return Elli::get_named_point(name);
}

bool Circ::contains(Point p) const {
	return pow(p.x - c.x, 2) + pow(p.y - c.y, 2) <= a2;
}

/********/
/* RECT */
/********/

Point Rect::get_named_point(string name) const {
	if(name == "c")
		return c;
	else if(name == "nw")
		return Point(c.x - mid_width, c.y + mid_height);
	else if(name == "n")
		return Point(c.x, c.y + mid_height);
	else if(name == "ne")
		return Point(c.x + mid_width, c.y + mid_height);
	else if(name == "e")
		return Point(c.x + mid_width, c.y);
	else if(name == "se")
		return Point(c.x + mid_width, c.y - mid_height);
	else if(name == "s")
		return Point(c.x, c.y - mid_height);
	else if(name == "sw")
		return Point(c.x - mid_width, c.y - mid_height);
	else if(name == "w")
		return Point(c.x - mid_width, c.y);
	else
		throw invalid_argument("Point doesn't exist.");
}

domain Rect::get_domain() const {
	return {get_named_point("sw"), get_named_point("ne")};
}

bool Rect::contains(Point p) const {
	return abs(p.x - c.x) <= mid_width && abs(p.y - c.y) <= mid_height;
}

/*******/
/* TRI */
/*******/

Point Tri::get_named_point(string name) const {
	if(name == "c")
		return Point((v0.x + v1.x + v2.x) / 3, (v0.y + v1.y + v2.y) / 3);
	else if(name == "v0")
		return v0;
	else if(name == "v1")
		return v1;
	else if(name == "v2")
		return v2;
	else if(name == "s01")
		return Point((v0.x + v1.x) / 2, (v0.y + v1.y) / 2);
	else if(name == "s02")
		return Point((v0.x + v2.x) / 2, (v0.y + v2.y) / 2);
	else if(name == "s12")
		return Point((v1.x + v2.x) / 2, (v1.y + v2.y) / 2);
	else
		throw invalid_argument("Point doesn't exist.");
}

domain Tri::get_domain() const {
	vector<Point> vertices{v0, v1, v2};

	return Shape::get_domain(vertices);
}

bool Tri::contains(Point p) const {
	double d1 = (p.x - v1.x) * (v0.y - v1.y) - (v0.x - v1.x) * (p.y - v1.y);
	double d2 = (p.x - v2.x) * (v1.y - v2.y) - (v1.x - v2.x) * (p.y - v2.y);
	double d3 = (p.x - v0.x) * (v2.y - v0.y) - (v2.x - v0.x) * (p.y - v0.y);

	return !(((d1 < 0) || (d2 < 0) || (d3 < 0)) && ((d1 > 0) || (d2 > 0) || (d3 > 0)));
}

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

Point Shift::get_named_point(string name) const {
	return (ref_shape->get_named_point(name)).shift(t);
}

domain Shift::get_domain() const {
	domain ref_dom = ref_shape->get_domain();

	return {ref_dom[0].shift(t), ref_dom[1].shift(t)};
}

bool Shift::contains(Point p) const {
	return ref_shape->contains(p.shift(t_inv));
}

/*******/
/* ROT */
/*******/

Point Rot::get_named_point(string name) const {
	return (ref_shape->get_named_point(name)).rotate(r, angle);
}

domain Rot::get_domain() const {
	domain ref_dom = ref_shape->get_domain();
	vector<Point> vertices;

	Point v0(ref_dom[0].x, ref_dom[1].y);
	Point v1(ref_dom[1].x, ref_dom[0].y);
	Point v2(ref_dom[0].x, ref_dom[0].y);
	Point v3(ref_dom[1].x, ref_dom[1].y);

	vertices.push_back(v0.rotate(r, angle));
	vertices.push_back(v1.rotate(r, angle));
	vertices.push_back(v2.rotate(r, angle));
	vertices.push_back(v3.rotate(r, angle));

	return Shape::get_domain(vertices);
}

bool Rot::contains(Point p) const {
	return ref_shape->contains(p.rotate(r, - angle));
}

/*********/
/* UNION */
/*********/

Point Union::get_named_point(string name) const {
	return shapes.at(0)->get_named_point(name);
}

domain Union::get_domain() const {
	domain dom = shapes.at(0)->get_domain();
	Point lb = dom[0], ur = dom[1];

	for(auto e = shapes.cbegin() + 1; e != shapes.cend(); e++) {
		dom = (*e)->get_domain();

		if(dom[0].x < lb.x)
			lb.x = dom[0].x;

		if(dom[0].y < lb.y)
			lb.y = dom[0].y;

		if(dom[1].x > ur.x)
			ur.x = dom[1].x;

		if(dom[1].y > ur.y)
			ur.y = dom[1].y;
	}

	return {lb, ur};
}

bool Union::contains(Point p) const {
	for(const auto& shape : shapes)
		if(shape->contains(p))
			return true;

	return false;
}

/********/
/* DIFF */
/********/

Point Diff::get_named_point(string name) const {
	return shape_in->get_named_point(name);
}

domain Diff::get_domain() const {
	return shape_in->get_domain();
}

bool Diff::contains(Point p) const {
	return shape_in->contains(p) && !shape_out->contains(p);
}
