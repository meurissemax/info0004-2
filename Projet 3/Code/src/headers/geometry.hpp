#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <array>
#include <string>
#include <cmath>
#include <memory>
#include <vector>

#include "graphics.hpp"

/* Basic structure */

/*********/
/* POINT */
/*********/

struct Point {
	Point() : x(0), y(0) { }
	Point(double x, double y) : x(x), y(y) { }

	void operator +=(Point p) { x += p.x; y += p.y; }
	void operator -=(Point p) { x -= p.x; y -= p.y; }
	void operator *=(double n) { x *= n; y *= n; }
	void operator /=(double n) { x /= n; y /= n; }

	Point shift(Point p) const;
	Point rotate(Point p, double angle) const;

	double x, y;
};

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

typedef std::array<Point, 2> domain;

class Shape {
public:
	Shape() : color(Color()) { }
	virtual ~Shape() = default;

	void set_color(Color c) { color = c; }
	Color get_color() const { return color; }

	virtual Point get_named_point(std::string name) const = 0;
	virtual domain get_domain() const = 0;
	domain get_domain(std::vector<Point> vertices) const;
	virtual bool contains(Point p) const = 0;

protected:
	Color color;
};

/* Primitive shapes */

/********/
/* ELLI */
/********/

class Elli : public Shape {
public:
	virtual Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	virtual bool contains(Point p) const override;

protected:
	Elli(Point c, double a, double b) : c(c), a(a), b(b), a2(pow(a, 2)), ab2(pow(a * b, 2)) { }
	friend class Parser;

	Point c;
	double a, b, a2, ab2;
};

/********/
/* CIRC */
/********/

class Circ : public Elli {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Circ(Point c, double radius) : Elli(c, radius, radius) { }
	friend class Parser;
};

/********/
/* RECT */
/********/

class Rect : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Rect(Point c, double width, double height) : c(c), width(width), height(height), mid_width(width / 2), mid_height(height / 2) { }
	friend class Parser;

	Point c;
	double width, height, mid_width, mid_height;
};

/*******/
/* TRI */
/*******/

class Tri : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Tri(Point v0, Point v1, Point v2) : v0(v0), v1(v1), v2(v2) { }
	friend class Parser;

	Point v0, v1, v2;
};

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

class Shift : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Shift(Point t, std::shared_ptr<Shape> ref_shape) : t(t), t_inv(Point(- t.x, - t.y)), ref_shape(ref_shape) { }
	friend class Parser;

	Point t, t_inv;
	std::shared_ptr<Shape> ref_shape;
};

/*******/
/* ROT */
/*******/

class Rot : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Rot(double angle, Point r, std::shared_ptr<Shape> ref_shape) : angle(angle), r(r), ref_shape(ref_shape) { }
	friend class Parser;

	double angle;
	Point r;
	std::shared_ptr<Shape> ref_shape;
};

/*********/
/* UNION */
/*********/

class Union : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Union(std::vector<std::shared_ptr<Shape>> shapes) : shapes(shapes) { }
	friend class Parser;

	std::vector<std::shared_ptr<Shape>> shapes;
};

/********/
/* DIFF */
/********/

class Diff : public Shape {
public:
	Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	bool contains(Point p) const override;

private:
	Diff(std::shared_ptr<Shape> shape_in, std::shared_ptr<Shape> shape_out) : shape_in(shape_in), shape_out(shape_out) { }
	friend class Parser;

	std::shared_ptr<Shape> shape_in, shape_out;
};

#endif
