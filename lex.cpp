#include "lex.hpp"

json::CharStream::CharStream(std::string s)
    :m_text_buffer(s)
    ,m_current(s.begin())
    ,m_current_char(*m_current)
    ,m_line(1)
    ,m_column(1)
{
    
}

void json::CharStream::set_string(std::string s) {
    m_text_buffer = s;

    m_current = m_text_buffer.begin();
    m_current_char = *m_current;

    m_line = 1;
    m_column = 1;
}

const char json::CharStream::next () {
    m_current = std::next(m_current);
    
    m_current_char = *std::prev(m_current);

    if (m_current_char == '\n') {
        m_line++;
        m_column = 0;
    }
    else {
        m_column++;
    }

    return m_current_char;
}

const char json::CharStream::current() const {
    return m_current_char;
}

const char json::CharStream::peek() const {
    return *m_current;
}

bool json::CharStream::is_end () const {
    if (m_current == m_text_buffer.end()) {
        return true;
    }
    else {
        return false;
    }
}

auto json::CharStream::line() const -> int {
    return m_line;
}

auto json::CharStream::column() const -> int {
    return m_column;
}

bool json::Lexer::read_file (std::string file_name) {
    std::fstream file(file_name);

    if (!file.is_open()) {
        return false;
    }
    
    std::string buffer = "";

    while (file) {
        std::string line;

        std::getline(file, line);

        line += "\n";

        buffer += line;
    }

    m_stream.set_string(buffer);
    return true;
}

void json::Lexer::set_json_string(std::string text) {
    m_stream.set_string(text);
}

std::vector<json::Token> json::Lexer::make_tokens() {
    while (!m_stream.is_end()) {

        switch (m_stream.next())
        {
        case '{':
            m_tokens.push_back(Token("{", json::token_type::curly_bracket_open));
            break;

        case '}':
            m_tokens.push_back(Token("}", json::token_type::curly_bracket_close));
            break;

        case '[':
            m_tokens.push_back(Token("[", json::token_type::square_bracket_open));
            break;

        case ']':
            m_tokens.push_back(Token("]", json::token_type::square_bracket_close));
            break;

        case ',':
            m_tokens.push_back(Token(",", json::token_type::comma));
            break;

        case ':':
            m_tokens.push_back(Token(":", json::token_type::colen));
            break;

        case '"':
        {
            std::string buffer = "";

            while (!m_stream.is_end()) {
                const char c = m_stream.next();
                
                if (c == '"') {
                    break;
                }

                buffer += c;
            }

            m_tokens.push_back(Token(buffer, token_type::string));
            break;
        }

        case 't':
        {
            std::string buffer = "t";
            
            while (is_alpha(m_stream.peek()) ) {
                const char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "true") {
                m_tokens.push_back(Token(buffer, token_type::true_value));
            }
            else {
                m_error = make_error(json::error_type::unexpected_character, m_stream.peek());

                return m_tokens;
            }

            break;
        }
        
        case 'f':
        {
            std::string buffer = "f";

            while (is_alpha(m_stream.peek())) {
                const char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "false") {
                m_tokens.push_back(Token(buffer, token_type::false_value));
            }
            else {
                m_error = make_error(json::error_type::unexpected_character, m_stream.peek());
                
                return m_tokens;
            }

            break;
        }

        case 'n':
        {
            std::string buffer = "n";

            while (is_alpha(m_stream.peek()) ) {
                const char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "null") {
                m_tokens.push_back(Token(buffer, token_type::null));
            }
            else {
                m_error = make_error(json::error_type::unexpected_character, m_stream.peek());
                
                return m_tokens;
            }

            break;
        }

        case ' ':
            break;

        case '\n':
            break;

        default:
            m_error = make_error(json::error_type::unexpected_character, m_stream.current());

            return m_tokens;
        }
    }

    return m_tokens;
}

bool json::Lexer::success() {
    if (m_error.error == json::error_type::none) {
        return true;
    }
    else {
        return false;
    }
}

json::Error json::Lexer::error() {
    return m_error;
}


bool json::Lexer::is_alpha(const char c) {
    if (c >= 97 && c <= 122) {
        return true;
    }
    else {
        return false;
    }

}

std::string json::Lexer::error_type_to_string(json::error_type e) {
    switch (e)
    {
    case json::error_type::none:
        return std::string("none");
        break;

    case json::error_type::unexpected_character:
        return std::string("unexpected_character");
        break;
    }
}

json::Error json::Lexer::make_error(json::error_type etype, const char c) {
    Error error{
        m_stream.line()
       ,m_stream.column()
       ,etype
       ,error_type_to_string(etype)
       ,std::string{c}
    };

    return error;
}