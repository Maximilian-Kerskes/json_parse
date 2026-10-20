#include "json.hpp"

#include <iostream>

int main() {
    const Json document = Parser(R"({
        "name": "Ada",
        "active": true,
        "scores": [98, 97]
    })").parse();

    std::cout << document["name"].get<std::string>() << '\n';
    std::cout << document["scores"][0].get<std::int64_t>() << '\n';
}
