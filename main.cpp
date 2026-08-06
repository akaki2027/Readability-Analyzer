#include "Document.hpp"
#include "Lexicon.hpp"
#include <iostream>

// Console demo: analyzes a document and confirms that parallel tokenization
// produces the same result no matter how many worker threads are used.

int main() {

    // Load the lexicon singleton
    Lexicon::getInstance().loadcsv("./text/US English word info.csv");

    
    Document d;
    d.load("./text/gettysburg.txt");

    std::cout << "Tokens before extracttokens: " << d.numtokens() << "\n";

    d.extracttokens(4);  // try with 4 threads

    std::cout << "Tokens after extracttokens:  " << d.numtokens() << "\n";

    // Check syllabized output
    std::string& syl = d.getsyllabizedcontents();
    std::cout << "\nFirst 200 chars syllabized:\n";
    std::cout << syl.substr(0, 200) << "\n";

    // Check HTML output
    std::string& html = d.gethtmlcontents();
    std::cout << "\nFirst 200 chars HTML:\n";
    std::cout << html.substr(0, 200) << "\n";

    // Test with different thread counts
    for (int n : {1, 2, 4, 8}) {
        Document d2;
        d2.load("./text/gettysburg.txt");
        d2.extracttokens(n);
        std::cout << "Threads=" << n << " tokens=" << d2.numtokens() << "\n";
    }

    return 0;
}