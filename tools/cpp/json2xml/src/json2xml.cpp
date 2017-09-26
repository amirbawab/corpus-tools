#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <json2xml/RedditNode.h>
#include <json2xml/RedditTree.h>

// Global variables
std::string g_inputFileName;
std::string g_outputFileName;
bool g_showStat = false;

/**
 * Print program usage to stdout
 */
void printUsage() {
    std::cout
            << "json2xml - Convert JSON of a specific format to XML" << std::endl << std::endl
            << "Usage: json2xml -i <input file> -o <output file>" << std::endl
            << "    -i, --input\t\t\tInput file. Use -- for stdin" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
            << "    -s, --stat\t\t\tShow statistics" << std::endl
            << "    -h, --help\t\t\tDisplay this help message" << std::endl;
}

/**
 * Print JSON input file template to stdout
 */
void printXmlTemplate() {
    std::cout
            << "{" << std::endl
            << "    \"reddit\": [" << std::endl
            << "        {" << std::endl
            << "             \"body\": \"\"," << std::endl
            << "             \"author\": \"\"," << std::endl
            << "             \"created_utc\": \"\"," << std::endl
            << "             \"subreddit_id\": \"\"," << std::endl
            << "             \"link_id\": \"\"," << std::endl
            << "             \"parent_id\": \"\"," << std::endl
            << "             \"score\": \"\"," << std::endl
            << "             \"id\": \"\"" << std::endl
            << "        }" << std::endl
            << "    ]"  << std::endl
            << "}" << std::endl;
}


/**
 * Initialize parameters
 * @param argc
 * @param argv
 * @param doc
 * @param res
 */
void initParams(int argc, char *argv[]) {

    struct option longOptions[] = {
            {"input", required_argument, 0, 'i'},
            {"output", required_argument, 0, 'o'},
            {"stat", no_argument, 0, 's'},
            {"template", no_argument, 0, 't'},
            {"help",   no_argument,       0, 'h'},
            {0, 0,                        0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "htsi:o:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'i':
                g_inputFileName = optarg;
                break;
            case 'o':
                g_outputFileName = optarg;
                break;
            case 's':
                g_showStat = true;
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

    // Init params
    initParams(argc, argv);

    // Input and output are required
    if(g_inputFileName.empty() || g_outputFileName.empty()) {
        printUsage();
        return 1;
    }

    // Create a reddit tree
    RedditTree redditTree;

    std::cout << ">> Convert JSON object to class objects" << std::endl;
    if(!redditTree.load(g_inputFileName)) {
        return false;
    }

    std::cout << ">> Linking parent and children nodes" << std::endl;
    redditTree.linkNodes();

    std::cout << ">> Assigning weights to nodes" << std::endl;
    redditTree.putWeights(RedditTree::WEIGHT_SHORTEST_PATH);

    std::cout << ">> Building conversations" << std::endl;
    redditTree.buildConversations();

    // Generate XML
    if(!redditTree.generateXML(g_outputFileName)) {
        return 1;
    }

    return 0;
}