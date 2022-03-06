#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

namespace json {

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

    struct Token{
        std::string value;
        token_type type;
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
    auto codepoint_to_utf8(int) -> std::string;
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

        unsigned char m_current_char;
        int m_line, m_column;
    };

    class Lexer {
    public:
        Lexer() = default;

        auto read_file(std::string) -> bool;
        auto set_json_string(std::string) -> void;
        auto make_tokens() -> std::vector<json::Token>;
        
    private:
        auto is_alpha(const unsigned char) -> bool;

    private:
        CharStream m_stream;
        std::vector<json::Token> m_tokens;
    };
}