#include "Token.hpp"
#include "Syllabizer.hpp"
#include "Lexicon.hpp"

Token::Token(std::string sample) : contents(sample), isStop(false), pos(PARTS_OF_SPEECH::UNKNOWN), startPos(0) {}
Token::Token(const Token& other) : contents(other.contents), isStop(other.isStop), pos(other.pos), sylBreaks(other.sylBreaks), startPos(other.startPos) {}

Token& Token::operator=(const Token& other) {
if(this != &other) {
		contents = other.contents;
		isStop = other.isStop;
		pos = other.pos;
        sylBreaks = other.sylBreaks;
        startPos  = other.startPos;
	}
	return *this;
}
bool Token::operator==(const Token& other) const {
	return contents == other.contents;
}
std::string Token::get() const {
return contents;
}



void Token::setlexinfo(Lexicon* lex) {
	if (!lex) return;
	isStop = lex->isStopWord(contents);
	pos = lex->gettype(contents);
}
void Token::setlexinfoRef() {
    Lexicon& lex = Lexicon::getInstance();
     isStop = lex.isStopWord(contents);
    pos    = lex.gettype(contents);
}
 
void Token::setSylBreaks(const std::vector<int>& breaks) {
    sylBreaks = breaks;
}
 
const std::vector<int>& Token::getSylBreaks() const {
    return sylBreaks;
}
 
void Token::setStartPos(int p) { startPos = p; }
int  Token::getStartPos() const { return startPos; }
 
std::string Token::getSyllabizedString() const {
    if (sylBreaks.empty()) return contents;
    std::string result = contents;
    // insert dashes from right to left so indices stay valid
    for (auto it = sylBreaks.rbegin(); it != sylBreaks.rend(); ++it) {
        if (*it > 0 && *it < (int)result.size())
             result.insert(*it, "-");
    }
     return result;
}  

std::string Token::getHTML() const {
    std::string word = contents;

     if (isStop) return "<i>" + word + "</i>";

    switch (pos) {
        case PARTS_OF_SPEECH::NOUN:
            return "<span style=\"color:#FF0000;\">" + word + "</span>";
        case PARTS_OF_SPEECH::VERB:
             return "<span style=\"color:#00FF00;\">" + word + "</span>";
        case PARTS_OF_SPEECH::ADJECTIVE:
        case PARTS_OF_SPEECH::ADVERB:
            return "<span style=\"color:#0000FF;\">" + word + "</span>";
        case PARTS_OF_SPEECH::CONJUNCTION:
             return "<span style=\"color:#0000FF;\">" + word + "</span>";
        case PARTS_OF_SPEECH::UNKNOWN:
            return word + "[?]";
        default:
            return word;
    }
}
PARTS_OF_SPEECH Token::getpos() const { return pos; }
 bool Token::getIsStop() const { return isStop; }