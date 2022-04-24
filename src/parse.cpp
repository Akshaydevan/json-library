#include "../include/libjson/parse.hpp"

json::Value json::JsonParser::parse(std::vector<json::Token> t)
{
    m_tokenList = t;
    m_index = m_tokenList.begin();
    json::Value obj;

    if (m_tokenList[0].type == token_type::curly_bracket_open) {
        obj = load_object();
    } else if (m_tokenList[0].type == token_type::square_bracket_open) {
        obj = load_array();
    }

    return obj;
}

json::Value json::JsonParser::load_object()
{
    std::map<std::string, json::Value> obj;

    m_index++;

    // checking for empty object
    if (m_index->type == json::token_type::curly_bracket_close) {
        return json::Value(obj);
    }

    while (true) {
        if (m_index->type != json::token_type::string) {
            std::cerr << "invalid value inside object " << m_index->valueAsString()
                      << " expected string value"
                      << "\n";
            throw std::runtime_error("encountered error");
        }

        if ((++m_index)->type != json::token_type::colen) {
            std::cerr << "expected colen after " << std::prev(m_index)->valueAsString()
                      << "\n";
            throw std::runtime_error("encountered error");
        }

        auto item = std::prev(m_index)->value;
        std::string& key = std::get<std::string>(item);

        m_index++;

        json::Value value = load_value();
        obj.insert({ key, value });

        m_index++;

        switch (m_index->type) {
        case json::token_type::comma:
            m_index++;
            continue;

        case json::token_type::curly_bracket_close:
            return json::Value(obj);

        default:
            std::cerr << "unexpected value inside object " << m_index->valueAsString()
                      << "\n";
            throw std::runtime_error("encountered error");
        }
    }

    return json::Value(obj);
}

json::Value json::JsonParser::load_array()
{
    std::vector<json::Value> list;

    m_index++;

    // checking for empty array
    if (m_index->type == json::token_type::square_bracket_close) {
        return json::Value(list);
    }

    while (true) {
        list.push_back(std::move(load_value()));

        switch ((++m_index)->type) {
        case json::token_type::comma:
            m_index++;
            continue;

        case json::token_type::square_bracket_close:
            return list;

        default:
            std::cerr << "unexpected value inside array " << m_index->valueAsString()
                      << "\n";
            throw std::runtime_error("encountered error");
        }
    }
}

json::Value json::JsonParser::load_value()
{
    switch (m_index->type) {
    case json::token_type::string:
        return json::Value(m_index->valueAsString());
        break;

    case json::token_type::true_value:
        return json::Value(true);
        break;

    case json::token_type::false_value:
        return json::Value(false);
        break;

    case json::token_type::null:
        return json::Value(nullptr);
        break;

    case json::token_type::square_bracket_open:
        return json::Value(load_array());
        break;

    case json::token_type::number:
        return json::Value(std::get<float>(m_index->value));
        break;

    case json::token_type::curly_bracket_open:
        return json::Value(load_object());
        break;

    default:
        std::cerr << "invalid value";
        throw std::runtime_error("encountered error");
    }
}
