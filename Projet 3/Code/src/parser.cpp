/**
 * Object-oriented programming project - Project 3
 * Drawing geometric figures
 *
 * @author Maxime Meurisse (m.meurisse@student.uliege.be) - 20161278
 * @version 2019.05.15
 */

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdint>
#include <cstdlib>

#include "headers/parser.hpp"

using namespace std;

/* Public methods */

[[noreturn]] Parser::Parser() {
	cerr << "A file is required to create a parser." << endl;

	exit(EXIT_FAILURE);
}

Parser::Parser(string fname) {
	if(fname.empty()) {
		cerr << "Filename is empty." << endl;

		exit(EXIT_FAILURE);
	}

	filename = fname;
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

/* Private methods */

/****************************/
/* Token conversion methods */
/****************************/

token Parser::create_token(unsigned long line, unsigned long col, string content, ifstream& file) const {
	unsigned long nb_char = 0, nb_digit = 0, nb_point = 0, nb_operator = 0;
	unsigned long content_length = content.length();
	token t = {line, col - content_length, 0, content};

	if(!content.empty()) {
		/// We look at the type of each character of the content.
		for(char c : content)
			if(isalpha(c) || c == '_')
				nb_char++;
			else if(isdigit(c))
				nb_digit++;
			else if(c == '.')
				nb_point++;
			else if(c == '+' || c == '-')
				nb_operator++;

		/// Depending of the character types, we determine the type of the token.
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

token Parser::create_token(unsigned long line, unsigned long col, unsigned int type, string content) const {
	token t = {line, col - content.length(), type, content};

	return t;
}

void Parser::push_token(token t, string& buffer) {
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

	if(!file) {
		cerr << "Unable to open file." << endl;

		exit(EXIT_FAILURE); 
	}

	/// We read file line by line.
	while(getline(file, line_content)) {
		push_token(create_token(line, col, buffer, file), buffer);

		line++;
		col = 0;

		/// We read all the character on a line.
		for(char c : line_content) {
			col++;

			if(c == '#') { /// comment
				push_token(create_token(line, col, buffer, file), buffer);

				break;
			} else if(c == char(32)) { /// space
				push_token(create_token(line, col, buffer, file), buffer);

				continue;
			} else if(is_in(SPECIAL_CHARS, c)) { /// specials chars
				push_token(create_token(line, col, buffer, file), buffer);

				buffer = c;

				switch(c) {
					case '{': push_token(create_token(line, col, OPEN_BRACE, buffer), buffer); break;
					case '}': push_token(create_token(line, col, CLOSE_BRACE, buffer), buffer); break;
					case '(': push_token(create_token(line, col, OPEN_PAR, buffer), buffer); break;
					case ')': push_token(create_token(line, col, CLOSE_PAR, buffer), buffer); break;
					case '*':
					case '/': push_token(create_token(line, col, OPERATOR, buffer), buffer); break;
				}
			} else if(c == '.') { /// point (".")
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
			} else if(c == 'x' || c == 'y') { /// special points
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

token Parser::next_token(const unsigned long& incr = 1) {
	if(file_content_pos >= file_content_size) {
		cerr << "Out of file." << endl;

		exit(EXIT_FAILURE);
	}

	token t = file_content.at(file_content_pos);
	file_content_pos += incr;

	actual_line = t.line;
	actual_col = t.col;

	return t;
}

/*******************/
/* Parsing methods */
/*******************/

void Parser::parse_instr() {
	token keyword = next_token();

	while(keyword.type != END) {
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

		keyword = next_token();
	}
}

void Parser::parse_size() {
	token keyword = next_token();

	if(keyword.content != "size")
		print_error(keyword, "expected 'size' keyword (got '" + keyword.content + "').");

	double intpart;

	width = parse_number();

	if(width < 0.0 || modf(width, &intpart) != 0.0)
		print_error("width of the image must be positive integers.");

	height = parse_number();

	if(height < 0.0 || modf(height, &intpart) != 0.0)
		print_error("height of the image must be positive integers.");
}

void Parser::parse_circ() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	Point center = parse_point();
	double radius = parse_number();

	if(radius <= 0.0)
		print_error("radius of circle must be positive.");

	shapes[name] = make_shared<Circ>(Circ(center, radius));
}

void Parser::parse_elli() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	Point center = parse_point();
	double a = parse_number();

	if(a <= 0.0)
		print_error("semi-major radius of ellipse must be positive.");

	double b = parse_number();

	if(b <= 0.0)
		print_error("semi-minor radius of ellipse must be positive.");

	if(a < b)
		print_error("Semi-minor radius must be lesser than semi-major radius.");

	shapes[name] = make_shared<Elli>(Elli(center, a, b));
}

void Parser::parse_rect() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	Point center = parse_point();
	double w = parse_number();

	if(w <= 0.0)
		print_error("width of rectangle must be positive.");

	double h = parse_number();

	if(h <= 0.0)
		print_error("height of rectangle must be positive.");

	shapes[name] = make_shared<Rect>(Rect(center, w, h));
}

void Parser::parse_tri() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	Point v0 = parse_point();
	Point v1 = parse_point();
	Point v2 = parse_point();

	shapes[name] = make_shared<Tri>(Tri(v0, v1, v2));
}

void Parser::parse_shift() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	Point t = parse_point();
	string shift = parse_name();

	it = shapes.find(shift);

	if(it == shapes.end())
		print_error("shape '" + shift + "' doesn't exist.");

	shapes[name] = make_shared<Shift>(Shift(t, it->second));
}

void Parser::parse_rot() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	double angle = parse_number();
	Point r = parse_point();
	string rot = parse_name();

	it = shapes.find(rot);

	if(it == shapes.end())
		print_error("shape '" + rot + "' doesn't exist.");

	shapes[name] = make_shared<Rot>(Rot(angle, r, it->second));
}

void Parser::parse_union() {
	vector<shared_ptr<Shape>> union_shapes;
	string shape_name;

	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	token t = next_token();

	if(t.type != OPEN_BRACE)
		print_error(t, "expected '{' (got '" + t.content + "').");

	do {
		shape_name = parse_name();

		it = shapes.find(shape_name);

		if(it == shapes.end())
			print_error("shape '" + shape_name + "' doesn't exist.");

		union_shapes.push_back(it->second);
	} while(next_token(0).type == STRING);

	t = next_token();

	if(t.type != CLOSE_BRACE)
		print_error(t, "expected '}' (got '" + t.content + "')");

	shapes[name] = make_shared<Union>(Union(union_shapes));
}

void Parser::parse_diff() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it != shapes.end())
		print_error("shape name '" + name + "' already defined at " + to_string(shapes_name[name].first) + ":" + to_string(shapes_name[name].second) + ".");

	shapes_name[name] = make_pair(actual_line, actual_col);

	string shape_in = parse_name();

	auto it_in = shapes.find(shape_in);

	if(it_in == shapes.end())
		print_error("shape '" + shape_in + "' doesn't exist.");

	string shape_out = parse_name();

	auto it_out = shapes.find(shape_out);

	if(it_out == shapes.end())
		print_error("shape '" + shape_out + "' doesn't exist.");

	shapes[name] = make_shared<Diff>(Diff(it_in->second, it_out->second));
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

			if(r < 0.0 || r > 1.0)
				print_error("red value must be between 0.0 and 1.0.");

			g = parse_number();

			if(g < 0.0 || g > 1.0)
				print_error("green value must be between 0.0 and 1.0.");

			b = parse_number();

			if(b < 0.0 || b > 1.0)
				print_error("blue value must be between 0.0 and 1.0.");

			red = uint8_t(round(r * 255));
			green = uint8_t(round(g * 255));
			blue = uint8_t(round(b * 255));

			c = Color(red, green, blue);

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING: {
			name = parse_name();

			auto it = colors.find(name);

			if(it == colors.end())
				print_error("color name '" + name + "' doesn't exist.");

			c = it->second;

			break;
		}

		default: print_error(t, "expected '{' or a color name (got '" + t.content + "').");
	}

	return c;
}

