#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <cstdlib>
#include <vector>
#include <memory>
#include <utility>

#include "geometry.hpp"
#include "graphics.hpp"

enum parse_name_type : unsigned int {
	NEW_SHAPE,
	NEW_COLOR,
	NEW_FILL,
	CHECK_SHAPE,
	CHECK_COLOR
};

enum type : unsigned int {
	END,			/// End of file
	STRING,			/// fig_circle, x
	NUMBER,			/// +3, .4, 5.0, -1.
	POINT,			/// .
	OPEN_BRACE,		/// {
	CLOSE_BRACE,	/// }
	OPEN_PAR,		/// (
	CLOSE_PAR,		/// )
	OPERATOR		/// +, -, *, /
};

struct token {
	unsigned int line, col;
	unsigned int type;
	std::string content;
};

class Parser {
public:
	Parser() { }
	Parser(const std::string fname);

	void parse_file();
	void print_stats() const;

	size_t get_width() const;
	size_t get_height() const;
	std::vector<std::shared_ptr<Shape>> get_shapes() const;

private:
	const std::vector<char> SPECIAL_POINTS {'x', 'y'};
	const std::vector<char> SPECIAL_CHARS {'#', '{', '}', '(', ')', '*', '/', char(32)}; /// char(32) = ' '

	std::string filename;
	std::vector<token> file_content;

	unsigned int file_content_pos = 0;
	unsigned int file_content_size;

	std::vector<token> shapes;
	std::vector<token> colors;
	std::vector<token> fills;

	size_t _width;
	size_t _height;
	std::vector<std::pair<std::string, Color>> _colors;
	std::vector<std::shared_ptr<Shape>> _shapes;

	/****************************/
	/* Token conversion methods */
	/****************************/

	token create_token(const unsigned int line, const unsigned int col, const std::string content, std::ifstream& file) const;
	token create_token(const unsigned int line, const unsigned int col, const unsigned int type, const std::string content) const;

	void push_token(const token t, std::string& buffer);

	void convert_token();

	token next_token(const int& incr);

	/*******************/
	/* Parsing methods */
	/*******************/

	void parse_instr();

	void parse_size();

	void parse_circ();
	void parse_elli();
	void parse_rect();
	void parse_tri();

	void parse_shift();
	void parse_rot();
	void parse_union();
	void parse_diff();

	Color parse_color_def();
	void parse_color();
	void parse_fill();

	std::string parse_name(const unsigned int& instr_type);

	double parse_number();
	Point parse_point();

	/***************************/
	/* Error management method */
	/***************************/

	[[noreturn]] void print_error(const std::string& msg) const;
	[[noreturn]] void print_error(const token& t, const std::string& msg) const;

	/*******************/
	/* Utility methods */
	/*******************/
	
	bool is_in(const std::vector<char>& v, const char& c) const;
	bool is_in(const std::vector<token>& v, const token& t, token& exist) const;
};

#endif
