#pragma once

struct RedditNode {
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
};