void Parser::parse_color() {
	string name = parse_name();

	auto it = colors.find(name);

	if(it != colors.end())
		print_error("color name '" + name + "' already defined at " + to_string(colors_name[name].first) + ":" + to_string(colors_name[name].second) + ".");

	colors_name[name] = make_pair(actual_line, actual_col);

	Color c = parse_color_def();

	colors[name] = c;
}

void Parser::parse_fill() {
	string name = parse_name();

	auto it = shapes.find(name);

	if(it == shapes.end())
		print_error("shape name '" + name + "' doesn't exist.");

	Color c = parse_color_def();

	it->second->set_color(c);
	fills.push_back(it->second);
}

string Parser::parse_name() {
	token name = next_token();

	if(name.type != STRING)
		print_error(name, "expected string type for name (got '" + name.content + "').");

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

		if(!(t.content.at(0) == 'x' || t.content.at(0) == 'y'))
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
	double x, y, n;
	string name;
	Point p;

	token t = next_token(0);

	switch(t.type) {
		case OPEN_BRACE:
			t = next_token();

			x = parse_number();
			y = parse_number();

			p = Point(x, y);

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING: {
			name = parse_name();

			auto it = shapes.find(name);

			if(it == shapes.end())
				print_error("shape name '" + name + "' doesn't exist.");

			t = next_token();

			if(t.type != POINT)
				print_error(t, "expected '.' (got '" + t.content + "').");

			t = next_token();

			if(t.type != STRING)
				print_error(t, "expected a point name (got '" + t.content + "').");

			try {
				p = it->second->get_named_point(t.content);
			} catch(const invalid_argument& e) {
				print_error(t, "point " + t.content + "doesn't exist.");
			}

			break;
		}

		case OPEN_PAR:
			t = next_token();
			t = next_token();

			if(t.type != OPERATOR)
				print_error(t, "expected an operator (+, -, * or /).");

			p = parse_point();

			switch(t.content.at(0)) {
				case '+':
					t = next_token(0);

					while(t.type == OPEN_BRACE || t.type == STRING || t.type == OPEN_PAR) {
						p += parse_point();
						t = next_token(0);
					}

					break;

				case '-':
					t = next_token(0);

					while(t.type == OPEN_BRACE || t.type == STRING || t.type == OPEN_PAR) {
						p -= parse_point();
						t = next_token(0);
					}
					
					break;

				case '*':
					p *= parse_number();

					break;
				case '/':
					n = parse_number();

					if(n == 0.0)
						print_error(t, "division by 0 not allowed.");

					p /= n;

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

[[noreturn]] void Parser::print_error(const token& t, const string& msg) const {
	cerr << filename << ":" << t.line << ":" << t.col << ": error: " << msg << endl;

	exit(EXIT_FAILURE);
}

[[noreturn]] void Parser::print_error(const string& msg) const {
	cerr << filename << ":" << actual_line << ":" << actual_col << ": error: " << msg << endl;

	exit(EXIT_FAILURE);
}

/*******************/
/* Utility methods */
/*******************/

bool Parser::is_in(const array<char, 6>& a, const char& c) const {
	for(const char& a_c : a)
		if(a_c == c)
			return true;

	return false;
}
