#include <fstream>
#include <iostream>
#include <cstdint>
#include <cmath>

#include "headers/parser.hpp"

using namespace std;

/******************/
/* Public methods */
/******************/

Parser::Parser(const string fname) {
	if(fname.empty())
		print_error("Filename is empty.");

	Parser::filename = fname;
}

void Parser::parse_file() {
	convert_token();

	parse_size();
	parse_instr();
}

void Parser::print_stats() const {
	cout << "Number of shapes : " << shapes.size() << endl;
	cout << "Number of colors : " << colors.size() << endl;
	cout << "Number of fills : " << fills.size() << endl;
}

size_t Parser::get_width() const {
	return _width;
}

size_t Parser::get_height() const {
	return _height;
}

vector<shared_ptr<Shape>> Parser::get_shapes() const {
	return _shapes;
}

/*******************/
/* Private methods */
/*******************/

/****************************/
/* Token conversion methods */
/****************************/

token Parser::create_token(const unsigned int line, const unsigned int col, const string content, ifstream& file) const {
	unsigned int nb_char = 0, nb_digit = 0, nb_point = 0, nb_operator = 0;
	unsigned int content_length = content.length();
	token t = {line, col - content_length, 0, content};

	if(!content.empty()) {
		for(char c : content)
			if(isalpha(c) || c == '_')
				nb_char++;
			else if(isdigit(c))
				nb_digit++;
			else if(c == '.')
				nb_point++;
			else if(c == '+' || c == '-')
				nb_operator++;

		if(nb_operator == content_length && nb_operator == 1) {
			t.type = OPERATOR;
		} else if(nb_digit + nb_point + nb_operator == content_length && nb_point <= 1 && nb_operator <= 1 && nb_digit >= 1) {
			if(nb_operator == 0) {
				t.type = NUMBER;
			} else if(nb_operator == 1 && (content.at(0) == '+' || content.at(0) == '-')) {
				t.type = NUMBER;
			} else {
				file.close();

				print_error(t, "misplaced operator ('" + t.content + "').");
			}
		} else if(nb_char + nb_digit == content_length && isalpha(content.at(0))) {
			t.type = STRING;
		} else {
			file.close();

			print_error(t, "invalid element ('" + t.content + "').");
		}
	}

	return t;
}

token Parser::create_token(const unsigned int line, const unsigned int col, const unsigned int type, const string content) const {
	token t = {line, unsigned(col - content.length()), type, content};

	return t;
}

void Parser::push_token(const token t, string& buffer) {
	if(!buffer.empty()) {
		file_content.push_back(t);
		buffer.clear();
	}
}

void Parser::convert_token() {
	unsigned int line = 0, col = 0;
	string line_content, buffer;
	ifstream file;

	file.open(filename);

	if(!file)
		print_error("Unable to open file.");

	while(getline(file, line_content)) {
		push_token(create_token(line, col, buffer, file), buffer);

		line++;
		col = 0;

		for(char c : line_content) {
			col++;

			if(is_in(SPECIAL_CHARS, c)) {
				push_token(create_token(line, col, buffer, file), buffer);

				if(c == '#') {
					break;
				} else if(c == char(32)) {
					continue;
				} else {
					buffer = c;

					switch(c) {
						case '{': push_token(create_token(line, col, OPEN_BRACE, buffer), buffer); break;
						case '}': push_token(create_token(line, col, CLOSE_BRACE, buffer), buffer); break;
						case '(': push_token(create_token(line, col, OPEN_PAR, buffer), buffer); break;
						case ')': push_token(create_token(line, col, CLOSE_PAR, buffer), buffer); break;
						case '*':
						case '/': push_token(create_token(line, col, OPERATOR, buffer), buffer); break;
					}
				}
			} else if(c == '.') {
				token t = create_token(line, col, buffer, file);

				if(t.type == STRING) {
					push_token(t, buffer);

					buffer = c;
					push_token(create_token(line, col, POINT, buffer), buffer);
				} else if(t.type == NUMBER) {
					buffer += c;
				} else {
					if(!file_content.empty()) {
						token back = file_content.back();

						if(back.type == CLOSE_BRACE || back.type == CLOSE_PAR) {
							buffer = c;
							push_token(create_token(line, col, POINT, buffer), buffer);
						} else {
							buffer += c;
						}
					} else {
						buffer += c;
					}
				}
			} else if(is_in(SPECIAL_POINTS, c)) {
				if(buffer.back() == '.') {
					push_token(create_token(line, col, POINT, buffer), buffer);

					buffer = c;
					push_token(create_token(line, col, buffer, file), buffer);
				} else {
					buffer += c;
				}
			} else {
				buffer += c;
			}
		}
	}

	push_token(create_token(line, col, buffer, file), buffer);

	buffer = "END";
	push_token(create_token(++line, buffer.length(), END, buffer), buffer);

	file_content_size = file_content.size();

	file.close();
}

