// Syllabizer.cpp	Creed Jones	Feb 4 2026
// This class implements methods to break an English word (string) into syllables
// NOTE: it's neither perfect nor comprehensive, but the resulting syllables are
//  expected to be good enough to calculate word completixy
#include <algorithm>
#include <set>
#include <iostream>

#include "Syllabizer.hpp"

const std::string Syllabizer::vowels = "aeiouyAEIOUY";		// known weakness: y is not always a vowel
const std::string Syllabizer::consonants = "bcdfghjklmnpqrstvwxzBCDFGHJKLMNPQRSTVWXZ";
const std::string Syllabizer::prefixes[] = { "re", "un", "pre", "dis", "non", "de", "con", "en"};
const std::string Syllabizer::suffixes[] = { "ing", "er", "less", "ly", "go" ,"ment", "tion", "sion", "able", "ous"};

bool Syllabizer::isvowel(char c) {
	return (vowels.find(c) < vowels.npos);
}

bool Syllabizer::isconsonant(char c) {
	return (consonants.find(c) < consonants.npos);
}

std::string Syllabizer::preprocesstext( std::string input) {
	input.erase(std::remove_if(input.begin(), input.end(),
		[](unsigned char c) {return (c > 127); }), input.end());       // remove non-base-ascii characters
	std::transform(input.begin(), input.end(), input.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return input;
}

// produce a list of v, c, and maybe other things to represent the vowel-consonant pattern
std::string Syllabizer::getvcpattern(std::string input) {
	std::string pattern;
	// first, find vowels, consonants and others
	for (int index = 0; index < input.length(); index++) {
		if (input[index] == 'y') {
			pattern.append("V");
		}
		else if (isvowel(input[index])) {
			// if the next char is also a vowel, this one will be long and the next will be short
			// replace short vowels with 'v' and long with 'V'
			if (index < (input.length() - 1) && isvowel(input[index + 1])) {
				pattern.append("V");
				index++;
			}
			pattern.append("v");
		}
		else if (isconsonant(input[index]))
			pattern.append("c");
		else
			return std::string("");		// if any other characters are found, return empty string - invalid input
	}
	// if the last letter in the word is a vowel, then it's usually long
	if (pattern[input.length()-1] == 'v') {
		pattern[input.length() - 1] = 'V';
	}

	// if the last letter in the word is an 'e', the previous vowel (if there is one) is long
	// and the e is silent and isn't used in any of the other rules - mark it as other
	if (input[input.size() - 1] == 'e') {
		pattern[input.size() - 1] = 'o';
		// find prior vowel
		int index = static_cast<int>(input.size()) - 1;
		while (--index >= 0) {
			if (pattern[index] == 'v') {
				pattern[index] = 'V';
				break;
			}
		}
	}
	return pattern;
}



std::vector<int> Syllabizer::syllabize(const std::string& input) {
	// The syllable breaks are returned in a vector of ints
	// the value "2" means that the syllable break is BEFORE the original
	// character #2 in the string. So, pos-ter would return a vector of [3]
	std::set<int> result;
	result.insert(0);				// all valid words have at least one syllable
	std::set<int> workingresult;
	// form the vowel-consonant pattern string
	int loc, startloc, endloc;
	// clean the string
	std::string cleaned = preprocesstext(input);
	std::string vcpattern = getvcpattern(cleaned);
	if (vcpattern.size() <= 0) {
		return std::vector<int>();
	}
	// std::cout << "Word [" << input << "],  pattern = [" << vcpattern << "]" << std::endl;
	bool changed = false;
	bool ruleenable[] = { true, true, true, true, true };		// to enable rules separately for debug

	// Rule 1: Prefixes & Suffixes: Separate common prefixes (re-, un-, pre-) and suffixes (-ing, -ed, -less)
	//   from the root word (e.g., re-view, work-ing). 
	//   must apply this iteratively ("carelessly", unprepared", "predisposes")
	if (ruleenable[0]) {
		startloc = 0;
		do {
			changed = false;
			for (std::string prefix : prefixes) {
				if (input.find(prefix, startloc) == startloc) {
				// if (input.substr(startloc, prefix.size()) == prefix) {
					startloc += static_cast<int>(prefix.size());
					result.insert(startloc);
					changed = true;
				}
			}
		} while (changed);

		endloc = static_cast<int>(input.size());
		do {
			changed = false;
			for (std::string suffix : suffixes) {
				int suffixsize = static_cast<int>(suffix.size());
				if (endloc >= suffixsize) {
					if (input.find(suffix, endloc-suffixsize) == endloc-suffixsize) {
					// if (input.substr(endloc - suffixsize, suffixsize) == suffix) {
						endloc -= suffixsize;
						result.insert(endloc);
						changed = true;
					}
				}
			}
		} while (changed);

	}

	// Rule 5. If the first letter is a vowel, either 'o' or 'e', and there is at least one other vowel in the word,
	//  that vowel is a syllable
	if (ruleenable[4]) {
		if ((cleaned[0] == 'o') || (cleaned[0] == 'e')) {
			std::string restofword = vcpattern.substr(1);
			if (restofword.find_first_of("vV") != restofword.npos) {
				result.insert(1);
			}
		}
	}

	// Rule 2: VC/CV (Vowel-Consonant/Consonant-Vowel): Split between the two consonants (e.g., rab-bit, but-ter, par-rot). 
	if (ruleenable[1]) {
		if (result.size() > 0)
		{
			for (std::set<int>::iterator iter = result.begin(); iter != result.end(); ) {
				startloc = *iter;
				endloc = (++iter != result.end()) ? *iter : vcpattern.size();
				std::string subpattern = vcpattern.substr(startloc, endloc-startloc);
				std::transform(subpattern.begin(), subpattern.end(), subpattern.begin(), [](unsigned char c) { return std::tolower(c); } );
				loc = static_cast<int>(subpattern.find("vccv"));
				while (loc >= 0) {
					std::string found(cleaned.substr(startloc + loc, 4));
					result.insert(startloc + loc + 2);
					loc = static_cast<int>(subpattern.find("vccv", startloc + loc + 4));
				}
			}
		}
	}

	// Rule 3: V/CV (Vowel/Consonant-Vowel): Split before the consonant if the first vowel is long (e.g., o-pen, ba-sic)
	if (ruleenable[2]) {
		if (result.size() > 0)
		{
			for (std::set<int>::iterator iter = result.begin(); iter != result.end(); ) {
				startloc = *iter;
				endloc = (++iter != result.end()) ? *iter : vcpattern.size();
				std::string subpattern = vcpattern.substr(startloc, endloc-startloc);
				std::transform(subpattern.begin(), subpattern.end(), subpattern.begin(), [](unsigned char c) { return std::tolower(c); });
				loc = static_cast<int>(subpattern.find("Vcv"));
				while (loc >= 0) {
					std::string found(cleaned.substr(loc, 3));
					result.insert(startloc + loc + 2);
					loc = static_cast<int>(subpattern.find("Vcv", loc + 3));
				}
			}
		}
	}

	// Rule 4. VC / V(Vowel - Consonant / Vowel) : Split after the consonant if the first vowel is short(e.g., cab - in, nap - kin).
	if (ruleenable[3]) {
		if (result.size() > 0)
		{
			for (std::set<int>::iterator iter = result.begin(); iter != result.end(); ) {
				startloc = *iter;
				endloc = (++iter != result.end()) ? *iter : vcpattern.size();
				std::string subpattern = vcpattern.substr(startloc, endloc-startloc);
				std::transform(subpattern.begin(), subpattern.end(), subpattern.begin(), [](unsigned char c) { return std::tolower(c); });
				loc = static_cast<int>(subpattern.find("vcv"));
				while (loc >= 0) {
					std::string found(cleaned.substr(loc, 3));
					result.insert(startloc + loc + 2);
					loc = static_cast<int>(subpattern.find("vcv", loc + 3));		/// TODO = confirm
				}
			}
		}
	}

	// now i need to produce the final result (syllable breaks) in sorted order
	// since they may have been detected in some other order
	std::vector<int> retval;
	while (!result.empty()) {
		retval.push_back(std::move(const_cast<int&>(*result.begin())));
		result.erase(*result.begin());
	}
	return retval;
};