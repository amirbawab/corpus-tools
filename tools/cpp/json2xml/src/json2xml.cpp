#include <json.hpp>
#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <map>
#include <json2xml/json2xml.h>

int main() {

    // Load json file
    std::ifstream inputJson("/tmp/data/data.json");
    nlohmann::json jsonObj;
    inputJson >> jsonObj;

    // Create node map
    std::map<std::string, std::shared_ptr<RedditNode>> redditMap;

    // Get reddit array
    auto redditArray = jsonObj["reddit"];
    for(auto redditObject : redditArray) {
        std::shared_ptr<RedditNode> redditNode = std::make_shared<RedditNode>(RedditNode{
                redditObject["body"],
                redditObject["author"],
                redditObject["created_utc"],
                redditObject["subreddit_id"],
                redditObject["link_id"],
                redditObject["parent_id"],
                redditObject["score"],
                redditObject["id"],
        });
        redditMap[redditNode->m_id] = redditNode;
    }
    std::cout << redditMap.size() << std::endl;

    return 0;
}