token Parser::next_token(const int& incr = 1) {
	if(file_content_pos >= file_content_size)
		print_error("Out of file.");

	token t = file_content.at(file_content_pos);
	file_content_pos += incr;

	return t;
}

/*******************/
/* Parsing methods */
/*******************/

void Parser::parse_instr() {
	token keyword = next_token();

	if(keyword.type != END) {
		string content = keyword.content;

		if(keyword.type != STRING)
			print_error(keyword, "invalid keyword format ('" + keyword.content + "').");

		if(content == "circ")
			parse_circ();
		else if(content == "elli")
			parse_elli();
		else if(content == "rect")
			parse_rect();
		else if(content == "tri")
			parse_tri();
		else if(content == "shift")
			parse_shift();
		else if(content == "rot")
			parse_rot();
		else if(content == "union")
			parse_union();
		else if(content == "diff")
			parse_diff();
		else if(content == "color")
			parse_color();
		else if(content == "fill")
			parse_fill();
		else
			print_error(keyword, "unknown keyword ('" + content + "').");
	}
}

void Parser::parse_size() {
	token keyword = next_token();

	if(keyword.content != "size")
		print_error(keyword, "expected 'size' keyword (got '" + keyword.content + "').");

	double intpart;

	_width = parse_number();
	_height = parse_number();

	if(!(_width >= 0.0 & _height >= 0.0 & modf(_width, &intpart) == 0.0 && modf(_height, &intpart) == 0.0))
		print_error("dimensions of the image must be positive integers.");
}

void Parser::parse_circ() {
	string name = parse_name(NEW_SHAPE);
	Point center = parse_point();
	double radius = parse_number();

	if(!(radius > 0.0))
		print_error("radius of circle must be positive.");

	_shapes.push_back(make_shared<Circ>(Circ(name, center, radius)));

	parse_instr();
}

void Parser::parse_elli() {
	string name = parse_name(NEW_SHAPE);
	Point center = parse_point();
	double a = parse_number();
	double b = parse_number();

	if(!(a > 0.0 && b > 0.0))
		print_error("radii of ellipse must be positive.");

	_shapes.push_back(make_shared<Elli>(Elli(name, center, a, b)));

	parse_instr();
}

void Parser::parse_rect() {
	string name = parse_name(NEW_SHAPE);
	Point center = parse_point();
	double width = parse_number();
	double height = parse_number();

	if(!(width > 0.0 && height > 0.0))
		print_error("dimensions of rectangle must be positive.");

	_shapes.push_back(make_shared<Rect>(Rect(name, center, width, height)));

	parse_instr();
}

void Parser::parse_tri() {
	string name = parse_name(NEW_SHAPE);
	Point v0 = parse_point();
	Point v1 = parse_point();
	Point v2 = parse_point();

	_shapes.push_back(make_shared<Tri>(Tri(name, v0, v1, v2)));

	parse_instr();
}

void Parser::parse_shift() {
	string name = parse_name(NEW_SHAPE);
	Point t = parse_point();
	string shift = parse_name(CHECK_SHAPE);

	size_t i;

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == shift)
			break;

	_shapes.push_back(make_shared<Shift>(Shift(name, t, _shapes.at(i))));

	parse_instr();
}

