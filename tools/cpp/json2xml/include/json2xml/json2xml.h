#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>

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
    int m_height = 0;
    bool isRoot() const {return m_linkId == m_parentId;}
    bool isLeaf() const {return m_childrenNodes.empty();}
};

class RedditTree {
public:
    std::map<std::string, std::shared_ptr<RedditNode>> m_redditNodes;
    std::vector<std::shared_ptr<RedditNode>> m_rootNodes;
    std::vector<std::vector<std::shared_ptr<RedditNode>>> m_threadNodes;
    void loadBuild(std::string fileName);
private:
    std::vector<std::shared_ptr<RedditNode>> _extractPath(std::shared_ptr<RedditNode> root);
    void _linkNodes();
    void _populate(std::string fileName);
    void _makeThreads();
    void _putHeights(std::shared_ptr<RedditNode> &root, bool max);
};