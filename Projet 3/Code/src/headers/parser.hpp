#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <array>
#include <map>

#include "geometry.hpp"
#include "graphics.hpp"

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
	unsigned long line, col;
	unsigned long type;
	std::string content;
};

class Parser {
public:
	Parser() { print_error("A file is required."); }
	Parser(std::string fname);

	void parse_file();
	void print_stats() const;

	size_t get_width() const { return width; }
	size_t get_height() const { return height; }
	std::vector<std::shared_ptr<Shape>> get_fills() const { return fills; }

private:
	const std::array<char, 6> SPECIAL_CHARS {{'{', '}', '(', ')', '*', '/'}};

	std::string filename;
	std::vector<token> file_content;

	unsigned long file_content_pos = 0;
	unsigned long file_content_size;

	std::map<std::string, std::shared_ptr<Shape>> shapes;
	std::map<std::string, Color> colors;

	size_t width, height;
	std::vector<std::shared_ptr<Shape>> fills;

	/****************************/
	/* Token conversion methods */
	/****************************/

	token create_token(unsigned int line, unsigned int col, std::string content, std::ifstream& file) const;
	token create_token(unsigned int line, unsigned int col, unsigned int type, std::string content) const;

	void push_token(token t, std::string& buffer);

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

	std::string parse_name();

	double parse_number();
	Point parse_point();

	/***************************/
	/* Error management method */
	/***************************/

	void print_error(const std::string& msg) const;
	void print_error(const token& t, const std::string& msg) const;

	/*******************/
	/* Utility methods */
	/*******************/

	bool is_in(const std::array<char, 6>& a, const char& c) const;
};

#endif
