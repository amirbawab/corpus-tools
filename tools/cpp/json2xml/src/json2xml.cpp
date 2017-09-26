#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <json2xml/RedditNode.h>
#include <json2xml/RedditTree.h>

// Global variables
std::string g_inputFileName;
std::string g_outputFileName;
bool g_showStat = false;
int g_weightAlgorithm = RedditTree::WEIGHT_SHORTEST_PATH_NUM;

/**
 * Print program usage to stdout
 */
void printUsage() {
    std::cout
            << "json2xml - Convert JSON of a specific format to XML" << std::endl << std::endl
            << "Usage: json2xml -i <input file> -o <output file>" << std::endl
            << "    -i, --input\t\t\tInput file" << std::endl
            << "    -o, --output\t\tOutput file" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
            << "    -w, --weight\t\tAlgorithm used to set the nodes weights: (default=1)  2" << std::endl
            << "                \t\t1) For every tree T in a forest F:" << std::endl
            << "                \t\t     Select in T the shortest path P" << std::endl
            << "                \t\t     Add P to the list of conversations" << std::endl
            << "                \t\t     For every node N in P:" << std::endl
            << "                \t\t       For every child node C of N:" << std::endl
            << "                \t\t         Add C to F" << std::endl
            << "                \t\t2) For every tree T in a forest F:" << std::endl
            << "                \t\t     Select in T the longest path P" << std::endl
            << "                \t\t     Add P to the list of conversations" << std::endl
            << "                \t\t     For every node N in P:" << std::endl
            << "                \t\t       For every child node C of N:" << std::endl
            << "                \t\t         Add C to F" << std::endl
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
            {"weight", required_argument, 0, 'w'},
            {"template", no_argument, 0, 't'},
            {"help",   no_argument,       0, 'h'},
            {0, 0,                        0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "htsi:o:w:", longOptions, &optionIndex)) != -1) {
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
            case 'w':
                try{
                    g_weightAlgorithm = std::stoi(std::string(optarg));
                }catch (std::invalid_argument){
                    // Do nothing
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
        return 1;
    }

    std::cout << ">> Linking parent and children nodes" << std::endl;
    redditTree.linkNodes();

    std::cout << ">> Assigning weights to nodes" << std::endl;
    switch (g_weightAlgorithm) {
        case RedditTree::WEIGHT_LONGEST_PATH_NUM:
            redditTree.putWeights(RedditTree::WEIGHT_LONGEST_PATH);
            break;
        case RedditTree::WEIGHT_SHORTEST_PATH_NUM:
        default:
            redditTree.putWeights(RedditTree::WEIGHT_SHORTEST_PATH);
            break;
    }

    std::cout << ">> Building conversations" << std::endl;
    redditTree.buildConversations();

    // Generate XML
    if(!redditTree.generateXML(g_outputFileName)) {
        std::cerr << "Could not write tot output file: " << g_outputFileName << std::endl;
        return 1;
    }

    return 0;
}