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

bool RedditTree::load(std::string fileName) {

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

void RedditTree::linkNodes() {

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

void RedditTree::putWeights(std::function<void(std::shared_ptr<RedditNode> node)> weightFunction) {
    // Run passed function on all the root nodes (root node of each tree in the forest)
    for(std::shared_ptr<RedditNode> &node : m_rootNodes) {
        weightFunction(node);
    }
}

void RedditTree::buildConversations() {
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
        m_conversationNodes.push_back(path);

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

    // The current node is part of the path
    nodes.push_back(root);

    // While a leaf node was not added to the path
    while (!nodes.back()->m_childrenNodes.empty()) {

        // Iterate over all children nodes
        std::shared_ptr<RedditNode> selectedChild = nullptr;
        for(std::shared_ptr<RedditNode> &childNode : nodes.back()->m_childrenNodes) {
            if(selectedChild == nullptr || childNode->m_weight > selectedChild->m_weight) {
                selectedChild = childNode;
            }
        }

        // If a child was selected
        if(selectedChild != nullptr) {
            nodes.push_back(selectedChild);
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
    outputXML << "<?xml version=\"1.0\"?>" << std::endl;
    outputXML<< "<dialog>" << std::endl;

    // Add statistics tag
    if(g_showStat) {

        // Computer the number of conversations
        std::map<int, int> conversationSizeMap;
        for(const auto &conversation : m_conversationNodes) {
            conversationSizeMap[conversation.size()]++;
        }

        outputXML << "    <statistics total=\"" << m_conversationNodes.size() << "\">" << std::endl;
        for(auto conversationPair : conversationSizeMap) {
            outputXML << "        <conversation size=\"" << conversationPair.first
                      << "\" count=\"" << conversationPair.second << "\"/>" << std::endl;
        }
        outputXML << "    </statistics>" << std::endl;
    }

    for(auto &conversation : m_conversationNodes) {
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

    // Heuristic #1: Check longest path
    std::function<void(std::shared_ptr<RedditNode>)> longestPath = [&](std::shared_ptr<RedditNode> node) {
        if(node) {
            int maxHeight = -1;
            for(auto &child : node->m_childrenNodes) {
                longestPath(child);
                maxHeight = std::max(maxHeight, child->m_weight);
            }
            node->m_weight = maxHeight + 1;
       }
    };

    // Assign weight for all nodes
    redditTree.putWeights(longestPath);

    std::cout << ">> Building conversations" << std::endl;
    redditTree.buildConversations();

    // Generate XML
    if(!redditTree.generateXML(g_outputFileName)) {
        return 1;
    }

    return 0;
}