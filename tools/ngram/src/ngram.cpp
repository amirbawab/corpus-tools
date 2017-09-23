#include <iostream>
#include <pugixml.hpp>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

void printUsage() {
    std::cout
            << "ngram - Run Ngram on an xml input of the following form:" << std::endl << std::endl
            << "Usage: ngram [-i input|--]" << std::endl
            << "    -i, --input\t\t\tInput file. Use -- for stdin" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
            << "    -h, --help\t\t\tDisplay this help message" << std::endl;
}

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

    // Find dialog tag
    pugi::xml_node dialog = doc.child("dialog");

    // Loop over conversations
    for (pugi::xml_node sTag = dialog.child("s"); sTag; sTag = sTag.next_sibling("s")) {

        // Loop over utterances
        for (pugi::xml_node uttTag = sTag.child("utt"); uttTag; uttTag = uttTag.next_sibling("utt")) {
            std::cout << uttTag.attribute("uid").value() << ": " << uttTag.child_value() << std::endl;
        }
    }

    std::vector<std::vector<int>> wordWordCount;
    return 0;
}