void Parser::parse_rot() {
	string name = parse_name(NEW_SHAPE);
	double angle = parse_number();
	Point p = parse_point();
	string rot = parse_name(CHECK_SHAPE);

	size_t i;

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == rot)
			break;

	_shapes.push_back(make_shared<Rot>(Rot(name, angle, p, _shapes.at(i))));

	parse_instr();
}

void Parser::parse_union() {
	vector<shared_ptr<Shape>> union_shapes;
	string temp;

	string name = parse_name(NEW_SHAPE);

	token t = next_token();

	if(t.type != OPEN_BRACE)
		print_error(t, "expected '{' (got '" + t.content + "').");

	temp = parse_name(CHECK_SHAPE);

	size_t i;

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == temp)
			break;

	union_shapes.push_back(_shapes.at(i));

	while(next_token(0).type == STRING) {
		temp = parse_name(CHECK_SHAPE);

		for(i = 0; i < _shapes.size(); i++)
			if(_shapes.at(i)->get_name() == temp)
				break;

		union_shapes.push_back(_shapes.at(i));
	}

	t = next_token();

	if(t.type != CLOSE_BRACE)
		print_error(t, "expected '}' (got '" + t.content + "')");

	_shapes.push_back(make_shared<Union>(Union(name, union_shapes)));

	parse_instr();
}

void Parser::parse_diff() {
	vector<shared_ptr<Shape>> diff_shapes;

	string name = parse_name(NEW_SHAPE);
	string base = parse_name(CHECK_SHAPE);
	string sub = parse_name(CHECK_SHAPE);

	size_t i;

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == base)
			break;

	diff_shapes.push_back(_shapes.at(i));

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == sub)
			break;

	diff_shapes.push_back(_shapes.at(i));

	_shapes.push_back(make_shared<Diff>(Diff(name, diff_shapes)));

	parse_instr();
}

Color Parser::parse_color_def() {
	Color c;
	double r, g, b;
	uint8_t red, green, blue;
	string name;

	token t = next_token(0);

	switch(t.type) {
		case OPEN_BRACE:
			t = next_token();

			r = parse_number();
			g = parse_number();
			b = parse_number();

			if(!(r >= 0.0 && r <= 1.0 && g >= 0.0 && g <= 1.0 && b >= 0.0 && b <= 1.0))
				print_error("color values must be between 0.0 and 1.0.");

			red = uint8_t(round(r * 255));
			green = uint8_t(round(g * 255));
			blue = uint8_t(round(b * 255));

			c = Color(red, green, blue);

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING:
			name = parse_name(CHECK_COLOR);

			size_t i;

			for(i = 0; i < _colors.size(); i++)
				if(_colors.at(i).first == name)
					break;

			c = _colors.at(i).second;

			break;

		default: print_error(t, "expected '{' or a color name (got '" + t.content + "').");
	}

	return c;
}

void Parser::parse_color() {
	string name = parse_name(NEW_COLOR);
	Color c = parse_color_def();

	_colors.push_back(make_pair(name, c));

	parse_instr();
}

void Parser::parse_fill() {
	string name = parse_name(NEW_FILL);
	Color c = parse_color_def();

	size_t i;

	for(i = 0; i < _shapes.size(); i++)
		if(_shapes.at(i)->get_name() == name)
			break;

	_shapes.at(i)->set_color(c);

	parse_instr();
}

