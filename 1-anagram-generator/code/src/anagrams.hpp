#ifndef ANAGRAMS_HH
#define ANAGRAMS_HH

#include <vector>
#include <utility>
#include <string>

typedef std::vector<std::pair<std::string, std::string>> Dictionary;

/**
 * This function initializes a dictionary (of type 'Dictionnary') from a list
 * of words (supposed to be sorted alphabetically), filled in a txt file. If
 * a word is not valid (does not contain only lowercase letters), a warning
 * message is printed, but the dictionary is still created.
 *
 * @param 	filename The path to the txt file containing the dictionary words
 * @return 	A dictionary containing all the words in the file in the same order
 */
Dictionary create_dictionary(const std::string& filename);

/**
 * This function takes as input a string entered by the user, a dictionary
 * of words (of type 'Dictionary') and a limit. It checks whether the string
 * of the user is valid. If this is the case, it finds all the anagrames of
 * this chain (containing at most 'max' words if 'max' is > 0, and as many
 * words as possible if 'max' = 0) and returns them in a vector.
 *
 * @param 	input The string entered by the user
 * @param	dict The dictionary of words
 * @param 	max The maximum number of words (0 for no restriction)
 * @return 	A vector where each element is a vector containing a unique
 *			anagram of the string entered by the user
 */
std::vector<std::vector<std::string>> anagrams(const std::string& input, const Dictionary& dict, unsigned max);

#endif
