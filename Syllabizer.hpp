// Syllabizer.hpp	Creed Jones	Feb 4 2026
// This class implements methods to break an English word (string) into syllables
#ifndef SYLLABIZER_HPP
#define SYLLABIZER_HPP
#include <string>
#include <vector>

/*! \class Syllabizer
	\brief A utility class for English language syllable decomposition.
*/
class Syllabizer {
public:
	static const std::string vowels; ///< Vowels with y 
	static const std::string consonants;///< Consonants
	static const std::string prefixes[];///< Common prefixes
	static const std::string suffixes[];///< Common suffixes
	// these are defined in the cpp, so the function can be called at class level
private:
	/** @brief Checks if a character is a vowel (a, e, i, o, u, y). @param c The character to check. @return True if vowel. */
	static bool isvowel(char c);
	static bool isconsonant(char c);
	static std::string getvcpattern(std::string input);
public:
	/*! \brief Removes non-alpha characters and lowercases the input. \param input raw word string. \return Cleaned lowercased string. */
	static std::string preprocesstext(std::string input);
	/*! \brief Splits a word into syllables. \param input A single word with no spaces or punctuation.*/
	static std::vector<int> syllabize(const std::string& input);
};

#endif // SYLLABIZER_HPP