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
    bool m_caseInsensitive;
public:
    Ngram(pugi::xml_document *doc):m_caseInsensitive(false), m_doc(doc) {}
    ~Ngram() {delete m_doc;}

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
    void buildNgramTable();

    /**
     * Report the possible next words
     */
    std::vector<std::pair<std::string, double>> whatsNext(std::string word, double threshold = 0.0);

    /**
     * Set case insensitive
     */
    void setCaseInsensitive(bool caseInsensitive) {m_caseInsensitive = caseInsensitive;}

    /**
     * Check if case insensitive
     */
    bool isCaseInsensitive() const {return m_caseInsensitive;}
};
