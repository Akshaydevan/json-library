#include "../include/libjson/json.hpp"

json::Value js::read(const std::string& jsontext)
{
    json::Lexer lexer;
    json::JsonParser parser;

    std::vector<json::Token> tokens;
    json::Value value;

    lexer.set_json_string(jsontext);

    tokens = lexer.make_tokens();
    value = parser.parse(tokens);

    return value;
}
