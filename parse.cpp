#include "parse.hpp"

bool json::json_parser::parse(std::vector<json::Token> t){
    tokenList = t;
    index = tokenList.begin();


    if(tokenList[0].type == token_type::curly_bracket_open){        
        index++;

        if(!load_object()){
            return false;
        }
    }

    return true;
}

bool json::json_parser::load_object(){
    while(true){
        if(index->type != json::token_type::string){
            std::cerr << "invalid value " << index->value << " expected string value" << "\n";
            return false;
        }

        if((++index)->type != json::token_type::colen){
            std::cerr << "expected colen after " << std::prev(index)->value << "\n";
            return false;
        }

        std::string &key = std::prev(index)->value;
        
        index++;

        json::value value = load_value();

        container.insert(key, std::move(value));

        index++;

        switch (index->type)
        {
        case json::token_type::comma:
            index++;
            continue;
        
        case json::token_type::curly_bracket_close:
            return true;

        default:
            std::cerr << "unexpected value " << index->value << "\n";
            return false;
            break;
        }

    }
}

json::value json::json_parser::load_array(){
    std::vector<json::value> list;

    while(true){

        index++;
        list.push_back(std::move(load_value()));
        
        switch ((++index)->type)
        {
        case json::token_type::comma:
            continue;
        
        case json::token_type::square_bracket_close:
            return list;

        default:
            std::cerr << "unexpected value " << index->value << "\n";
            return list;
        }
    }
}

json::value json::json_parser::load_value(){
    switch (index->type)
    {
    case json::token_type::string:
        return json::value(index->value);
        break;
    
    case json::token_type::true_value:
        return json::value(true);
        break;

    case json::token_type::false_value:
        return json::value(false);
        break;

    case json::token_type::square_bracket_open:
        return json::value(load_array());
        break;
    
    default:
        return json::value(false);
        break;
    }
}

json::map&& json::json_parser::get_map(){
    return std::move(container);
}