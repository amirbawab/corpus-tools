#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <json2xml/RedditNode.h>

class RedditTree {
public:

    // Map all nodes to their unique id
    std::map<std::string, std::shared_ptr<RedditNode>> m_redditNodes;

    // Store all the nodes that are root
    std::vector<std::shared_ptr<RedditNode>> m_rootNodes;

    // Store conversations
    std::vector<std::vector<std::shared_ptr<RedditNode>>> m_conversationNodes;

    // Heuristic numbers
    static const int WEIGHT_SHORTEST_PATH_NUM=1;
    static const int WEIGHT_LONGEST_PATH_NUM=2;

    // Heuristic #1: Check longest path
    static std::function<void(std::shared_ptr<RedditNode>)> WEIGHT_LONGEST_PATH;

    // Heuristic #2: Check shortest path
    static std::function<void(std::shared_ptr<RedditNode>)> WEIGHT_SHORTEST_PATH;

    /**
     * Generate XML file based on the conversations
     */
    bool generateXML(std::string fileName);

    /**
     * Link parent and children nodes
     */
    void linkNodes();

    /**
     * Load JSON file into nodes
     */
    bool load(std::string fileName);

    /**
     * Build conversations
     */
    void buildConversations(int minSize, int maxSize);

    /**
     * Assign weights to nodes
     */
    void putWeights(std::function<void(std::shared_ptr<RedditNode> node)> weightFunction);
private:

    /**
     * Extract path from a given tree
     * Note: When extracting a conversation path from a tree,
     * the child with the HIGHEST weight will be selected
     */
    std::vector<std::shared_ptr<RedditNode>> _extractPath(std::shared_ptr<RedditNode> root);
};
