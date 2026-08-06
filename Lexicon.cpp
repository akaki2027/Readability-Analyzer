// Lexicon.cpp     Aditya Kaki  Apr 9 2026

#include "Lexicon.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

PARTS_OF_SPEECH Lexicon::stringToPOS(const std::string& s) {
    if (s == "Adjective") return PARTS_OF_SPEECH::ADJECTIVE;
    if (s == "Adverb") return PARTS_OF_SPEECH::ADVERB;
    if (s == "Article") return PARTS_OF_SPEECH::ARTICLE;
    if (s == "Conjunction") return PARTS_OF_SPEECH::CONJUNCTION;
    if (s == "Determiner") return PARTS_OF_SPEECH::DETERMINER;
    if (s == "Interjection") return PARTS_OF_SPEECH::INTERJECTION;
    if (s == "Letter") return PARTS_OF_SPEECH::LETTER;
    if (s == "Name") return PARTS_OF_SPEECH::NAME;
    if (s == "Not") return PARTS_OF_SPEECH::NOT;
    if (s == "Noun") return PARTS_OF_SPEECH::NOUN;
    if (s == "Number") return PARTS_OF_SPEECH::NUMBER;
    if (s == "Preposition") return PARTS_OF_SPEECH::PREPOSITION;
    if (s == "Pronoun") return PARTS_OF_SPEECH::PRONOUN;
    if (s == "Verb") return PARTS_OF_SPEECH::VERB;
    return PARTS_OF_SPEECH::UNKNOWN;
}

int Lexicon::loadcsv(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return 0;

    std::string line;
    //skip the header
    std::getline(file, line);

    int count = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;

        // Parse CSV
        std::stringstream ss(line);
        std::string word, stopstr, freqstr, posstr;

        std::getline(ss, word, ',');
        std::getline(ss, stopstr, ',');
        std::getline(ss, freqstr, ',');
        std::getline(ss, posstr, ',');

        if (word.empty()) continue;

        // Converting all to lowercase
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c) { return std::tolower(c); });

        bool isstop = (stopstr == "1");
        PARTS_OF_SPEECH pos = stringToPOS(posstr);

        if (addWord(word, isstop, pos)) {
            count++;
        }
    }
    return count;
}

bool Lexicon::addWord(std::string word, bool isstop, PARTS_OF_SPEECH wordtype) {
    //Converting all to lowercase
    std::transform(word.begin(), word.end(), word.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (word.empty()) return false;
    if (words.find(word) != words.end()) return false;

    words[word] = { isstop, wordtype };
    return true;
}

bool Lexicon::isStopWord(std::string word) {
    std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
    auto it = words.find(word);
    if (it != words.end()) return it->second.stopword;
    return false;
}

PARTS_OF_SPEECH Lexicon::gettype(std::string word) {
    std::transform(word.begin(), word.end(), word.begin(),
        [](unsigned char c) { return std::tolower(c); });
    auto it = words.find(word);
    if (it != words.end()) return it->second.pos;
    return PARTS_OF_SPEECH::UNKNOWN;
}

int Lexicon::size() const {
    return (int)words.size();
}