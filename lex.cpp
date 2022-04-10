#include "lex.hpp"

json::CharStream::CharStream(std::string s)
    :m_text_buffer(s)
    ,m_current(s.begin())
    ,m_line(1)
    ,m_column(1)
{
    
}

void json::CharStream::set_string(std::string s) {
    m_text_buffer = s;

    m_current = m_text_buffer.begin();

    m_line = 1;
    m_column = 1;
}

const unsigned char json::CharStream::next () {
    char current_char = *m_current;

    m_current = std::next(m_current);

    if (current_char == '\n') {
        m_line++;
        m_column = 0;
    }
    else {
        m_column++;
    }

    return current_char;
}

const unsigned char json::CharStream::current() const {
    if (m_current == m_text_buffer.begin()) {
        return *m_current;
    }
    
    return *std::prev(m_current);
}

const unsigned char json::CharStream::peek() const {
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

std::string json::Token::valueAsString() {
    switch(this->type) {
    case json::token_type::curly_bracket_open:
    case json::token_type::curly_bracket_close:
    case json::token_type::square_bracket_open:
    case json::token_type::square_bracket_close:
    case json::token_type::comma:
    case json::token_type::colen:
    case json::token_type::null:
    case json::token_type::string:
        return std::get<std::string>(this->value);

    case json::token_type::number:{
        std::stringstream stream;
        stream << std::get<float>(this->value);

        return stream.str();
    }

    case json::token_type::true_value:
        return "true";

    case json::token_type::false_value:
        return "false";

    case json::token_type::end_of_file:
        return "eof";
    }
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
            m_tokens.push_back(Token{"{", json::token_type::curly_bracket_open});
            break;

        case '}':
            m_tokens.push_back(Token{"}", json::token_type::curly_bracket_close});
            break;

        case '[':
            m_tokens.push_back(Token{"[", json::token_type::square_bracket_open});
            break;

        case ']':
            m_tokens.push_back(Token{"]", json::token_type::square_bracket_close});
            break;

        case ',':
            m_tokens.push_back(Token{",", json::token_type::comma});
            break;

        case ':':
            m_tokens.push_back(Token{":", json::token_type::colen});
            break;

        case '"':
        {
            std::string buffer = "";

            while (!m_stream.is_end()) {
                const unsigned char c = m_stream.next();
                
                /*
                 * checkign if double quotes (' " ') is escaped as it should not
                 * be treated as end of string. It also checks if back slash (' \ ')
                 * is escaped so that the second black slash is not treated as another
                 * escape sequence.
                 */
                if (c == '\\' && (m_stream.peek() == '\"' || m_stream.peek() == '\\')) {
                    buffer += c;
                    buffer += '"';
                    m_stream.next();
                    continue;
                }

                if (c == '"') {
                    break;
                }

                buffer += c;
            }

            if (m_stream.current() != '"') {
                
                throw Error(m_stream.line(), m_stream.column(), "expected '\"' at the end of string");
            }

            std::string encoded_buffer;

            for (auto i = buffer.begin(); i != buffer.end(); i++) {
                unsigned char c = *i;

                if (c == '\\') {
                    if (std::next(i) == buffer.end()){
                        throw Error(m_stream.line(), m_stream.column(), "abrupt end of escape sequence");
                    }
                
                    if (*++i == 'u') {
                        if (buffer.end() - i+1 < 5) {
                            throw Error(m_stream.line(), m_stream.column(), "malformed unicode escape sequence");
                        }

                        //check for surrogate pair
                        if (buffer.end() - i + 1 >= 10) {

                            if (*(i + 5) == '\\' && *(i + 6) == 'u') {
                                std::string full_string = std::string(i + 1, i + 11);

                                encoded_buffer += encode_utf_escape_sequence(std::string(i+1, i + 11));
                                
                                i += 10;
                                continue;
                            }
                        }

                        encoded_buffer += encode_utf_escape_sequence(std::string(i+1, i+5));
                        i += 4;
                        continue;
                    }
                    else {
                        //adding escape character to buffer

                        encoded_buffer += c;
                        encoded_buffer += *i;
                    }
                }
                else {
                    encoded_buffer += c;
                }
            }

            m_tokens.push_back(Token{encoded_buffer, json::token_type::string});
            break;
        }

        case 't':
        {
            std::string buffer = "t";
            
            while (is_alpha(m_stream.peek()) ) {
                const unsigned char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "true") {
                m_tokens.push_back(Token{buffer, json::token_type::true_value});
            }
            else {
                throw Error(m_stream.line(), m_stream.column(), "unexpected character sequence " + buffer);
                return m_tokens;
            }

            break;
        }
        
        case 'f':
        {
            std::string buffer = "f";

            while (is_alpha(m_stream.peek())) {
                const unsigned char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "false") {
                m_tokens.push_back(Token{buffer, json::token_type::false_value});
            }
            else {
                throw Error(m_stream.line(), m_stream.column(), "unexpected character sequence " + buffer);
                
                return m_tokens;
            }

            break;
        }

        case 'n':
        {
            std::string buffer = "n";

            while (is_alpha(m_stream.peek()) ) {
                const unsigned char c = m_stream.next();

                buffer += c;
            }

            if (buffer == "null") {
                m_tokens.push_back(Token{buffer, json::token_type::null});
            }
            else {
                throw Error(m_stream.line(), m_stream.column(), "unexpected character sequence " + buffer);

                return m_tokens;
            }

            break;
        }

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
        case '.':
        {
            std::string number;

            number += m_stream.current();

            while (!m_stream.is_end() && json::is_numeric(m_stream.peek())) {
                number.push_back(m_stream.next());
            }

            if (parse_integer(number)) {
                std::stringstream stream(number);
                float num;
                stream >> num;

                m_tokens.push_back(Token{num, json::token_type::number});
            }
            else {
                throw Error(m_stream.line(), m_stream.column(), "invalid number");
            }

            break;
        }

        case ' ':
            break;

        case '\n':
            break;

        default: {
            std::string err_msg = "unexpected character ";
            err_msg.push_back(m_stream.current());

            throw Error(m_stream.line(), m_stream.column(), err_msg);

            return m_tokens;
            }
        }
    }

    return m_tokens;
}


