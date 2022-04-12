#pragma once

#include <iostream>
#include <vector>

#include "lex.hpp"
#include "map.hpp"

namespace json {
class JsonParser {
public:
    JsonParser() = default;

    json::Value parse(std::vector<json::Token>);
    json::Value load_object();
    json::Value load_array();
    json::Value load_value();

private:
    std::vector<json::Token> m_tokenList;
    std::vector<json::Token>::iterator m_index;
};

} // namespace json
