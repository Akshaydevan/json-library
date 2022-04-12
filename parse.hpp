#pragma once

#include <iostream>
#include <vector>

#include "lex.hpp"
#include "map.hpp"

namespace json {
class json_parser {
public:
    json_parser() = default;

    json::value parse(std::vector<json::Token>);
    json::value load_object();
    json::value load_array();
    json::value load_value();

private:
    std::vector<json::Token> tokenList;
    std::vector<json::Token>::iterator index;
};

} // namespace json
