#include "lexer.hpp"
#include "parser.hpp"
#include "serializer.hpp"
#include "value.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void check(bool condition, const std::string &message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Function>
void check_throws(Function &&function, const std::string &message) {
    try {
        function();
    } catch (const std::exception &) {
        return;
    }

    throw std::runtime_error(message);
}

void test_values() {
    Json null;
    check(null.holds<std::nullptr_t>(), "default Json should be null");

    Json object;
    object["name"] = "John";
    object["age"] = 30;

    check(object.holds<Json::object>(),
          "object access should create an object");
    check(object["name"].get<std::string>() == "John", "string value mismatch");
    check(object["age"].get<std::int64_t>() == 30, "integer value mismatch");

    const Json &readonly = object;
    check(readonly["name"].get<std::string>() == "John", "const access failed");
    check_throws([&] { readonly["missing"]; },
                 "missing const key should throw");
}

void test_lexer() {
    Lexer lexer(R"({"name":"John","age":30,"active":true})");
    const std::vector<TokenType> expected{
        TokenType::LEFT_BRACE,  TokenType::STRING, TokenType::COLON,
        TokenType::STRING,      TokenType::COMMA,  TokenType::STRING,
        TokenType::COLON,       TokenType::NUMBER, TokenType::COMMA,
        TokenType::STRING,      TokenType::COLON,  TokenType::TRUE,
        TokenType::RIGHT_BRACE, TokenType::END,
    };

    for (TokenType type : expected) {
        check(lexer.next().type == type, "unexpected token type");
    }
}

void test_parser() {
    const Json value = Parser(R"({
        "name": "John",
        "age": 30,
        "score": 3.5,
        "active": true,
        "nothing": null,
        "tags": ["cpp", "json"]
    })")
                           .parse();

    check(value["name"].get<std::string>() == "John", "parsed string mismatch");
    check(value["age"].get<std::int64_t>() == 30, "parsed integer mismatch");
    check(value["score"].get<double>() == 3.5, "parsed double mismatch");
    check(value["active"].get<bool>(), "parsed boolean mismatch");
    check(value["nothing"].holds<std::nullptr_t>(), "parsed null mismatch");
    check(value["tags"].get<Json::array>().size() == 2,
          "parsed array mismatch");

    check(Parser("[1, 2, 3]").parse().get<Json::array>().size() == 3,
          "root array parsing failed");
    check(Parser("null").parse().holds<std::nullptr_t>(),
          "root null parsing failed");

    check_throws([] { Parser(R"({"missing":})").parse(); },
                 "invalid value should throw");
    check_throws([] { Parser(R"({"missing" 1})").parse(); },
                 "missing colon should throw");
}

void test_serializer() {
    Json value;
    value["name"] = "John";
    value["message"] = "hello\nworld";
    value["items"] = Json::array{1, 2, 3};
    value["active"] = true;
    value["nothing"] = nullptr;

    const std::string text = serialize(value);
    check(
        text ==
            R"({"active":true,"items":[1,2,3],"message":"hello\nworld","name":"John","nothing":null})",
        "serialized JSON mismatch: " + text);

    std::ofstream output("tests/resources/serialized.json");
    check(output.is_open(), "could not open serializer output file");
    output << text;
    output.close();

    std::ifstream written("tests/resources/serialized.json");
    check(written.is_open(), "could not reopen serializer output file");
    std::stringstream contents;
    contents << written.rdbuf();
    check(contents.str() == text, "serializer output file mismatch");
}

void test_file() {
    std::ifstream file("tests/resources/testFile.json");
    check(file.is_open(), "could not open testFile.json");

    std::stringstream contents;
    contents << file.rdbuf();

    const Json value = Parser(contents.str()).parse();
    const Json &person = value["person"];

    check(person["name"].get<std::string>() == "John Doe",
          "testFile.json name mismatch");
    check(person["age"].get<double>() == 3.124, "testFile.json age mismatch");
    check(person["address"]["zipcode"].get<std::int64_t>() == 12345,
          "testFile.json zipcode mismatch");
    check(person["hobbies"].get<Json::array>().size() == 3,
          "testFile.json hobbies mismatch");
}

} // namespace

int main() {
    try {
        test_values();
        test_lexer();
        test_parser();
        test_serializer();
        test_file();
    } catch (const std::exception &error) {
        std::cerr << "TEST FAILURE: " << error.what() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "All tests passed\n";
    return EXIT_SUCCESS;
}
