#include "../include/libjson/map.hpp"

json::Value json::Value::operator[](std::string key)
{
    auto map = std::get<std::map<std::string, Value>>(*this);

    return map[key];
}

json::Value json::Value::operator[](int index)
{
    auto list = std::get<std::vector<Value>>(*this);

    return list[index];
}

std::string json::Value::asString()
{
    return std::get<std::string>(*this);
}

bool json::Value::asBool()
{
    return std::get<bool>(*this);
}

float json::Value::asNum()
{
    return std::get<float>(*this);
}
