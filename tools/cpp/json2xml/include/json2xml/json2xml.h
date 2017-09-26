#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>

class RedditNode {
public:
    RedditNode(std::string body, std::string author, std::string createdUtc, std::string subredditId,
               std::string linkId, std::string parentId, std::string score, std::string id) :
            m_body(body), m_author(author), m_createdUtc(createdUtc), m_subredditId(subredditId), m_linkId(linkId),
            m_parentId(parentId), m_score(score), m_id(id){}
    std::string m_body;
    std::string m_author;
    std::string m_createdUtc;
    std::string m_subredditId;
    std::string m_linkId;
    std::string m_parentId;
    std::string m_score;
    std::string m_id;
    std::shared_ptr<RedditNode> m_parentNode;
    std::vector<std::shared_ptr<RedditNode>> m_childrenNodes;
    int m_weight = 0;
    bool isRoot() const {return m_linkId == m_parentId;}
    bool isLeaf() const {return m_childrenNodes.empty();}
};

class RedditTree {
public:

    // Map all nodes to their unique id
    std::map<std::string, std::shared_ptr<RedditNode>> m_redditNodes;

    // Store all the nodes that are root
    std::vector<std::shared_ptr<RedditNode>> m_rootNodes;

    // Store conversations
    std::vector<std::vector<std::shared_ptr<RedditNode>>> m_conversationNodes;

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
    void buildConversations();

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