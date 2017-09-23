#include <iostream>
#include <pugixml.hpp>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <algorithm>
#include <map>

/**
 * Print program usage to stdout
 */
void printUsage() {
    std::cout
            << "ngram - Run Ngram on an xml input of the following form:" << std::endl << std::endl
            << "Usage: ngram [-i input|--]" << std::endl
            << "    -i, --input\t\t\tInput file. Use -- for stdin" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
            << "    -h, --help\t\t\tDisplay this help message" << std::endl;
}

/**
 * Print XML input file template to stdout
 */
void printXmlTemplate() {
    std::cout
            << "<?xml version=\"1.0\"?>" << std::endl
            << "<dialog>" << std::endl
            << "    <s>" << std::endl
            << "        <utt uid=\"1\">Hey, how are you?</utt>" << std::endl
            << "        <utt uid=\"2\">I’m fine thank you!</utt>" << std::endl
            << "        <utt uid=\"1\">Nice!</utt>" << std::endl
            << "    </s>" << std::endl
            << "    <s>" << std::endl
            << "        <utt uid=\"1\">>Who’s around for lunch?</utt>" << std::endl
            << "        <utt uid=\"2\">Me!</utt>" << std::endl
            << "        <utt uid=\"3\">Me, too!</utt>" << std::endl
            << "    </s>" << std::endl
            << "</dialog>" << std::endl;
}

/**
 * Initialize parameters
 * @param argc
 * @param argv
 * @param doc
 * @param res
 */
void initParams(int argc, char *argv[], pugi::xml_document &doc, pugi::xml_parse_result &res) {

    struct option longOptions[] = {
            {"input", required_argument, 0, 'i'},
            {"template", required_argument, 0, 't'},
            {"help",   no_argument,       0, 'h'},
            {0, 0,                        0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hti:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'i':
                if(strcmp(optarg, "--") == 0) {
                    // Read from standard input
                    // e.g. cat file.xml | ngram -i --
                    std::stringstream xmlContent;
                    std::string lineInput;
                    while (getline(std::cin, lineInput)) {
                        xmlContent << lineInput;
                    }
                    res = doc.load_string(xmlContent.str().c_str());
                } else {
                    // Load from file
                    res = doc.load_file(optarg);
                }
                break;
            case 't':
                printXmlTemplate();
                exit(0);
            case 'h':
            default:
                break;
        }
    }
}

/**
 * Filter message
 * Remove punctuations
 * @param uttStr
 */
void filterMessage(std::string &uttStr) {
    // Remove punctuations
    for (size_t i = 0, len = uttStr.size(); i < len; i++) {
        if (ispunct(uttStr[i])) {
            uttStr.erase(i--, 1);
            len = uttStr.size();
        }
    }
}

/**
 * Tokenize a string into a vector of strings
 * @param uttStr
 * @return
 */
std::vector<std::string> tokenizeUtt(const std::string &uttStr) {
    auto start = std::find(uttStr.begin(), uttStr.end(), ' ');
    std::vector<std::string> tokens;
    tokens.push_back("<s>");
    tokens.push_back(std::string(uttStr.begin(), start));
    while (start != uttStr.end()) {
        const auto finish = find(++start, uttStr.end(), ' ');
        tokens.push_back(std::string(start, finish));
        start = finish;
    }
    return tokens;
}

/**
 * Print Ngram table to stdout
 * @param wordWordCounter
 */
void printNgramTable(const std::map<std::string, std::map<std::string, unsigned int>> &wordWordCounter) {
    for(auto wordWord : wordWordCounter) {
        for(auto word : wordWord.second) {
            std::cout << wordWord.first << ":" << word.first << " -> " << word.second << std::endl;
        }
    }
}

void buildNgramTable(pugi::xml_document &doc) {

    // Prepare table
    std::map<std::string, unsigned int> wordCounter;
    std::map<std::string, std::map<std::string, unsigned int>> wordWordCounter;

    // Find dialog tag
    pugi::xml_node dialog = doc.child("dialog");

    // Loop over conversations
    for (pugi::xml_node sTag = dialog.child("s"); sTag; sTag = sTag.next_sibling("s")) {

        // Loop over utterances
        for (pugi::xml_node uttTag = sTag.child("utt"); uttTag; uttTag = uttTag.next_sibling("utt")) {
            std::string uttStr = uttTag.child_value();
            filterMessage(uttStr);
            std::vector<std::string> tokens = tokenizeUtt(uttStr);

            // Increment counter for <s>
            wordCounter[tokens[0]]++;

            // Increment counter for the utt tokens
            for(size_t index=1; index < tokens.size(); index++) {
                wordWordCounter[tokens[index-1]][tokens[index]]++;
                wordCounter[tokens[index]]++;
            }
        }
    }
}

int main(int argc, char** argv) {
    // Prepare xml document
    pugi::xml_document doc;
    pugi::xml_parse_result res;

    // Initialize parameters
    initParams(argc, argv, doc, res);

    // Handle errors
    if(res.status == pugi::status_internal_error) {
        printUsage();
        return 1;
    } else if(res.status == pugi::status_file_not_found) {
        std::cerr << "Input file not found!" << std::endl;
        return 1;
    } else if(res.status != pugi::status_ok) {
        std::cerr << "An error occurred while parsing the input." << std::endl;
        return 1;
    }

    // Build the Ngram table
    buildNgramTable(doc);
    return 0;
}