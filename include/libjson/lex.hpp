#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace json {

enum class token_type {
    curly_bracket_open,
    curly_bracket_close,
    square_bracket_open,
    square_bracket_close,
    comma,
    colen,
    null,
    string,
    number,
    true_value,
    false_value,
    end_of_file
};

struct Token {
    std::variant<float, bool, std::string> value;
    token_type type;

    int line, column;
    std::string valueAsString();
};

class Error : public std::exception {
public:
    Error() = default;
    Error(int, int, std::string);

    const char* what() const throw();

    auto line() -> int;
    auto column() -> int;

private:
    int m_line = -1;
    int m_column = -1;
    std::string m_error_msg = "";
};

auto hexstring_to_int(std::string) -> int;
auto parse_integer(std::string) -> bool;
auto codepoint_to_utf8(int) -> std::string;
auto is_alpha(const unsigned char) -> bool;
auto is_numeric(const unsigned char) -> bool;
auto encode_utf_escape_sequence(std::string) -> std::string;

struct CharStream {
public:
    CharStream() = default;
    CharStream(std::string);

    auto set_string(std::string) -> void;

    auto next() -> const unsigned char;
    auto peek() const -> const unsigned char;

    auto current() const -> const unsigned char;
    auto is_end() const -> bool;

    auto line() const -> int;
    auto column() const -> int;

private:
    std::string m_text_buffer;
    std::string::iterator m_current;

    int m_line, m_column;
};

class Lexer {
public:
    Lexer() = default;

    auto read_file(std::string) -> bool;
    auto set_json_string(std::string) -> void;
    auto make_tokens() -> std::vector<json::Token>;

private:
    CharStream m_stream;
    std::vector<json::Token> m_tokens;
};
} // namespace json
