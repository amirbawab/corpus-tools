#include <iostream>
#include <json.hpp>
#include <algorithm>
#include <queue>
#include <climits>
#include <fstream>
#include <json2xml/RedditTree.h>

// Import global variable
extern bool g_showStat;

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

        // Store node in map
        m_redditNodes[redditNode->m_id] = redditNode;

        // Store author
        if(m_authorIds.find(redditNode->m_author) == m_authorIds.end()) {
            m_authorIds[redditNode->m_author] = m_authorIds.size()+1;
        }
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

void RedditTree::buildConversations(int minSize, int maxSize) {
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
        if(path.size() >= minSize && path.size() <= maxSize) {
            m_conversationNodes.push_back(path);
        }

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
            outputXML << "        <utt uid=\"" << m_authorIds[utterance->m_author] << "\">"
                      << utterance->m_body << "</utt>" << std::endl;
        }
        outputXML << "    </s>" << std::endl;
    }
    outputXML << "</dialog>";
    outputXML.close();
    return true;
}

/*************
 * HEURISTICS
 ************/

std::function<void(std::shared_ptr<RedditNode>)> RedditTree::WEIGHT_LONGEST_PATH = [&](std::shared_ptr<RedditNode> node) {
    if(node) {
        int maxHeight = -1;
        for(auto &child : node->m_childrenNodes) {
            RedditTree::WEIGHT_LONGEST_PATH(child);
            maxHeight = std::max(maxHeight, child->m_weight);
        }
        node->m_weight = maxHeight + 1;
    }
};

// Heuristic #2: Check shortest path
std::function<void(std::shared_ptr<RedditNode>)> RedditTree::WEIGHT_SHORTEST_PATH = [&](std::shared_ptr<RedditNode> node) {
    if(node) {
        int maxHeight = 1;
        for(auto &child : node->m_childrenNodes) {
            RedditTree::WEIGHT_SHORTEST_PATH(child);
            maxHeight = std::min(maxHeight, child->m_weight);
        }
        node->m_weight = maxHeight - 1;
    }
};
