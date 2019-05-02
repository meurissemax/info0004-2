#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <string>
#include <map>

#include "graphics.hpp"

/* Basic structure */

/*********/
/* POINT */
/*********/

struct Point {
	Point() : x(0), y(0) { }
	Point(float _x, float _y) : x(_x), y(_y) { }

	void shift(Point p);
	void rotate(Point p, float angle);

	float x, y;
};

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

class Shape {
public:
	Shape() : fill(false) { }
	virtual ~Shape() = default;

	bool is_fill() const;

	void set_color(float r, float g, float b);
	Color get_color() const;

	Point get_named_point(std::string name) const;

	virtual void shift(Point p);
	virtual void rotate(Point p, float angle);

	virtual bool is_in(Point p) const = 0;

protected:
	std::string name;

	bool fill;
	Color color;

	std::map<std::string, Point> named_points;

	bool is_inside(Point polygon[], int n, Point p) const;

private:
	bool on_segment(Point p, Point q, Point r) const;
	int orientation(Point p, Point q, Point r) const;
	bool do_intersect(Point p1, Point q1, Point p2, Point q2) const;
};

/* Primitive shapes */

/********/
/* CIRC */
/********/

class Circ : public Shape {
public:
	Circ(std::string name, Point c, float radius);

	bool is_in(Point p) const override;

private:
	float _radius;
};

/********/
/* ELLI */
/********/

class Elli : public Shape {
public:
	Elli(std::string name, Point c, float a, float b);

	void rotate(Point p, float angle) override;
	bool is_in(Point p) const override;

private:
	float _a, _b;
	float _angle;
};

/********/
/* RECT */
/********/

class Rect : public Shape {
public:
	Rect(std::string name, Point c, float width, float height);

	bool is_in(Point p) const override;

private:
	float _width, _height;
};

/*******/
/* TRI */
/*******/

class Tri : public Shape {
public:
	Tri(std::string name, Point v0, Point v1, Point v2);

	bool is_in(Point p) const override;
};

/* Derived shapes */

/*********/
/* SHIFT */
/*********/

/*******/
/* ROT */
/*******/

/*********/
/* UNION */
/*********/

/********/
/* DIFF */
/********/

#endif
