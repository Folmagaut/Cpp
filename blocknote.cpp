Node LoadNode(std::istream& input) {
    // Skip whitespace
    std::istreambuf_iterator<char> it(input);
    while (std::isspace(*it)) {
        ++it;
    }

    // Check the next token
    if (*it == '[') {
        ++it;
            Array array;
            while (*it != ']') {
                array.push_back(LoadNode(input));
                // Skip comma and whitespace
                while (std::isspace(*it) || *it == ',') {
                    ++it;
                }
            }
            ++it; // Skip ']'
            return Node(std::move(array));
    } else if (*it == '{') {
        ++it;
            Dict map;
            while (*it != '}') {
                // Парсим ключ (строку)
                std::string key; // = LoadString(input);
                ++it;
                    //std::string str;
                    while (*it != '"' && it != std::istreambuf_iterator<char>()) {
                        if (*it == '\\') {
                            ++it;
                            switch (*it) {
                                case '"': key += '"'; break;
                                case '\\': key += '\\'; break;
                                case '/': key += '/'; break;
                                case 'b': key += '\b'; break;
                                case 'f': key += '\f'; break;
                                case 'n': key += '\n'; break;
                                case 'r': key += '\r'; break;
                                case 't': key += '\t'; break;
                                default: throw ParsingError("Invalid escape sequence");
                            }
                        } else {
                            key += *it;
                        }
                        ++it;
                    }
                    if (*it != '"') {
                        throw ParsingError("Unexpected end of string");
                    }
                    ++it;

                // Пропускаем двоеточие
                if (*it != ':') {
                    throw ParsingError("Expected ':' after key");
                }
                ++it;

                // Парсим значение
                Node value = LoadNode(input);

                map[std::move(key)] = std::move(value);

                // Пропускаем запятую, если она есть
                if (*it == ',') {
                    ++it;
                } else if (*it != '}') {
                    throw ParsingError("Expected ',' or '}'");
                }
            }
            ++it; // Пропускаем закрывающую фигурную скобку
            return Node(map);
    } else if (*it == '"') {
        ++it;
            std::string str;
            while (*it != '"' && it != std::istreambuf_iterator<char>()) {
                if (*it == '\\') {
                    ++it;
                    switch (*it) {
                        case '"': str += '"'; break;
                        case '\\': str += '\\'; break;
                        case '/': str += '/'; break;
                        case 'b': str += '\b'; break;
                        case 'f': str += '\f'; break;
                        case 'n': str += '\n'; break;
                        case 'r': str += '\r'; break;
                        case 't': str += '\t'; break;
                        default: throw ParsingError("Invalid escape sequence");
                    }
                } else {
                    str += *it;
                }
                ++it;
            }
            if (*it != '"') {
                throw ParsingError("Unexpected end of string");
            }
            ++it;
            return Node(std::move(str));
    } else if (std::isdigit(*it) || *it == '-') {
        // Handle numbers
        std::string number;
        while (std::isdigit(*it) || *it == '.' || *it == 'e' || *it == 'E' || *it == '-' || *it == '+') {
            number += *it;
            ++it;
        }
        try {
            return Node(std::stod(number));
        } catch (const std::exception& e) {
            throw ParsingError("Invalid number");
        }
    } else if (*it == 't' || *it == 'f' || *it == 'n') {
        // Handle booleans and null
        std::string token;
        while (std::isalpha(*it)) {
            token += *it;
            ++it;
        }
        if (token == "true") {
            return Node(true);
        } else if (token == "false") {
            return Node(false);
        } else if (token == "null") {
            return Node(nullptr);
        } else {
            throw ParsingError("Invalid token");
        }
    } else {
        throw ParsingError("Invalid JSON token");
    }
}