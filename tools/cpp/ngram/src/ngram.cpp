#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <algorithm>
#include <ngram/ngram.h>

// Global flags
bool g_printCounter = false;
bool g_caseInsensitive = false;
std::string g_word;
double g_threshold = 0.0;

void Ngram::filterMessage(std::string &uttStr) {
    // Remove punctuations
    for (size_t i = 0, len = uttStr.size(); i < len; i++) {
        if (ispunct(uttStr[i])) {
            uttStr.erase(i--, 1);
            len = uttStr.size();
        }
    }
}

std::vector<std::string> Ngram::tokenizeUtt(const std::string &uttStr) {
    auto start = std::find(uttStr.begin(), uttStr.end(), ' ');
    std::vector<std::string> tokens;
    tokens.push_back("<s>");
    tokens.push_back(std::string(uttStr.begin(), start));
    while (start != uttStr.end()) {
        const auto finish = find(++start, uttStr.end(), ' ');
        tokens.push_back(std::string(start, finish));
        start = finish;
    }
    tokens.push_back("</s>");
    return tokens;
}

void Ngram::printNgramTable() {
    std::cout << "Ngram table" << std::endl
              << "-----------" << std::endl;
    for(auto wordWord : m_wordWordCounter) {
        for(auto word : wordWord.second) {
            std::cout << "count(" << wordWord.first << ", " << word.first << ") = " << word.second << std::endl;
        }
    }
    std::cout << "---" << std::endl;
}

void Ngram::printWordCount() {
    std::cout << "Ngram word occurrence" << std::endl
              << "---------------------" << std::endl;
    for(auto word : m_wordCounter) {
        std::cout << "count(" << word.first << ") = " << word.second << std::endl;
    }
    std::cout << "---" << std::endl;
}

void Ngram::buildNgramTable() {

    // Find dialog tag
    pugi::xml_node dialog = m_doc->child("dialog");

    // Loop over conversations
    for (pugi::xml_node sTag = dialog.child("s"); sTag; sTag = sTag.next_sibling("s")) {

        // Loop over utterances
        for (pugi::xml_node uttTag = sTag.child("utt"); uttTag; uttTag = uttTag.next_sibling("utt")) {
            std::string uttStr = uttTag.child_value();
            filterMessage(uttStr);

            // Check if the letter case is important
            if(m_caseInsensitive) {
                std::transform(uttStr.begin(), uttStr.end(), uttStr.begin(), ::tolower);
            }
            std::vector<std::string> tokens = tokenizeUtt(uttStr);

            // Increment counter for <s>
            m_wordCounter[tokens[0]]++;

            // Increment counter for the utt tokens
            for(size_t index=1; index < tokens.size(); index++) {
                m_wordWordCounter[tokens[index-1]][tokens[index]]++;
                m_wordCounter[tokens[index]]++;
            }
        }
    }
}

std::vector<std::pair<std::string, double>> Ngram::whatsNext(std::string word, double threshold) {

    // Check if case insensitive
    if(m_caseInsensitive) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    }

    // Prepare sorted vector
    std::vector<std::pair<std::string, double>> nextWords;

    // Check if word exist in the database
    if(m_wordCounter.find(word) != m_wordCounter.end()) {
        for(auto &pair : m_wordWordCounter[word]) {
            double prob = (double)m_wordWordCounter[word][pair.first] / m_wordCounter[word];
            if(prob >= threshold) {
                nextWords.push_back(std::pair<std::string, double>(
                        pair.first, prob));
            }
        }
    }

    // Sort words based on confidence level
    sort(nextWords.begin(), nextWords.end(), [=](std::pair<std::string, double>& a, std::pair<std::string, double>& b){
         return a.second < b.second;
    });
    return nextWords;
}

/**
 * Print program usage to stdout
 */
void printUsage() {
    std::cout
            << "ngram - Run Ngram on an xml input" << std::endl << std::endl
            << "Usage: ngram [-i input|--]" << std::endl
            << "    -i, --input\t\t\tInput file. Use -- for stdin" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
            << "    -c, --counter\t\tDisplay Ngram results" << std::endl
            << "    -I, --insensitive\t\tCase insensitive" << std::endl
            << "    -w, --word\t\t\tWhat the next value after this word" << std::endl
            << "    -T, --threshold\t\tProbability threshold" << std::endl
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
            {"template", no_argument, 0, 't'},
            {"counter", no_argument, 0, 'c'},
            {"insensitive", no_argument, 0, 'I'},
            {"word", no_argument, 0, 'w'},
            {"threshold", required_argument, 0, 'T'},
            {"help",   no_argument,       0, 'h'},
            {0, 0,                        0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hcIti:w:T:", longOptions, &optionIndex)) != -1) {
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
            case 'c':
                g_printCounter = true;
                break;
            case 'I':
                g_caseInsensitive = true;
                break;
            case 'w':
                g_word = optarg;
                break;
            case 'T':
                try {
                    g_threshold = std::stod(optarg);
                } catch (std::invalid_argument) {
                    // Keep default value
                }
                break;
            case 'h':
            default:
                break;
        }
    }
}



int main(int argc, char** argv) {
    // Prepare xml document
    pugi::xml_document *doc = new pugi::xml_document;
    pugi::xml_parse_result res;

    // Initialize parameters
    initParams(argc, argv, *doc, res);

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
    Ngram ngram(doc);
    ngram.setCaseInsensitive(g_caseInsensitive);
    ngram.buildNgramTable();

    // Print results if requested
    if(g_printCounter) {
        ngram.printNgramTable();
        std::cout << std::endl;
        ngram.printWordCount();
        std::cout << std::endl;
    }

    // Word occurrence
    if(!g_word.empty()) {
        std::cout << "Possible word(s) after " << g_word << std::endl
                  << "---------------------" << std::endl;
        auto result = ngram.whatsNext(g_word, g_threshold);
        for(auto pair : result) {
            std::cout << "P(" << pair.first <<" | " << g_word << ") = " << pair.second << std::endl;
        }
    }
    return 0;
}