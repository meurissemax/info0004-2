#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <string>
#include <vector>
#include <memory>

#include "graphics.hpp"

/* Basic structure */

/*********/
/* POINT */
/*********/

struct Point {
	Point() : x(0), y(0) { }
	Point(double _x, double _y) : x(_x), y(_y) { }

	Point operator+(Point p) const { return Point(x + p.x, y + p.y); }
	Point operator-(Point p) const { return Point(x - p.x, y - p.y); }
	Point operator*(double n) const { return Point(x * n, y * n); }
	Point operator/(double n) const { return Point(x / n, y / n); }

	void shift(Point p);
	void rotate(Point p, double angle);

	double x, y;
};

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

class Shape {
public:
	Shape() : fill(false) { }
	virtual ~Shape() = default;

	std::string get_name() const { return name; }

	bool is_fill() const { return fill; }
	void set_color(Color c);
	Color get_color() const;

	virtual Point get_named_point(std::string name) const = 0;
	virtual bool contains(Point p) const = 0;

protected:
	std::string name;

	bool fill;
	Color color;
};

/* Primitive shapes */

/********/
/* ELLI */
/********/

class Elli : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Elli(std::string name, Point c, double a, double b);
	friend class Parser;

	Point _c;
	double _a, _b;
};

/********/
/* RECT */
/********/

class Rect : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Rect(std::string name, Point c, double width, double height);
	friend class Parser;

	Point _c;
	double _width, _height, _mid_width, _mid_height;
};

/*******/
/* TRI */
/*******/

class Tri : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Tri(std::string name, Point v0, Point v1, Point v2);
	friend class Parser;

	Point _v0, _v1, _v2;
};

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

class Shift : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Shift(std::string name, Point p, std::shared_ptr<Shape> ref_shape);
	friend class Parser;

	Point _p, _p_inv;
	std::shared_ptr<Shape> _ref_shape;
};

/*******/
/* ROT */
/*******/

class Rot : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Rot(std::string name, double angle, Point p, std::shared_ptr<Shape> ref_shape);
	friend class Parser;

	double _angle;
	Point _p;
	std::shared_ptr<Shape> _ref_shape;
};

/*********/
/* UNION */
/*********/

class Union : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Union(std::string name, std::vector<std::shared_ptr<Shape>> shapes);
	friend class Parser;

	std::vector<std::shared_ptr<Shape>> _shapes;
};

/********/
/* DIFF */
/********/

class Diff : public Shape {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Diff(std::string name, std::shared_ptr<Shape> shape_from, std::shared_ptr<Shape> shape_to);
	friend class Parser;

	std::shared_ptr<Shape> _shape_from, _shape_to;
};

#endif
