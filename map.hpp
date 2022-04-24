#pragma once
#include <iostream>
#include <map>
#include <memory>
#include <variant>
#include <vector>

namespace json {
class Value;

using value_type = std::variant<float, bool, std::nullptr_t, std::string, std::vector<Value>,
    std::map<std::string, Value>>;

class Value : public json::value_type {
public:
    using json::value_type::value_type;
    using json::value_type::operator=;

    json::Value operator[] (std::string);
    json::Value operator[] (int);

    std::string asString();
    bool asBool();
    float asNum();
};
} // namespace json
