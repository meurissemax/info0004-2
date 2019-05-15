/**
 * Object-oriented programming project - Project 3
 * Drawing geometric figures
 *
 * @author Maxime Meurisse (m.meurisse@student.uliege.be) - 20161278
 * @version 2019.05.15
 */

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
	Point(double _x, double _y) : x(_x), y(_y) { }

	/**
	 * The following operators work component by component.
	 *
	 * For example, the addition of two points will result in the addition of
	 * their respective coordinates.
	 */
	void operator +=(Point p) { x += p.x; y += p.y; }
	void operator -=(Point p) { x -= p.x; y -= p.y; }
	void operator *=(double n) { x *= n; y *= n; }
	void operator /=(double n) { x /= n; y /= n; }

	/**
	 * Create a translated point of this point according to point 'p'.
	 *
	 * The coordinates of the created point will be given by
	 *		(x + p.x, y + p.y)
	 *
	 * @param p the translation point
	 * @return a translated point of this point
	 */
	Point shift(Point p) const;

	/**
	 * Create a rotated point of this point around point 'p' of
	 * an angle 'angle' (in degree).
	 *
	 * @param p the center of rotation
	 * @param angle the angle of rotation (in degree)
	 * @return a rotated point of this point
	 */
	Point rotate(Point p, double angle) const;

	/// Coordinates x and y of the point
	double x, y;
};

/* Abstract shape */

/*********/
/* SHAPE */
/*********/

/**
 * The domain of a shape is represented by a rectangular area.
 * This is represented by the points SW and NE of the rectangle.
 */
typedef std::array<Point, 2> domain;

class Shape {
public:
	Shape() : color(Color()) { }
	virtual ~Shape() = default;

	void set_color(Color c) { color = c; }
	Color get_color() const { return color; }

	/**
	 * Return the named point 'name' of the shape.
	 * All named points are defined in the project statement.
	 *
	 * This function is pure virtual and is redefined in all children
	 * of this class.
	 *
	 * @param name the name of the named point
	 * @return a 'Point' representing the named point
	 */
	virtual Point get_named_point(std::string name) const = 0;

	/**
	 * Return the domain of the shape, according to the representation
	 * of a domain.
	 *
	 * This function is pure virtual and is redefined in all children
	 * of this class.
	 *
	 * @return domain the domain of the shape
	 */
	virtual domain get_domain() const = 0;

	/**
	 * Return the domain of the shape according to his vertices.
	 * This function is mainly usde by polygons.
	 *
	 * @param vertices a vector of point containing all vertices of the shape
	 * @return domain the domain of the shape
	 */
	domain get_domain(std::vector<Point> vertices) const;

	/**
	 * Returns a boolean indicating whether a point is in the shape or not.
	 *
	 * This function is pure virtual and is redefined in all children
	 * of this class.
	 *
	 * @param p the point to check
	 * @return a boolean value indicating if the point is inside the shape or not
	 */
	virtual bool contains(Point p) const = 0;

protected:
	/// The color of the shape (by default, default value of Color)
	Color color;
};

/* Primitive shapes */

/**
 * All the following classes have a private constructor and
 * are friend with 'Parser' class.
 *
 * Only the 'Parser' class can instantiate shapes.
 *
 * There is therefore no need to check the validity of the arguments
 * of each method because the 'Parser' class is already taking care of it.
 */

/********/
/* ELLI */
/********/

class Elli : public Shape {
public:
	virtual Point get_named_point(std::string name) const override;
	domain get_domain() const override;
	virtual bool contains(Point p) const override;

protected:
	Elli(Point _c, double _a, double _b) : c(_c), a(_a), b(_b), a2(pow(_a, 2)), ab2(pow(_a * _b, 2)) { }
	friend class Parser;

	/// Center point
	Point c;

	/// Semi-major radius (a) and semi-minor radius (b)
	/// a2 and ab2 are precalculated values
	double a, b, a2, ab2;
};

/********/
/* CIRC */
/********/

/**
 * A 'Circ' is a child of 'Elli' simply because a 'Circ' is an 'Elli'
 * with semi-major radius = semi-minor radius.
 */

class Circ : public Elli {
public:
	Point get_named_point(std::string name) const override;
	bool contains(Point p) const override;

private:
	Circ(Point _c, double _radius) : Elli(_c, _radius, _radius) { }
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
	Rect(Point _c, double _width, double _height) : c(_c), width(_width), height(_height), mid_width(_width / 2), mid_height(_height / 2) { }
	friend class Parser;

	/// Center point
	Point c;

	/// mid_width and mid_height are precalculated values
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
	Tri(Point _v0, Point _v1, Point _v2) : v0(_v0), v1(_v1), v2(_v2) { }
	friend class Parser;

	/// The three vertices of the triangle (no matter the order)
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
	Shift(Point _t, std::shared_ptr<Shape> _ref_shape) : t(_t), t_inv(Point(- _t.x, - _t.y)), ref_shape(_ref_shape) { }
	friend class Parser;

	/// Point of translation (t) and his inverse (t_inv)
	/// Inverse point lead to the inverse translation
	Point t, t_inv;

	/// A reference to the shape to translate
	/// Note : the original shape will be never translated.
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
	Rot(double _angle, Point _r, std::shared_ptr<Shape> _ref_shape) : angle(_angle), r(_r), ref_shape(_ref_shape) { }
	friend class Parser;

	/// The angle of rotation
	double angle;

	/// The center of rotation
	Point r;

	/// A reference to the shape to rotate
	/// Note : the original shape will be never rotated.
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
	Union(std::vector<std::shared_ptr<Shape>> _shapes) : shapes(_shapes) { }
	friend class Parser;

	/// The vector of references to all shapes composing the union
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
	Diff(std::shared_ptr<Shape> _shape_in, std::shared_ptr<Shape> _shape_out) : shape_in(_shape_in), shape_out(_shape_out) { }
	friend class Parser;

	/// Reference to the shape to be substracted from (shape_in)
	/// Reference to the shape to be substracted (shape_out)
	std::shared_ptr<Shape> shape_in, shape_out;
};

#endif
