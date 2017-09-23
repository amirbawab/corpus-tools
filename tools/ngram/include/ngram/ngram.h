#pragma once

#include <pugixml.hpp>
#include <sstream>
#include <vector>
#include <map>

class Ngram {
private:
    pugi::xml_document *m_doc;
    std::map<std::string, unsigned int> m_wordCounter;
    std::map<std::string, std::map<std::string, unsigned int>> m_wordWordCounter;
public:

    /**
     * Constructor
     */
    Ngram(pugi::xml_document *doc);

    /**
     * Destructor
     */
    ~Ngram();

    /**
     * Remove punctuations
     */
    void filterMessage(std::string &uttStr);

    /**
     * Tokenize a string into a vector of strings
     */
    std::vector<std::string> tokenizeUtt(const std::string &uttStr);

    /**
     * Print Ngram table to stdout
     */
    void printNgramTable();

    /**
     * Print word occurrence
     */
    void printWordCount();

    /**
     * Build Ngram table
     */
    void buildNgramTable(bool caseInsensitive);
};
