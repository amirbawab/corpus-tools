#include <iostream>
#include <pugixml.hpp>
#include <sstream>
#include <vector>

int main() {

    // Read input
    std::stringstream xmlContent;
    std::string lineInput;
    while (getline(std::cin, lineInput)) {
        xmlContent << lineInput;
    }

    // Parse xml
    pugi::xml_document doc;
    doc.load_string(xmlContent.str().c_str());
    pugi::xml_node dialog = doc.child("dialog");

    // Loop over conversations
    for (pugi::xml_node sTag = dialog.child("s"); sTag; sTag = sTag.next_sibling("s")) {

        // Loop over utterances
        for (pugi::xml_node uttTag = sTag.child("utt"); uttTag; uttTag = uttTag.next_sibling("utt")) {
            std::cout << uttTag.attribute("uid").value() << ": " << uttTag.child_value() << std::endl;
        }
    }

    std::vector<std::vector<int>> wordWordCount;
    return 0;
}