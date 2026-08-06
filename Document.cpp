// Document.cpp     Aditya Kaki  Jan 28 2026
//  implementation file for the Document class

#include "Document.hpp"
#include "Syllabizer.hpp"
#include "ThreadSafeQueue.hpp"
#include <set>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cctype>
#include <algorithm>
#include <cmath>
#include <thread>
#include <utility>

void Document::preprocess(std::string& s)
{
	std::string r = "";
	for (unsigned char c : s) {
        if (c<=127) {
            	r += (char)std::tolower(c);
		}
    }
   	 s = r;
}

void Document::tokenize() {
    this->tokens.clear();
    if (content.empty()) return;

    const char* delims = " \t\r\n\"'.?!-;:()#$&*,0123456789";
    char* cstr = new char[content.length() + 1];
    std::strcpy(cstr, content.c_str());

    char* tp = std::strtok(cstr, delims);
    while (tp != nullptr) {
        tokens.push_back(Token(std::string(tp)));
        tp = std::strtok(nullptr, delims);
        }
    delete[] cstr;
}


Document::Document(std::string introcontent) : txtname(""){
	preprocess(introcontent);
   	 this->content = introcontent;
    tokenize();
}
Document::Document(const Document& other) : txtname(other.txtname), content(other.content), tokens(other.tokens), tuple_data(other.tuple_data) {}

Document& Document::operator=(const Document& other) {
	if(this != &other) {
		txtname = other.txtname;
		content = other.content;
	tokens = other.tokens;
		tuple_data = other.tuple_data;
		}
	return *this;
}

bool Document::operator==(const Document& other) const {
	return (txtname == other.txtname && content == other.content);
}

int Document::load(std::string name) {

	std::ifstream file(name);
	if (!file.is_open()) return -1;
		std::stringstream buff;
	buff << file.rdbuf();
	std::string tcontent = buff.str();

	preprocess(tcontent);
	this->txtname = name;
	this->content = tcontent;

	tokenize();
	return (int)content.length();

}

int Document::loadContentsOnly(std::string name) {
    std::ifstream file(name);
    if (!file.is_open()) return -1;
    std::stringstream buff;
    buff << file.rdbuf();
    std::string tcontent = buff.str();
    preprocess(tcontent);
    this->txtname = name;
    this->content = tcontent;
    return (int)content.length();
}

void Document::processTokens() {
    tokenize();
}

size_t Document::numtokens() const {
	return tokens.size();
}


int Document::computecounts(int length) {
    if (length <= 0 || tokens.size() < (size_t)length) return 0;
    std::map<std::string, int> CURRENT;
    for (size_t i = 0; i <= tokens.size() - length; ++i) {
        std::string tuple = "";//empty
        for (int j = 0; j < length; ++j) {
            tuple += tokens[i + j].get() + (j == length - 1 ? "" : " "); //Start at first word 
        }
        CURRENT[tuple]++;
    }
    tuple_data[length] = CURRENT;
    return (int)CURRENT.size(); //# of tuples
}

int Document::gettuplecount(int length, int num, std::string& tuple, int& count) {
    if (tuple_data.find(length) == tuple_data.end()) return -1;

    auto& counts = tuple_data[length];
    if (num < 0 || (size_t)num >= counts.size()) return -2;

    // Sort by frequency
    std::vector<std::pair<std::string, int>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
    tuple = sorted[num].first;
    count = sorted[num].second;
    return num;
}

int Document::totalsyllables() const {
    int syls = 0; 
    for (const auto& t : tokens) {
        syls += (int)Syllabizer::syllabize(t.get()).size();
    }
    return syls;
}

int Document::numsentences() const {
    if (content.empty()) return 0;
    int count = 0;
    for (size_t i = 0; i < content.length(); i++)
    {
        if (content[i] == '?' || content[i] == '!') {
            count++;
        }
        else if (content[i] == '.') {
            if (i + 1 < content.length() && content[i + 1] == '.') {
                while (i < content.length() && content[i] == '.') {
                    i++;
                }
                i--;
            }
            else {
                count++;
            }
        }
    }
    return (count == 0 && !tokens.empty()) ? 1 : count;
}

int Document::nhardwords() const {
    int hard = 0;
    for (const auto& t : tokens) {
        if (Syllabizer::syllabize(t.get()).size() > 2)
        {
            hard++;
        }
    }
    return hard;

}
double Document::getreadingease() const {
    double w = numtokens();
    double s = numsentences();
    double syl = totalsyllables();
    if (w == 0 || s == 0) return -1.0;
    return 206.835 - 1.015 * (w / s) - 84.6 * (syl / w);
}

double Document::getfleschkincaid() const {
    double w = numtokens();
    double s = numsentences();
    double syl = totalsyllables();

    if (w == 0 || s == 0) return -1.0;
    return 0.39 * (w / s) + 11.8 * (syl / w) - 15.59;
}
double Document::getgunningfog() const {
    double w = numtokens();
    double s = numsentences();
    double hw = nhardwords();
    if (w == 0 || s == 0) return -1.0;
    return  0.4 * ((w / s) + 25 * (hw / w));
} 



