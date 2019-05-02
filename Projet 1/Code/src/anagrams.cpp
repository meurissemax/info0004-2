/**
 * Object-oriented programming projects - Project 1
 * Anagram Generator
 *
 * This program generates all the unique anagrams of a string based on a
 * dictionary of words.
 *
 * @author Maxime Meurisse (m.meurisse@student.uliege.be) - 20161278
 * @version 2019.05.04
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cstdlib>

#include "anagrams.hpp"

using namespace std;

[[noreturn]] static void set_error(const string& msg) {
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

static string get_sorted(const string& str) {
	string sorted = str;
	sort(sorted.begin(), sorted.end());

	return sorted;
}

/**
 * This function removes all spaces from a string and checks whether it is
 * composed exclusively of characters included in [a, z].
 *
 * @param 	str The string to check
 * @return 	A Boolean value indicating whether the string contains only
 * 			characters included in [a, z]
 */
static bool check_word(string& str) {
	if(str.empty())
		return false;

	str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());

	for(char c : str)
		if(!(isalpha(c) && islower(c)))
			return false;

	return true;
}

/**
 * This function takes as input two strings and checks if it is possible to
 * remove at the first all the letters of the seconds. If this is the case,
 * the difference of the strings is stored in a parameter passed as reference.
 *
 * @param 	str The string to which we will remove letters, if possible
 * @param 	sub The string to remove at the first string
 * @param 	d 	The string that will contains the difference between str and
 				sub, if this difference is possible
 * @return 	A Boolean value indicating whether it is possible to remove
 *			letters from the second string to the first
 */
static bool diff(const string& str, const string& sub, string& d) {
	if(str.size() < sub.size())
		return false;

	d = str;

	auto it1 = sub.begin();
	auto it2 = d.begin();

	while(it1 != sub.end())
		if(it2 == d.end() || *it1 < *it2)
			return false;
		else if(*it1 > *it2)
			it2++;
		else {
			it2 = d.erase(it2);
			it1++;
		}

	return true;
}

/**
 * This function recursively finds the anagrams of a string based on
 * available dictionary words.
 *
 * @param 	letters The remaining letters to form an anagram
 * @param 	dict The dictionary words available to form an anagram
 * @param 	search 	The position of elements that can be part of an anagram in
 *					the dictionary
 * @param 	chosen The vector of words forming a solution
 * @param 	results The vector containing all the possible anagrams
 * @param 	max The maximum number of words (-1 for no restriction)
 */
static void find(const string& letters, const Dictionary& dict, const vector<long>& search, vector<string>& chosen, vector<vector<string>>& results, const int max) {
	string d;
	vector<long> update;

	/// If the word limit is reached
	if(max == 0)
		return;

	for(auto i = search.rbegin(); i != search.rend(); i++)
		if(diff(letters, dict[unsigned(*i)].second, d)) {
			update.insert(update.begin(), *i);

			/// We add the word to a possible solution
			chosen.push_back(dict[unsigned(*i)].first);

			if(!d.empty())
				find(d, dict, update, chosen, results, max - 1);
			else
				results.push_back(chosen);

			chosen.pop_back();
		}
}

Dictionary create_dictionary(const string& filename) {
	ifstream file;
	string wrd;
	Dictionary dict;
	bool warning = false;

	file.open(filename);

	if(!file)
		set_error("Unable to open file.");

	while(file >> wrd)
		if(check_word(wrd))
			dict.push_back(pair<string, string>(wrd, get_sorted(wrd)));
		else
			warning = true;

	if(warning)
		cout << "Warning! Invalid word(s) in dictionary." << endl;

	file.close();

	return dict;
}

vector<vector<string>> anagrams(const string& input, const Dictionary& dict, unsigned max) {
	int limit = int(max);

	string correct = input, d;
	Dictionary filter;

	vector<long> search;
	vector<string> chosen;
	vector<vector<string>> results;

	/// We first check the input entered by the user
	if(!check_word(correct))
		set_error("Input is not valid.");

	correct = get_sorted(correct);

	/// A dictionary filter is used: only the words whose letters are also
	/// in the user's input are kept
	for(auto p : dict)
		if(diff(correct, p.second, d))
			filter.push_back(p);

	/// The positions of each word are initialized
	for(unsigned i = 0; i < filter.size(); i++)
		search.push_back(i);

	/// With a value of -1, the limit value will never reach 0 by decrementing
	/// in recursive calls.
	if(limit == 0)
		limit = -1;

	find(correct, filter, search, chosen, results, limit);

	return results;
}
