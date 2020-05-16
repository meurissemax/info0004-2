#ifndef PARSER_HH
#define PARSER_HH

#include <string>
#include <vector>

/// This enum defines the different possible operations for the 'parse_name' method.
enum parse_name_type : unsigned int {
	NEW_SHAPE,
	NEW_COLOR,
	NEW_FILL,
	CHECK_SHAPE,
	CHECK_COLOR
};

/// This enum defines the different possible types of token.
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
	unsigned int line, col;
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
	Parser() { }
	Parser(const std::string fname);

	/**
	 * This function is use to parse the file 'filename'.
	 * It does this in 2 operations :
	 * 		1. convert all the content of the file into tokens;
	 * 		2. parse the file with the proper instructions (size first, and then the rest).
	 */
	void parse_file();

	/**
	 * This function is use to print is the 'stdout' the statistics of the parsing.
	 * The statistics are :
	 * 		- the number of shapes defined
	 * 		- the number of colors defined
	 * 		- the number of fill operations used
	 */
	void print_stats() const;

private:
	/// Vectors of special points and chars in the paint file.
	const std::vector<char> SPECIAL_POINTS{'x', 'y'};
	const std::vector<char> SPECIAL_CHARS{'#', '{', '}', '(', ')', '*', '/', char(32)}; /// char(32) = ' '

	/// The file that the parser parse.
	std::string filename;

	/// The content of the file, converted to tokens (always ending by a "END" token).
	std::vector<token> file_content;

	/// Informations about the 'file_content' for 'next_token' method.
	unsigned int file_content_pos = 0;
	unsigned int file_content_size;

	/// Informations about the parsing operation.
	std::vector<token> shapes;
	std::vector<token> colors;
	std::vector<token> fills;

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
	void parse_rect();
	void parse_tri();

	void parse_shift();
	void parse_rot();
	void parse_union();
	void parse_diff();

	void parse_color_def();
	void parse_color();
	void parse_fill();

	void parse_name(const unsigned int& instr_type);

	void parse_number();
	void parse_point();

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