auto json::encode_utf_escape_sequence(std::string unicode_string) -> std::string{
    int codepoint1 = hexstring_to_int(unicode_string);
    

    if (unicode_string.length() > 4) {
        int codepoint2 = hexstring_to_int(std::string(unicode_string.begin() + 6, unicode_string.end()));
    
        //convert utf16 surrogate pair to code-point
        codepoint1 -= 0xD800;
        codepoint2 -= 0xDc00;

        codepoint1 = codepoint1 << 10;

        int final_codepoint = codepoint1 | codepoint2;
        final_codepoint += 0x10000;

        return json::codepoint_to_utf8(final_codepoint);
    }


    return json::codepoint_to_utf8(codepoint1);
}

auto json::hexstring_to_int(std::string hex_string) -> int {
    int codepoint = 0;

    for (auto i = hex_string.begin(); i < hex_string.begin() + 4; i++) {
        codepoint *= 16;

        switch (*i) {
        case '0': codepoint += 0; break;
        case '1': codepoint += 1; break;
        case '2': codepoint += 2; break;
        case '3': codepoint += 3; break;
        case '4': codepoint += 4; break;
        case '5': codepoint += 5; break;
        case '6': codepoint += 6; break;
        case '7': codepoint += 7; break;
        case '8': codepoint += 8; break;
        case '9': codepoint += 9; break;

        case 'a':
        case 'A':
            codepoint += 10; break;

        case 'b':
        case 'B':
            codepoint += 11; break;

        case 'c':
        case 'C':
            codepoint += 12; break;

        case 'd':
        case 'D':
            codepoint += 13; break;

        case 'e':
        case 'E':
            codepoint += 14; break;

        case 'f':
        case 'F':
            codepoint += 15; break;

        default:
            throw std::runtime_error("invalid character");
        }
    }

    return codepoint;
}

auto json::parse_integer(std::string s) -> bool
{
    //[[plus or minus] [number] [fraction]] [[e] [plus or minus] [number]]

    std::string integer, fraction, exponent;
    bool canReadFraction = true;

    int i = 0;

    //can have plus or minus at start
    if (s[0] == '+' || s[0] == '-') {
        integer.push_back(s[0]);

        if (s.length() <= 1) {
            return false;
        }

        i++;
    }

    for (; s.begin() + i != s.end(); i++) {
        char c = s[i];

        switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            integer.push_back(c);
            break;

        case '.':
            if (canReadFraction) {
                integer.push_back(c);
            }
            else {
                return false;
            }

            break;

        case 'e':
        {
            if (s.length() - (i + 1) == 0) {
                return false;
            }

            integer.push_back(c);

            if (s[i + 1] == '+' || s[i + 1] == '-') {
                integer.push_back(s[++i]);

                if (s.length() - (i + 1) == 0) {
                    return false;
                }
            }

            //exponent cannot be a float
            canReadFraction = false;

            break;
        }

        default:
            return false;
        }
    }

    //check for overflow using stringstream
    std::stringstream stream(integer);
    double res;

    if (!(stream >> res)) {
        return false;
    }


    return true;
}

std::string json::codepoint_to_utf8(int codepoint) {
    std::string s;

    if (codepoint <= 0x7F) {
        // Plain ASCII

        s.resize(1);

        s[0] = (char)codepoint;
    }
    else if (codepoint <= 0x07FF) {
        // 2-byte unicode

        s.resize(2);

        s[0] = (char)(((codepoint >> 6) & 0x1F) | 0xC0);
        s[1] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    }
    else if (codepoint <= 0xFFFF) {
        // 3-byte unicode

        s.resize(3);
        s[0] = (char)(((codepoint >> 12) & 0x0F) | 0xE0);
        s[1] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        s[2] = (char)(((codepoint >> 0) & 0x3F) | 0x80);

    }
    else if (codepoint <= 0x10FFFF) {
        // 4-byte unicode

        s.resize(4);
        s[0] = (char)(((codepoint >> 18) & 0x07) | 0xF0);
        s[1] = (char)(((codepoint >> 12) & 0x3F) | 0x80);
        s[2] = (char)(((codepoint >> 6) & 0x3F) | 0x80);
        s[3] = (char)(((codepoint >> 0) & 0x3F) | 0x80);
    }

    return s;
}

bool json::is_alpha (const unsigned char c) {
    if (c >= 97 && c <= 122) {
        return true;
    }
    else {
        return false;
    }

}

bool json::is_numeric (const unsigned char c) {
    if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.' || c == 'e' || c == 'E') {
        return true;
    }
    else {
        return false;
    }
}

json::Error::Error(int l, int c, std::string s)
    :m_line(l)
    ,m_column(c)
    ,m_error_msg(s)
{

}

const char* json::Error::what() const throw()
{
    return m_error_msg.c_str();
}

int json::Error::line()
{
    return m_line;
}

auto json::Error::column() -> int
{
    return m_column;
}