string Parser::parse_name(const unsigned int& parse_name_type) {
	token name = next_token();
	token exist;

	if(name.type != STRING)
		print_error(name, "expected string type for name (got '" + name.content + "').");

	switch(parse_name_type) {
		case NEW_SHAPE:
			if(is_in(shapes, name, exist))
				print_error(name, "shape '" + name.content + "' already defined at " + to_string(exist.line) + ":" + to_string(exist.col) + ".");
			else
				shapes.push_back(name);

			break;

		case NEW_COLOR:
			if(is_in(colors, name, exist))
				print_error(name, "color '" + name.content + "' already defined at " + to_string(exist.line) + ":" + to_string(exist.col) + ".");
			else
				colors.push_back(name);

			break;

		case NEW_FILL:
			if(!is_in(shapes, name, exist))
				print_error(name, "shape '" + name.content + "' doesn't exist.");
			else
				fills.push_back(name);

			break;

		case CHECK_SHAPE:
			if(!is_in(shapes, name, exist))
				print_error(name, "shape '" + name.content + "' doesn't exist.");

			break;

		case CHECK_COLOR:
			if(!is_in(colors, name, exist))
				print_error(name, "color '" + name.content + "' doesn't exist.");

			break;

		default: print_error("parse_name : unknown operation.");
	}

	return name.content;
}

double Parser::parse_number() {
	token t = next_token(0);

	if(t.type != NUMBER) {
		Point p = parse_point();

		t = next_token();

		if(t.type != POINT)
			print_error(t, "expected '.' (got '" + t.content + "').");

		t = next_token();

		if(!is_in(SPECIAL_POINTS, t.content.at(0)))
			print_error(t, "'" + t.content + "' is not a valid coordinate.");

		if(t.content == "x")
			return p.x;
		else
			return p.y;
	} else {
		t = next_token();

		return ::atof(t.content.c_str());
	}
}

Point Parser::parse_point() {
	size_t i;
	double num, num1, num2;
	string name;
	Point p, tmp;

	token t = next_token(0);

	switch(t.type) {
		case OPEN_BRACE:
			t = next_token();

			num1 = parse_number();
			num2 = parse_number();

			p = Point(num1, num2);

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING:
			name = parse_name(CHECK_SHAPE);

			t = next_token();

			if(t.type != POINT)
				print_error(t, "expected '.' (got '" + t.content + "').");

			t = next_token();

			if(t.type != STRING)
				print_error(t, "expected a point name (got '" + t.content + "').");

			for(i = 0; i < _shapes.size(); i++)
				if(_shapes.at(i)->get_name() == name)
					break;

			p = _shapes.at(i)->get_named_point(t.content);

			break;

		case OPEN_PAR:
			t = next_token();
			t = next_token();

			if(t.type != OPERATOR)
				print_error(t, "expected an operator (+, -, * or /).");

			switch(t.content.at(0)) {
				case '+':
					do {
						tmp = p;
						p = tmp + parse_point();

						t = next_token(0);
					} while(t.type == OPEN_BRACE || t.type == STRING || t.type == OPEN_PAR);

					break;

				case '-':
					p = parse_point();
					t = next_token(0);

					while(t.type == OPEN_BRACE || t.type == STRING || t.type == OPEN_PAR) {
						tmp = parse_point();
						t = next_token(0);

						p = p - tmp;
					}
					
					break;

				case '*':
					tmp = parse_point();
					num = parse_number();

					p = tmp * num;

					break;

				case '/':
					tmp = parse_point();
					num = parse_number();

					p = tmp / num;

					break;

				default: print_error(t, "unknown operator.");
			}

			t = next_token();

			if(t.type != CLOSE_PAR)
				print_error(t, "expected ')' (got '" + t.content + "').");

			break;

		default: print_error(t, "expected a valid point definition.");
	}

	return p;
}

/***************************/
/* Error management method */
/***************************/

[[noreturn]] void Parser::print_error(const string& msg) const {
	cerr << msg << endl;

	exit(EXIT_FAILURE);
}

[[noreturn]] void Parser::print_error(const token& t, const string& msg) const {
	cerr << filename << ":" << t.line << ":" << t.col << ": error: " << msg << endl;

	exit(EXIT_FAILURE);
}

/*******************/
/* Utility methods */
/*******************/

bool Parser::is_in(const vector<char>& v, const char& c) const {
	for(char v_c : v)
		if(v_c == c)
			return true;

	return false;
}

bool Parser::is_in(const vector<token>& v, const token& t, token& exist) const {
	for(token v_t : v)
		if(v_t.content == t.content) {
			exist = v_t;

			return true;
		}

	return false;
}