int Document::numsyllables() const {
    return totalsyllables();
}
	
int Document::numhardwords() const {
    return nhardwords();
}

double Document::getfkgradelevel() const {
    return getfleschkincaid();
}

double Document::getgfgradelevel() const {
    return getgunningfog();
}

int Document::numuniquewords() const {
    std::set<std::string> unique;
    for (const auto& t : tokens) {
        unique.insert(t.get());
    }
    return (int)unique.size();
}
 
void Document::setlexinfo(Lexicon* lex) {
    for (Token& t : tokens) {
        t.setlexinfo(lex);
    }
}
 
std::string Document::getHTML() const {
     std::string html = "";
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0) html += " ";
        html += tokens[i].getHTML();
    }
    return html;
}

 

using WorkItem   = std::pair<std::string, int>;
using ResultItem = Token;
 static void workerFunc(ThreadSafeQueue<WorkItem>& workQ, ThreadSafeQueue<ResultItem>& resultQ )
 {
    Lexicon& lex = Lexicon::getInstance();
    while(true){
        WorkItem item = workQ.pop();
        if(item.first.empty()) break;
        Token t(item.first);
        t.setStartPos(item.second);
        auto breaks = Syllabizer::syllabize(item.first); //syllabize content
         t.setSylBreaks(breaks);
        // Lexical info from Singleton
        t.setlexinfoRef();
 
        resultQ.push(t);
    }
 }

 void Document::extracttokens(int numthreads) {
    if(content.empty()) return;
     if(numthreads < 1) numthreads = 1;
    if(numthreads > 64) numthreads = 64;

    //reset
    tokens.clear();
    htmlContents.clear();
    syllabizedContents.clear();
    extractDone = false;

    //Tokenize to get raw word list and positions
    const char* delims = " \t\r\n\"'.?!-;:()#$&*,0123456789";
      std::vector<WorkItem> WItems;
    char* cstr = new char[content.length() + 1];
    std::strcpy(cstr, content.c_str());

    //track the positions and scan manually
    const char* src = content.c_str();
     size_t len = content.length();
    size_t i = 0;
    while (i < len) {
        // skip delimiters
        while (i < len && std::strchr(delims, src[i])) i++;
        if (i >= len) break;
        // find end of word
         size_t start = i;
        while (i < len && !std::strchr(delims, src[i])) i++;
        std::string word(src + start, i - start);
        if (!word.empty()) {
            WItems.emplace_back(word, (int)start);
        }
    }
    delete[] cstr;
     ThreadSafeQueue<WorkItem> workQ;
    ThreadSafeQueue<ResultItem> resultQ;

    //consumer threads
     std::vector<std::thread> threads;
    for (int t = 0; t < numthreads; t++) {
        threads.emplace_back(workerFunc, std::ref(workQ), std::ref(resultQ));
    }

    //produce so push all work items
    for (auto& item : WItems) {
        workQ.push(item);
    }
    for (int t = 0; t < numthreads; t++) {
        workQ.push({"", 0});
    }
 
    // Collect the results
    std::vector<Token> results;
     results.reserve(WItems.size());
    for (size_t k = 0; k < WItems.size(); k++) {
        results.push_back(resultQ.pop());
    }
 
    // Join the threads
    for (auto& th : threads) th.join();
 
    // Sort results
    std::sort(results.begin(), results.end(), [](const Token& a, const Token& b){
        return a.getStartPos() < b.getStartPos();
    });
 
    tokens = results;

    //making the HTML and syllabized strings
    htmlContents.clear();
    syllabizedContents.clear();
    for (size_t k = 0; k < tokens.size(); k++) {
        if (k > 0) { htmlContents += " "; syllabizedContents += " "; }
        htmlContents        += tokens[k].getHTML();
        syllabizedContents  += tokens[k].getSyllabizedString();
    }
 
    extractDone = true;
}

std::string& Document::gethtmlcontents() const {
    if (!extractDone) {
        htmlContents.clear();
        for (size_t i = 0; i < tokens.size(); i++) {
            if (i > 0) htmlContents += " ";
            htmlContents += tokens[i].getHTML();
        }
    }
    return htmlContents;
}

std::string& Document::getsyllabizedcontents() const {
    if(!extractDone){
        syllabizedContents.clear();
        for (size_t i = 0; i < tokens.size(); i++) {
            if (i > 0) syllabizedContents += " ";
            // compute on the fly if sylBreaks not set
            auto breaks = Syllabizer::syllabize(tokens[i].get());
            std::string word = tokens[i].get();
            for (auto it = breaks.rbegin(); it != breaks.rend(); ++it)
                if (*it > 0 && *it < (int)word.size()) word.insert(*it, "-");
            syllabizedContents += word;
        }
    }
    return syllabizedContents;
}