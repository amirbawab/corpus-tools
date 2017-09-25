#include <json.hpp>
#include <iostream>
#include <fstream>
#include <json2xml/json2xml.h>


int main() {

    // Load json file
    std::ifstream inputJson("/tmp/data/data.json");
    nlohmann::json jsonObj;
    inputJson >> jsonObj;

    // Create a reddit tree
    RedditTree redditTree;

    // Populate the reddit tree
    auto redditArray = jsonObj["reddit"];
    for(auto redditObject : redditArray) {

        // Clean parent id
        std::string parendId = redditObject["parent_id"];
        parendId = parendId.substr(parendId.find("_")+1);
        std::cout << parendId << std::endl;

        // Create reddit node
        std::shared_ptr<RedditNode> redditNode = std::make_shared<RedditNode>(RedditNode{
                redditObject["body"],
                redditObject["author"],
                redditObject["created_utc"],
                redditObject["subreddit_id"],
                redditObject["link_id"],
                parendId,
                redditObject["score"],
                redditObject["id"],
        });
        redditTree.m_redditNodes[redditNode->m_id] = redditNode;
    }

    // Link the nodes
    for(auto redditNodePair : redditTree.m_redditNodes) {

        // If parent is found
        if(redditTree.m_redditNodes.find(redditNodePair.second->m_parentId) != redditTree.m_redditNodes.end()){
            std::shared_ptr<RedditNode> parentNode = redditTree.m_redditNodes[redditNodePair.second->m_parentId];
            parentNode->m_childrenNodes.push_back(redditNodePair.second);
            redditNodePair.second->m_parentNode = parentNode;
        }
    }

    // Build the reddit tree
    std::cout << redditTree.m_redditNodes.size() << std::endl;

    return 0;
}