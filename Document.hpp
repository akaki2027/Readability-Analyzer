// Document.hpp     Aditya Kaki  Jan 28 2026
//  header file for the Document class

#ifndef DOCUMENT_HPP
#define DOCUMENT_HPP
#include <string> 
#include <vector>
#include <map>
#include "Token.hpp" 
#include "Lexicon.hpp" 


class Document {
    private:
        std::string txtname;
 	std::string content;
	std::vector<Token> tokens;
	std::map<int,std::map<std::string,int>> tuple_data;

	mutable std::string htmlContents;
	mutable std::string syllabizedContents;
	mutable bool extractDone = false;

	void preprocess(std::string& s);
	void tokenize();

    public:/*! \brief Initializing the Constructor. \param introcontent Optional string to start the document content.*/
        Document(std::string introcontent = "");//assigned empty
		/*! \brief The Copy constructor. */
		Document(const Document& other);
		/*! \brief The Assignment operator. */
	Document& operator=(const Document& other);
	/*! \brief The Equality operator, is true if filename and content match. */
	bool operator==(const Document& other) const;
	/*! \brief Returns the raw content string. */
	std::string getrawcontent() const { return content; }

	/*! \brief Load the text file. \param filename Full path to file. */
	int load(std::string filename);
	int loadContentsOnly(std::string filename);
	void processTokens();
	/*! \brief The number of tokens in the document. */
		size_t numtokens() const;
		/*! \brief The total syllable count across tokens. */
		int totalsyllables() const;
		/*! \brief Alias for totalsyllables(). */
		int numsyllables() const;
		/*! \brief Returns the number of sentences. */
		int numsentences() const;
		/*! \brief Returns the number of hard words with more than 2 syllables. */
		int nhardwords() const;
		/*! \brief Alias for nhardwords(). */
		int numhardwords() const;
		/*! \brief Returns the flesch reading Ease score. */
		double getfkgradelevel() const;
		/*! \brief Returns the gunning fog reading Ease score. */
		double getgfgradelevel() const;

		/*! \brief Returns reading ease score */
		double getreadingease() const;
		/*! \brief Alias for getfkgradelevel(). */
		double getfleschkincaid() const;
		/*! \brief Alias for getgfgradelevel(). */
		double getgunningfog() const;
		/*! \brief finds word tuple frequency. \param length Tuple length. \return Number of tuples found. */
		int computecounts(int length);
		/*! \brief Retrieves specific tuple data by index.
		 * \param length The length of tuples to look in.
		 * \param num The index of the tuple to retrieve.
		 * \param tuple Output parameter for the tuple string.
		 * \param count Output parameter for the frequency count.
		 * \return 0 if successful, non-zero if index is out of bounds.
		 */
		int gettuplecount(int length, int num, std::string& tuple, int& count);

			/*! \brief Returns the number of unique words. */
	int numuniquewords() const;
 
	/*! \brief Set lexicon info on all tokens. */
	void setlexinfo(Lexicon* lex);
 
	/*! \brief Get the full document as decorated HTML. */
	std::string getHTML() const;

	/*! \brief Tokenizes the document across a pool of worker threads, computing
	 * syllable breaks and lexicon info for each token. Must be called before
	 * gethtmlcontents() or getsyllabizedcontents().
	 * \param numthreads Number of worker threads to use (clamped to 1-64).
	 */
	void extracttokens(int numthreads);

	/*! \brief The document rendered as HTML, built by extracttokens(). */
	std::string& gethtmlcontents() const;

	/*! \brief The document with syllable breaks marked, built by extracttokens(). */
	std::string& getsyllabizedcontents() const;

};

#endif 