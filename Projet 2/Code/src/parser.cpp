/**
 * Object-oriented programming projects - Project 2
 * Parsing a painting language
 *
 * This file is the implementation of the 'Parser' class.
 *
 * @author Maxime MEURISSE (m.meurisse@student.uliege.be) - 20161278
 * @version 2019.04.26
 */

#include <fstream>
#include <iostream>
#include <cstdlib>

#include "parser.hpp"

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
	cout << "Number of shapes: " << shapes.size() << endl;
	cout << "Number of colors: " << colors.size() << endl;
	cout << "Number of fills: " << fills.size() << endl;
}

/*******************/
/* Private methods */
/*******************/

/****************************/
/* Token conversion methods */
/****************************/

/**
 * This method is used to create a token by determining its type based on its content.
 *
 * @param line the line of the token
 * @param col the col of the token
 * @param content the content of the token
 * @param file the file to close if an error occured
 *
 * @return a token whose type has been determined according to its content
 */
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

/**
 * This method is used to create a token.
 *
 * @param line the line of the token
 * @param col the col of the token
 * @param type the type of the token
 * @param content the content of the token
 *
 * @return the created token
 */
token Parser::create_token(const unsigned int line, const unsigned int col, const unsigned int type, const string content) const {
	token t = {line, unsigned(col - content.length()), type, content};

	return t;
}

/**
 * This method is used to push the token in the 'file_content' vector.
 * It also flushes the buffer.
 * If the buffer is empty, it means that the token is empty too (no content, unknown type), so we don't push.
 *
 * @param t the token to push
 * @param buffer the buffer containing the content of the token
 */
void Parser::push_token(const token t, string& buffer) {
	if(!buffer.empty()) {
		file_content.push_back(t);
		buffer.clear();
	}
}

/**
 * This method is used to convert the content of the file 'filename' to tokens and store it in 'file_content'.
 */
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

/**
 * This method is used to return a token of the 'file_content' vector.
 * Depending on the value of 'incr', the next token that will be return at next call is the same or the next.
 *
 * @param incr an increment value that determines the next token that will be return at next call
 *
 * @return a token of the 'file_content' vector
 */
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

/**
 * This method is used to parse an instruction.
 * It determines the instruction based on the content of the keyword token,
 * and call the corresponding parse method.
 * It stops when the "END" token pop.
 */
