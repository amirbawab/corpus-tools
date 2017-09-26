#include <json.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include <climits>
#include <json2xml/json2xml.h>

void RedditTree::_populate(std::string fileName) {

    // Load json file
    std::ifstream inputJson(fileName);
    nlohmann::json jsonObj;
    inputJson >> jsonObj;

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

void RedditTree::loadBuild(std::string fileName) {
    _populate(fileName);
    _linkNodes();
    for(auto &rootNode : m_rootNodes) {
        _putHeights(rootNode, false);
    }
    _makeThreads();
}

int main() {

    // Create a reddit tree
    RedditTree redditTree;
    redditTree.loadBuild("/tmp/data/data.json");

    // Print a thread
    std::map<int, int> threadSize;
    for(auto vec : redditTree.m_threadNodes) {
        threadSize[vec.size()]++;
    }

    for(auto pair : threadSize) {
        std::cout << "Size " << pair.first << ": " << pair.second << std::endl;
    }

    // Build the reddit tree
    std::cout << "Number of comments: " << redditTree.m_redditNodes.size() << std::endl;
    std::cout << "Number of threads: " << redditTree.m_threadNodes.size() << std::endl;

    return 0;
}