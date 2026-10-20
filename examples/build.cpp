#include "json.hpp"

#include <iostream>

int main() {
    Json document;
    document["name"] = "Ada";
    document["active"] = true;
    document["ports"] = Json::array{8080, 8443};

    std::cout << serialize(document) << '\n';
}