void Parser::parse_instr() {
	token keyword = next_token();

	if(keyword.type != END) {
		string content = keyword.content;

		if(keyword.type != STRING)
			print_error(keyword, "invalid keyword format ('" + keyword.content + "').");

		if(content == "circ")
			parse_circ();
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

/**
 * This method is used to parse a 'size' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_size() {
	token keyword = next_token();

	if(keyword.content != "size")
		print_error(keyword, "expected 'size' keyword (got '" + keyword.content + "').");

	parse_number();
	parse_number();
}

/**
 * This method is used to parse a 'circ' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_circ() {
	parse_name(NEW_SHAPE);
	parse_point();
	parse_number();

	parse_instr();
}

/**
 * This method is used to parse a 'rect' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_rect() {
	parse_name(NEW_SHAPE);
	parse_point();
	parse_number();
	parse_number();

	parse_instr();
}

/**
 * This method is used to parse a 'tri' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_tri() {
	parse_name(NEW_SHAPE);
	parse_point();
	parse_point();
	parse_point();

	parse_instr();
}

/**
 * This method is used to parse a 'shift' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_shift() {
	parse_name(NEW_SHAPE);
	parse_point();
	parse_name(CHECK_SHAPE);

	parse_instr();
}

/**
 * This method is used to parse a 'rot' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_rot() {
	parse_name(NEW_SHAPE);
	parse_number();
	parse_point();
	parse_name(CHECK_SHAPE);

	parse_instr();
}

/**
 * This method is used to parse a 'union' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_union() {
	parse_name(NEW_SHAPE);

	token t = next_token();

	if(t.type != OPEN_BRACE)
		print_error(t, "expected '{' (got '" + t.content + "').");

	parse_name(CHECK_SHAPE);

	while(next_token(0).type == STRING)
		parse_name(CHECK_SHAPE);

	t = next_token();

	if(t.type != CLOSE_BRACE)
		print_error(t, "expected '}' (got '" + t.content + "')");

	parse_instr();
}

/**
 * This method is used to parse a 'diff' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_diff() {
	parse_name(NEW_SHAPE);
	parse_name(CHECK_SHAPE);
	parse_name(CHECK_SHAPE);

	parse_instr();
}

/**
 * This method is used to parse a color definition, i.e. a "color" in EBNF.
 */
void Parser::parse_color_def() {
	token t = next_token(0);

	switch(t.type) {
		case OPEN_BRACE:
			t = next_token();

			parse_number();
			parse_number();
			parse_number();

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING: parse_name(CHECK_COLOR); break;
		default: print_error(t, "expected '{' or a color name (got '" + t.content + "').");
	}
}

/**
 * This method is used to parse a 'color' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_color() {
	parse_name(NEW_COLOR);
	parse_color_def();

	parse_instr();
}

/**
 * This method is used to parse a 'fill' instruction,
 * defined according to the project guidelines.
 */
void Parser::parse_fill() {
	parse_name(NEW_FILL);
	parse_color_def();

	parse_instr();
}

/**
 * This method is used to parse a name, i.e. a "name" in EBNF.
 * Depending on the value of 'parse_name_type', the method parse a shape name, color name or fill name,
 * and also check if the name exists/doesn't exist.
 *
 * @param parse_name_type determines the type of operation the method should perform
 */
void Parser::parse_name(const unsigned int& parse_name_type) {
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
}

/**
 * This method is used to parse a number, i.e. a 'number' in EBNF.
 */
void Parser::parse_number() {
	token t = next_token(0);

	if(t.type != NUMBER) {
		parse_point();

		t = next_token();

		if(t.type != POINT)
			print_error(t, "expected '.' (got '" + t.content + "').");

		t = next_token();

		if(!is_in(SPECIAL_POINTS, t.content.at(0)))
			print_error(t, "'" + t.content + "' is not a valid coordinate.");
	} else {
		t = next_token();
	}
}

/**
 * This method is used to parse a point, i.e. a 'point' in EBNF.
 */
void Parser::parse_point() {
	token t = next_token(0);

	switch(t.type) {
		case OPEN_BRACE:
			t = next_token();

			parse_number();
			parse_number();

			t = next_token();

			if(t.type != CLOSE_BRACE)
				print_error(t, "expected '}' (got '" + t.content + "').");

			break;

		case STRING:
			parse_name(CHECK_SHAPE);

			t = next_token();

			if(t.type != POINT)
				print_error(t, "expected '.' (got '" + t.content + "').");

			t = next_token();

			if(t.type != STRING)
				print_error(t, "expected a point name (got '" + t.content + "').");

			break;

		case OPEN_PAR:
			t = next_token();
			t = next_token();

			if(t.type != OPERATOR)
				print_error(t, "expected an operator (+, -, * or /).");

			switch(t.content.at(0)) {
				case '+':
				case '-':
					do {
						parse_point();

						t = next_token(0);
					} while(t.type == OPEN_BRACE || t.type == STRING || t.type == OPEN_PAR);

					break;

				case '*':
				case '/':
					parse_point();
					parse_number();

					break;

				default: print_error(t, "unknown operator.");
			}

			t = next_token();

			if(t.type != CLOSE_PAR)
				print_error(t, "expected ')' (got '" + t.content + "').");

			break;

		default: print_error(t, "expected a valid point definition.");
	}
}

/***************************/
/* Error management method */
/***************************/

/**
 * This method is used to print a message in the stderr
 * and stops the execution of the program with an error code.
 *
 * @param msg the msg to print in the stderr
 */
[[noreturn]] void Parser::print_error(const string& msg) const {
	cerr << msg << endl;

	exit(EXIT_FAILURE);
}

/**
 * This method is used to print a message in the stderr with information about the wrong token,
 * according to the format explained in the project guidelines.
 *
 * @param t the wrong token
 * @param msg the msg to print in the stderr
 */
[[noreturn]] void Parser::print_error(const token& t, const string& msg) const {
	cerr << filename << ":" << t.line << ":" << t.col << ": error: " << msg << endl;

	exit(EXIT_FAILURE);
}

/*******************/
/* Utility methods */
/*******************/

/**
 * This method is used to check if a given char is in a vector of chars.
 *
 * @param v the vector of chars
 * @param c the char
 *
 * @return a boolean value indicating if the char 'c' is in the vector of chars 'v'
 */
bool Parser::is_in(const vector<char>& v, const char& c) const {
	for(char v_c : v)
		if(v_c == c)
			return true;

	return false;
}

/**
 * This method is used to check if a given token is in a vector of tokens.
 * If the token already exists in the vector, the existing token is assigned to 'exist'.
 *
 * @param v the vector of tokens
 * @param t the token
 * @param exist the existing token (if already exists)
 *
 * @return a boolean value indicating if the token 't' is in the vector of tokens 'v'
 */
bool Parser::is_in(const vector<token>& v, const token& t, token& exist) const {
	for(token v_t : v)
		if(v_t.content == t.content) {
			exist = v_t;

			return true;
		}

	return false;
}
