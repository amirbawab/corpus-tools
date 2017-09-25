#include <json.hpp>
#include <iostream>
#include <fstream>

int main() {
    // read a JSON file
    std::ifstream inputJson("/tmp/data2.json");
    nlohmann::json jsonObj;
    inputJson >> jsonObj;

    // write prettified JSON to another file
    std::ofstream o("/tmp/pretty.json");
    o << std::setw(4) << jsonObj << std::endl;
    return 0;
}