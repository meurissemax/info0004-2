#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <string>
#include <vector>
#include <utility>
#include <memory>

#include "graphics.hpp"

/* Basic structure */

/*********/
/* POINT */
/*********/

struct Point {
	Point() : x(0), y(0) { }
	Point(double _x, double _y) : x(_x), y(_y) { }

	Point operator+(Point p) const;
	Point operator-(Point p) const;
	Point operator*(double n) const;
	Point operator/(double n) const;

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

	std::string get_name() const;

	bool is_fill() const;
	void set_color(Color c);
	Color get_color() const;

	virtual Point get_named_point(std::string name) const;

	virtual bool contains(Point p) const = 0;

protected:
	std::string name;

	bool fill;
	Color color;

	std::vector<std::pair<std::string, Point>> named_points;

	bool contains_polygon(std::vector<Point> vertices, Point p) const;
};

/* Primitive shapes */

/********/
/* CIRC */
/********/

class Circ : public Shape {
public:
	bool contains(Point p) const override;

private:
	Circ(std::string name, Point c, double radius);
	friend class Parser;

	double _radius;
};

/********/
/* ELLI */
/********/

class Elli : public Shape {
public:
	bool contains(Point p) const override;

private:
	Elli(std::string name, Point c, double a, double b);
	friend class Parser;

	double _a, _b;
};

/********/
/* RECT */
/********/

class Rect : public Shape {
public:
	bool contains(Point p) const override;

private:
	Rect(std::string name, Point c, double width, double height);
	friend class Parser;

	double _width, _height;
};

/*******/
/* TRI */
/*******/

class Tri : public Shape {
public:
	bool contains(Point p) const override;

private:
	Tri(std::string name, Point v0, Point v1, Point v2);
	friend class Parser;
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

	Point _p;
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
	Diff(std::string name, std::vector<std::shared_ptr<Shape>> shapes);
	friend class Parser;

	std::vector<std::shared_ptr<Shape>> _shapes;
};

#endif
