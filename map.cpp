#include "map.hpp"

void json::map::insert(std::string k, json::value v){
    objectMap.insert(std::make_pair(k, std::move(v)));
}

