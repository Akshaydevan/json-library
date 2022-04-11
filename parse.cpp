#include "parse.hpp"

json::value json::json_parser::parse(std::vector<json::Token> t){
    tokenList = t;
    index = tokenList.begin();
    json::value obj;

    if(tokenList[0].type == token_type::curly_bracket_open){        
        obj = load_object();
    }
    else if(tokenList[0].type == token_type::square_bracket_open) {
        obj = load_array();
    }

    return obj;
}

json::value json::json_parser::load_object(){
    std::map<std::string, json::value> obj;

    index++;

    //checking for empty object
    if (index->type == json::token_type::curly_bracket_close) {
        return json::value(obj);
    }

    while(true){

        if(index->type != json::token_type::string){
            std::cerr << "invalid value inside object " << index->valueAsString() << " expected string value" << "\n";
            throw std::runtime_error("encountered error");
        }

        if((++index)->type != json::token_type::colen){
            std::cerr << "expected colen after " << std::prev(index)->valueAsString() << "\n";
            throw std::runtime_error("encountered error");
        }

        auto item = std::prev(index)->value;
        std::string &key = std::get<std::string>(item);

        index++;

        json::value value = load_value();
        obj.insert({key, value});

        index++;


        switch (index->type)
        {
        case json::token_type::comma:
            index++;
            continue;
        
        case json::token_type::curly_bracket_close:
            return json::value(obj);

        default:
            std::cerr << "unexpected value inside object " << index->valueAsString() << "\n";
            throw std::runtime_error("encountered error");
        }

    }

    return json::value(obj);
}

json::value json::json_parser::load_array(){
    std::vector<json::value> list;

    index++;

    //checking for empty array
    if (index->type == json::token_type::square_bracket_close) {
        return json::value(list);
    }

    while(true){
        list.push_back(std::move(load_value()));
        
        switch ((++index)->type)
        {
        case json::token_type::comma:
            index++;
            continue;
        
        case json::token_type::square_bracket_close:
            return list;

        default:
            std::cerr << "unexpected value inside array " << index->valueAsString() << "\n";
            throw std::runtime_error("encountered error");
        }
    }
}

json::value json::json_parser::load_value(){
    switch (index->type)
    {
    case json::token_type::string:
        return json::value(index->valueAsString());
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

    case json::token_type::number:
        return json::value(std::get<float>(index->value));
        break;

    case json::token_type::curly_bracket_open:
        return json::value(load_object());
        break;
    
    default:
        std::cerr << "invalid value";
        throw std::runtime_error("encountered error");
    }
}
