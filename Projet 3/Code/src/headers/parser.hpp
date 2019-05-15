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

/// This enum defines the different possible types of a token.
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

/**
 * A token is an element of the file that is defined by
 * its position in the file ('line' and 'col'),
 * its type (cfr. enum 'type')
 * and its 'content'.
 */
struct token {
	unsigned long line, col;
	unsigned int type;
	std::string content;
};

/**
 * The 'Parser' class parses the contents of a file.
 * The file to be parsed is informed to the class during its instantiation.
 *
 * First, the parser takes care of converting all the content of the file into tokens.
 * Secondly, the tokens are traversed sequentially and parsed with the proper instructions.
 *
 * If an error occurs, it is displayed in the 'stderr' and the program stops.
 */
class Parser {
public:
	[[noreturn]] Parser();
	Parser(std::string fname);

	/**
	 * Parse the file 'filename'.
	 * It does this in 2 operations :
	 * 		1. convert all the content of the file into tokens;
	 * 		2. parse the file with the proper instructions (size first, and then the rest).
	 */
	void parse_file();

	/**
	 * Print is the 'stdout' the statistics of the parsing.
	 * The statistics are :
	 * 		- the number of shapes defined
	 * 		- the number of colors defined
	 * 		- the number of fill operations used
	 */
	void print_stats() const;

	size_t get_width() const { return size_t(width); }
	size_t get_height() const { return size_t(height); }
	std::vector<std::shared_ptr<Shape>> get_fills() const { return fills; }

private:
	/// Array of special chars in the paint file.
	const std::array<char, 6> SPECIAL_CHARS {{'{', '}', '(', ')', '*', '/'}};

	std::string filename; /// the file that the parser parse
	std::vector<token> file_content; /// the content of the file, converted to tokens (always ending by a "END" token)

	/// Informations about the 'file_content' for 'next_token' method.
	unsigned long file_content_pos = 0;
	unsigned long file_content_size;

	/// Informations about the position of the current token.
	unsigned long actual_line = 0;
	unsigned long actual_col = 0;

	/// Informations about shapes and color declared in the paint file.
	std::map<std::string, std::shared_ptr<Shape>> shapes;
	std::map<std::string, std::pair<unsigned long, unsigned long>> shapes_name;

	std::map<std::string, Color> colors;
	std::map<std::string, std::pair<unsigned long, unsigned long>> colors_name;

	/// Final informations to provide to create the PPM image.
	double width, height;
	std::vector<std::shared_ptr<Shape>> fills;

	/****************************/
	/* Token conversion methods */
	/****************************/

	/**
	 * Create a token by determining its type based on its content.
	 *
	 * @param line the line of the token
	 * @param col the col of the token
	 * @param content the content of the token
	 * @param file the file to close if an error occured
	 * @return a token whose type has been determined according to its content
	 */
	token create_token(unsigned long line, unsigned long col, std::string content, std::ifstream& file) const;

	/**
	 * Create a token.
	 *
	 * @param line the line of the token
	 * @param col the col of the token
	 * @param type the type of the token
	 * @param content the content of the token
	 * @return the created token
	 */
	token create_token(unsigned long line, unsigned long col, unsigned int type, std::string content) const;

	/**
	 * Push the token in the 'file_content' vector.
	 * It also flushes the buffer.
	 * If the buffer is empty, it means that the token is empty too (no content, unknown type), so we don't push.
	 *
	 * @param t the token to push
	 * @param buffer the buffer containing the content of the token
	 */
	void push_token(token t, std::string& buffer);

	/**
	 * Convert the content of the file 'filename' to tokens and store it in 'file_content'.
	 */
	void convert_token();

	/**
	 * Return a token of the 'file_content' vector.
	 * Depending on the value of 'incr', the next token that will be return at next call is the same or the next.
	 *
	 * @param incr an increment value that determines the next token that will be return at next call
	 * @return a token of the 'file_content' vector
	 */
	token next_token(const unsigned long& incr);

	/*******************/
	/* Parsing methods */
	/*******************/

	/**
	 * These instructions are used to parse the different instructions
	 * according to the project statement.
	 *
	 * Parse of atomic element returns the element (color, name, number and point).
	 */

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

	/**
	 * Print a message in the stderr with information about the wrong token,
	 * according to the format explained in the project guidelines.
	 *
	 * @param t the wrong token
	 * @param msg the msg to print in the stderr
	 */
	[[noreturn]] void print_error(const token& t, const std::string& msg) const;

	/**
	 * Print a message in the stderr with information about the wrong token,
	 * according to the format explained in the project guidelines.
	 *
	 * @param msg the msg to print in the stderr
	 */
	[[noreturn]] void print_error(const std::string& msg) const;

	/*******************/
	/* Utility methods */
	/*******************/

	/**
	 * Check if a given char is in an array of chars.
	 *
	 * @param a the array of chars
	 * @param c the char
	 * @return a boolean value indicating if the char 'c' is in the array of chars 'a'
	 */
	bool is_in(const std::array<char, 6>& a, const char& c) const;
};

#endif
