#pragma once
#include <iostream>
#include <memory>
#include <map>
#include <variant>
#include <vector>

namespace json{
    class value;

    using value_type = std::variant<float, bool, std::string, std::vector<value>, std::map<std::string, value>>;

    class value : public json::value_type
    {
        public:
            using json::value_type::value_type;
            using json::value_type::operator=;
    };
}
