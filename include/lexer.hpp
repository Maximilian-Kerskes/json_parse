#pragma once

#include <cctype>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>

enum class TokenType {
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    COLON,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_TYPE,
	END,
};

struct Token {
	TokenType type;
	std::string text;
    std::size_t position;
};

class Lexer {
public:
	explicit Lexer(std::string_view input) : input_(input) {}

	Token next() {
		skip_whitespace();

		if (at_end()) {
			return {TokenType::END, {}, position_};
		}

		const auto start = position_;
		const char c = advance();

		switch (c) {
			case '{': return {TokenType::LEFT_BRACE, {}, start};
			case '}': return {TokenType::RIGHT_BRACE, {}, start};
			case '[': return {TokenType::LEFT_BRACKET, {}, start};
			case ']': return {TokenType::RIGHT_BRACKET, {}, start};
			case ',': return {TokenType::COMMA, {}, start};
			case ':': return {TokenType::COLON, {}, start};
			case '"': return read_string(start);
			case 't': { read_keyword("rue", start); return {TokenType::TRUE, {}, start}; };
			case 'f': { read_keyword("alse", start); return {TokenType::FALSE, {}, start}; };
			case 'n': { read_keyword("ull", start); return {TokenType::NULL_TYPE, {}, start}; };
			default:
				if (c == '-' || std::isdigit(static_cast<unsigned char>(c))) {
					return read_number(start);
				}

                throw std::runtime_error(
                    "Unexpected character at position " + 
                    std::to_string(start)
                );
		}
	}

private:
	std::string_view input_;
	std::size_t position_ = 0;

	bool at_end() const {
		return position_ >= input_.size();
	}

	char peek() const {
		return at_end() ? '\0' : input_[position_];
	}

	char advance() {
		return input_[position_++];
	}

	void skip_whitespace() {
		while (!at_end() && std::isspace(static_cast<unsigned char>(peek()))) {
			++position_;
		}
	}

	Token read_string(std::size_t start) {
		std::string text;
		while(!at_end() && peek() != '"') {
            text += advance();
        }
        if (at_end()) {
            throw std::runtime_error("Unterminated string");
        }
        advance();
        return {TokenType::STRING, text, start};
	}

	Token read_number(std::size_t start) {
		const auto number_start = start;

		while (!at_end()) {
			const char c = peek();

			if (std::isdigit(static_cast<unsigned char>(c)) || c == '.' || c == '-' || c == '+') {
				advance();
			} else {
				break;
			}
		}

		return {
	TokenType::NUMBER,
			std::string(input_.substr(number_start, position_ - number_start)),
			start
		};
	}

	void read_keyword(std::string_view keyword_remaineder, std::size_t start) {
		for (char expected : keyword_remaineder) {
    		if (at_end() || advance() != expected) {
				throw std::runtime_error(
					"Invalid keyword at position " + 
					std::to_string(start)
				);
			}
		}
	}
		
};
