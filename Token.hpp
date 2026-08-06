#ifndef TOKEN_HPP
#define TOKEN_HPP
#include <string>
#include <vector>
#include "Lexicon.hpp"

class Lexicon;
enum class PARTS_OF_SPEECH;

class Token {
private: 
	std::string contents;
	bool isStop;
	PARTS_OF_SPEECH pos;
	std::vector<int> sylBreaks;
	int startPos;
public:
	/*! \brief The Constructor. \param sample String value of the token. */
	Token(std::string sample = "");
	/*! \brief The Copy constructor. */
	Token(const Token& other);
	/*! \brief The Assignment operator. */
	Token& operator = (const Token& other);
	/*! \brief The Equality operator. */
	bool operator==(const Token& other) const;

	/*! \brief Returning the token string. */
	std::string get() const;

	/*! \brief Set the lexicon info from Lexicon. */
	void setlexinfo(Lexicon* lex);
	void setlexinfoRef();

	 /*! \brief Set syllable breaks. */
    void setSylBreaks(const std::vector<int>& breaks);
	
	/*! \brief Get syllable breaks. */
    const std::vector<int>& getSylBreaks() const;

	 /*! \brief Get position in original text. */
    int getStartPos() const;

	/*! \brief Set position in original text. */
    void setStartPos(int pos);

	/*! \brief Token as a html with POS and stopword status.*/
	std::string getHTML() const;

	/*! \brief Get syllabized string */
    std::string getSyllabizedString() const;

	/*! \brief part of speech. */
	PARTS_OF_SPEECH getpos() const;

	/*! \brief Check if this token is a stopword. */
	bool getIsStop() const;

	
};

#endif