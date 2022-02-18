#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace json{
    enum class token_type{
        curly_bracket_open,
        curly_bracket_close,
        square_bracket_open,
        square_bracket_close,
        comma,
        colen,
        null,
        string,
        true_value,
        false_value,
        end_of_file
    };

    enum class error_type {
        none,
        unexpected_character
    };

    struct Token{
        std::string value;
        token_type type;
    };

    struct Error {
        int line = -1;
        int column = -1;
        error_type error = json::error_type::none;
        std::string error_msg = "";
        std::string error_value = "";
    };


    struct CharStream {
    public:
        CharStream() = default;
        CharStream(std::string);

        auto set_string(std::string) -> void;
        
        auto next() -> const char;
        auto peek() const -> const char;
        
        auto current() const -> const char;
        auto is_end() const -> bool;

        auto line() const -> int;
        auto column() const -> int;

    private:
        std::string m_text_buffer;
        std::string::iterator m_current;
        char m_current_char;
        int m_line, m_column;
    };

    class Lexer {
    public:
        Lexer() = default;

        auto read_file(std::string) -> bool;
        auto set_json_string(std::string) -> void;
        auto make_tokens() -> std::vector<json::Token>;

        auto success() -> bool;
        auto error() -> Error;
        
    private:
        auto is_alpha(const char) -> bool;
        auto error_type_to_string(json::error_type) -> std::string;
        auto make_error(json::error_type, const char) -> Error;

    private:
        CharStream m_stream;
        Error m_error;
        std::vector<json::Token> m_tokens;
    };
}