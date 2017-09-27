#pragma once

#include <string>
#include <memory>
#include <vector>

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
};
