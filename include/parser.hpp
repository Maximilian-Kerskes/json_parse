#pragma once

#include "lexer.hpp"
#include "value.hpp"
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
class Parser {
public:
	explicit Parser(std::string_view input) : lexer_(input), current_(lexer_.next()) {}

	Json parse() {
		Json result = parse_value();

		if (current_.type != TokenType::END) {
			error("Expected end of input");
		}

		return result;
	}

private:
    Lexer lexer_;
    Token current_;

	void advance() {
		current_ = lexer_.next();
	}

	void expect(TokenType type) {
		if (current_.type != type) {
			error("Unexpected token");
		}

		advance();
	}

	[[noreturn]]
	void error(const std::string& message) const {
		throw std::runtime_error(message + " at position " + std::to_string(current_.position));
	}

	Json parse_value() {
		switch (current_.type) {
			case TokenType::LEFT_BRACE: return parse_object();
            case TokenType::LEFT_BRACKET: return parse_array();
			case TokenType::STRING: {
				std::string value = current_.text;
				advance();
				return Json(std::move(value));
			}
            case TokenType::NUMBER: {
				return parse_number();
            }
            case TokenType::TRUE: {
                advance();
                return Json(true);
            }
            case TokenType::FALSE: {
                advance();
                return Json(false);
            }
            case TokenType::NULL_TYPE: {
                advance();
                return Json(nullptr);
            }
            default: error("Unexpected token");
		}
	}

	Json parse_object() {
		Json::object result;
		expect(TokenType::LEFT_BRACE);

		if (current_.type == TokenType::RIGHT_BRACE) {
			advance();
			return Json(std::move(result));
		}

		while (true) {
			if (current_.type != TokenType::STRING) {
                error("Expected object key");
			}

			std::string key = current_.text;
			advance();

            expect(TokenType::COLON);
			result.emplace(std::move(key), parse_value());

			if (current_.type == TokenType::RIGHT_BRACE) {
                advance();
				break;
            }

			expect(TokenType::COMMA);
		}

		return Json(std::move(result));
	}

	Json parse_array() {
		Json::array result;
		expect(TokenType::LEFT_BRACKET);

		if (current_.type == TokenType::RIGHT_BRACKET) {
            advance();
            return Json(std::move(result));
        }

		while (true) {
			result.push_back(parse_value());

			if (current_.type == TokenType::RIGHT_BRACKET) {
                advance();
                break;
            }

			expect(TokenType::COMMA);
		}

        return Json(std::move(result));
	}

	Json parse_number() {
		const std::string text = current_.text;
		advance();

		if (text.find_first_of(".") != std::string::npos) {
			return Json(std::stod(text));
		}

		return Json(static_cast<std::int64_t>(std::stoll(text)));
	}
};
