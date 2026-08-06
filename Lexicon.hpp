// Lexicon.hpp     Aditya Kaki  Apr 9 2026

#ifndef LEXICON_HPP
#define LEXICON_HPP

#include <string>
#include <unordered_map>

/*! \brief Enum for parts of speech. */
enum class PARTS_OF_SPEECH {UNKNOWN, ADJECTIVE, ADVERB, ARTICLE, CONJUNCTION, DETERMINER, INTERJECTION, LETTER, NAME, NOT, NOUN, NUMBER, PREPOSITION, PRONOUN, VERB
};

/*! \class Lexicon
    \brief Stores word information including stop status and part of speech.
*/
class Lexicon {
public:

    /*! \brief Returns the one and only Lexicon instance. */
    static Lexicon& getInstance() {
        static Lexicon instance;
        return instance;
    }

    // Delete copy and assignment
    Lexicon(const Lexicon&) = delete;
    Lexicon& operator=(const Lexicon&) = delete;

    /*! \brief Load lexicon from a CSV file.
    */
    int loadcsv(std::string filename);

    /*! \brief Add a single word to the Lexicon.
        \param word The text of the word to add.
        \param isstop True if the word is a stopword.
        \param wordtype The part of speech for this word.
        \return True if added, false if word already exists or error.
    */
    bool addWord(std::string word, bool isstop, PARTS_OF_SPEECH wordtype);

    /*! \brief Check if a word is a stopword.
        \param word word to check.
    */
    bool isStopWord(std::string word);

    /*! \brief Get the part of speech for a word.
        \param word The word to look up.
    */
    PARTS_OF_SPEECH gettype(std::string word);

    /*! \brief The number of words in the lexicon. */
    int size() const;

private:
     Lexicon() = default;
    ~Lexicon() = default;

    struct WordInfo {
        bool stopword;
        PARTS_OF_SPEECH pos;
    };
    std::unordered_map<std::string, WordInfo> words;

    /*! \brief Converting the POS string from the CSV to enum. */
    static PARTS_OF_SPEECH stringToPOS(const std::string& s);
};

#endif