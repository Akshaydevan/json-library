#pragma once

#include <iostream>
#include <vector>
#include "lex.hpp"
#include "map.hpp"

namespace json{    
    class json_parser{
        public:
            json_parser() = default;
            
            bool parse(std::vector<json::Token>);
            json::value load_object();
            json::value load_array();
            json::value load_value();

            json::map&& get_map();
            
        private:
            std::vector<json::Token> tokenList;
            std::vector<json::Token>::iterator index;
            map container;
    };

}
