#include <json.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <climits>
#include <unistd.h>
#include <getopt.h>
#include <json2xml/json2xml.h>

// Global variables
std::string g_inputFileName;
std::string g_outputFileName;

/**
 * Print program usage to stdout
 */
void printUsage() {
    std::cout
            << "json2xml - Convert JSON of a specific format to XML" << std::endl << std::endl
            << "Usage: json2xml -i <input file> -o <output file>" << std::endl
            << "    -i, --input\t\t\tInput file. Use -- for stdin" << std::endl
            << "    -t, --template\t\tDisplay XML template" << std::endl
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

bool RedditTree::_populate(std::string fileName) {

    // Load json file
    std::ifstream inputJson(fileName);
    if(!inputJson.is_open()) {
        std::cerr << "Could not open input file" << std::endl;
        return false;
    }
    nlohmann::json jsonObj;
    if(inputJson >> jsonObj) {
        std::cerr << "Could not validate JSON" << std::endl;
        return false;
    }

    // Populate the reddit tree
    auto redditArray = jsonObj["reddit"];
    for(auto redditObject : redditArray) {
        // Clean parent id
        std::string parendId = redditObject["parent_id"];
        parendId = parendId.substr(parendId.find("_") + 1);

        // Clean link id
        std::string linkId = redditObject["link_id"];
        linkId = linkId.substr(linkId.find("_") + 1);

        // Create reddit node
        std::shared_ptr<RedditNode> redditNode = std::make_shared<RedditNode>(
                redditObject["body"],
                redditObject["author"],
                redditObject["created_utc"],
                redditObject["subreddit_id"],
                linkId,
                parendId,
                redditObject["score"],
                redditObject["id"]
        );
        m_redditNodes[redditNode->m_id] = redditNode;
    }
    return true;
}

void RedditTree::_linkNodes() {

    // Link the nodes
    for(auto redditNodePair : m_redditNodes) {

        // If parent is found, build connection
        if(!redditNodePair.second->isRoot() &&
                m_redditNodes.find(redditNodePair.second->m_parentId) != m_redditNodes.end()){
            std::shared_ptr<RedditNode> parentNode = m_redditNodes[redditNodePair.second->m_parentId];
            parentNode->m_childrenNodes.push_back(redditNodePair.second);
            redditNodePair.second->m_parentNode = parentNode;
        } else if(redditNodePair.second->isRoot() || !redditNodePair.second->m_parentNode) {
            m_rootNodes.push_back(redditNodePair.second);
        }
    }
}

//TODO Rename this to _putWeight, and min=INT_MIN
void RedditTree::_putHeights(std::shared_ptr<RedditNode> &root, bool max) {
    if(root) {
        int minMaxHeight = -1;
        if(!max) {
            minMaxHeight = INT_MAX;
        }
        for(auto &child : root->m_childrenNodes) {
            _putHeights(child, max);
            if(max) {
                minMaxHeight = std::max(minMaxHeight, child->m_height);
            } else {
                minMaxHeight = std::min(minMaxHeight, child->m_height);
            }
        }
        root->m_height = minMaxHeight + 1;
    }
}

void RedditTree::_makeThreads() {
    std::queue<std::shared_ptr<RedditNode>> queue;

    // Push all roots to the queue
    for(auto redditNode : m_rootNodes) {
        queue.push(redditNode);
    }

    // Start generating the threads
    while(!queue.empty()) {

        // Fetch and pop front node
        const std::shared_ptr<RedditNode> &front = queue.front();
        queue.pop();

        // Extract path
        std::vector<std::shared_ptr<RedditNode>> path = _extractPath(front);

        // Add longest path to the threads
        m_threadNodes.push_back(path);

        // Note: The last node in the path should not have any children, otherwise
        // there exist a path that is longer then the current extracted one.
        for(size_t pathIndex=0; pathIndex < path.size()-1; pathIndex++) {
            for(std::shared_ptr<RedditNode> &child : path[pathIndex]->m_childrenNodes) {
                if(child != path[pathIndex+1]) {
                    queue.push(child);
                }
            }
        }
    }
}

std::vector<std::shared_ptr<RedditNode>> RedditTree::_extractPath(std::shared_ptr<RedditNode> root) {
    std::vector<std::shared_ptr<RedditNode>> nodes;
    nodes.push_back(root);
    while (!nodes.back()->m_childrenNodes.empty()) {
        for(size_t i=0; i < nodes.back()->m_childrenNodes.size(); i++) {
            if(nodes.back()->m_childrenNodes[i]->m_height == nodes.back()->m_height-1) {
                nodes.push_back(nodes.back()->m_childrenNodes[i]);
                break;
            }
        }
    }
    return nodes;
}

bool RedditTree::generateXML(std::string fileName) {
    std::cout << ">> Generating XML: " << fileName << std::endl;
    std::ofstream outputXML(fileName);
    if(!outputXML.is_open()) {
        std::cerr << "Could not open output file" << std::endl;
        return false;
    }
    outputXML << "<?xml version=\"1.0\"?>" << std::endl
              << "<dialog>" << std::endl;
    for(auto &conversation : m_threadNodes) {
        outputXML << "    <s>" << std::endl;
        for(auto &utterance : conversation) {
            outputXML << "        <utt>" << utterance->m_body << "</utt>" << std::endl;
        }
        outputXML << "    </s>" << std::endl;
    }
    outputXML << "</dialog>";
    outputXML.close();
    return true;
}

bool RedditTree::loadBuild(std::string fileName) {
    std::cout << ">> Converting comments to a forest" << std::endl;
    if(!_populate(fileName)) {
        return false;
    }

    std::cout << ">> Linking parent and children nodes" << std::endl;
    _linkNodes();

    std::cout << ">> Assigning weights to nodes" << std::endl;
    for(auto &rootNode : m_rootNodes) {
        _putHeights(rootNode, false);
    }

    std::cout << ">> Building conversations" << std::endl;
    _makeThreads();
    return true;
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
            {"template", no_argument, 0, 't'},
            {"help",   no_argument,       0, 'h'},
            {0, 0,                        0, 0}
    };

    int optionIndex = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hti:o:", longOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'i':
                g_inputFileName = optarg;
                break;
            case 'o':
                g_outputFileName = optarg;
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
    if(!redditTree.loadBuild(g_inputFileName)) {
        return 1;
    }

    // Generate XML
    if(!redditTree.generateXML(g_outputFileName)) {
        return 1;
    }

    return 0;
}