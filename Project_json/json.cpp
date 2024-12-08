#include "json.h"

using namespace std;

namespace json {

namespace {
Node LoadNode(std::istream& input);

std::string LoadString(std::istream& input) {
    // Пропускаем начальные пробелы
    std::istreambuf_iterator<char> it(input);
    while (std::isspace(*it)) {
        ++it;
    }
    // Проверяем, что первый символ - двойная кавычка
    if (*it != '"') {
        throw ParsingError("Expected '\"' at the beginning of the string");
    }
    ++it;

    std::string str;
    while (*it != '"' && it != std::istreambuf_iterator<char>()) {
        if (*it == '\\') {
            ++it;
            switch (*it) {
                case '"': str += '"'; break;
                case '\\': str += '\\'; break;
                case '/': str += '/'; break;
                //case 'b': str += '\b'; break;
                //case 'f': str += '\f'; break;
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

    // Проверяем, что строка завершается двойной кавычкой
    if (*it != '"') {
        throw ParsingError("Unexpected end of string");
    }
    ++it;

    return str;
}

Node LoadStringNode(std::istream& input) {
    std::string str = LoadString(input);
    return Node(std::move(str));
}

json::Node LoadNumber(std::istream& input) {
    std::istreambuf_iterator<char> it(input);
    // Ищем числа и проверяем на int или double
    std::string number;
    bool is_int = true;
    while (std::isdigit(*it) || *it == '.' || *it == 'e' || *it == 'E' || *it == '-' || *it == '+') {
        number += *it;
        ++it;
        if (*it == '.' || *it == 'e' || *it == 'E') {
            is_int = false;
        }
    }
    try {
        if (is_int) {
            try {
            return Node(std::stoi(number));
            }
            catch (...) {
            }
        }
        return Node(std::stod(number));
    } catch (const std::exception& e) {
        throw ParsingError("Invalid number");
    }
}

Node LoadArray(std::istream& input) {
    std::istreambuf_iterator<char> it(input);
    ++it;
    Array array;
    while (*it != ']') {
        array.push_back(LoadNode(input));
        // Пропуск запятых и пробельных символов
        while (std::isspace(*it) || *it == ',') {
            ++it;
        }
    }
    ++it; // Пропускаем ']'
    return Node(std::move(array));
}

Node LoadDict(std::istream& input) {
    std::istreambuf_iterator<char> it(input);
    ++it;
        Dict map;
        while (*it != '}') {
            // Пропуск пробельных символов
            while (std::isspace(*it)) {
                ++it;
            }
            // Парсинг ключа
            std::string key = LoadString(input);
            // Пропуск двоеточий и пробельных символов
            while (std::isspace(*it)) {
                ++it;
            }
            if (*it != ':') {
                throw ParsingError("Expected ':' after key");
            }
            ++it;

            // Парсим значение
            Node value = LoadNode(input);
            map[std::move(key)] = std::move(value);

            // Пропускаем пробельные символы и запятую, проверяем наличие фигурной скобки
            while (std::isspace(*it)) {
                ++it;
            }
            if (*it == ',') {
                ++it;
            } else if (*it != '}') {
                throw ParsingError("Expected ',' or '}'");
            }
        }
        ++it; // Пропускаем фигурную скобку
        return Node(map);
}

Node LoadNode(std::istream& input) {
    // Пропускаем пробелы
    std::istreambuf_iterator<char> it(input);
    while (std::isspace(*it)) {
        ++it;
    }
    // Проверяем следующий символ
    if (*it == '[') {
        return LoadArray(input);
    } else if (*it == '{') {
        return LoadDict(input);
    } else if (*it == '"') {
        return LoadStringNode(input);
    } else if (std::isdigit(*it) || *it == '-') {
        return LoadNumber(input);
    } else if (*it == 't' || *it == 'f' || *it == 'n') {
        // Проверяем булевы значения и null
        std::string str;
        while (std::isalpha(*it)) {
            str += *it;
            ++it;
        }
        if (str == "true"sv) {
            return Node(true);
        } else if (str == "false"sv) {
            return Node(false);
        } else if (str == "null"sv) {
            return Node(nullptr);
        } else {
            throw ParsingError("Invalid bool or null value");
        }
    } else {
        throw ParsingError("Invalid JSON format");
    }
}

}  // namespace

Node::Node(std::nullptr_t)      : value_(nullptr) {}
Node::Node(std::string value)   : value_(std::move(value)) {}
Node::Node(int value)           : value_(value) {}
Node::Node(double value)        : value_(value) {}
Node::Node(bool value)          : value_(value) {}
Node::Node(Array array)         : value_(std::move(array)) {}
Node::Node(Dict map)            : value_(std::move(map)) {}

bool Node::IsInt() const {
    return holds_alternative<int>(value_);
}

bool Node::IsDouble() const {
    return holds_alternative<int>(value_) || holds_alternative<double>(value_);
}

bool Node::IsPureDouble() const {
    return holds_alternative<double>(value_);
}

bool Node::IsBool() const {
    return holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return holds_alternative<std::string>(value_);
}

bool Node::IsNull() const {
    return holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return holds_alternative<Dict>(value_);
}

int Node::AsInt() const {
    if (!IsInt()) {
        throw std::logic_error("int type is expected");
    }
    return std::get<int>(value_);
}

bool Node::AsBool() const {
    if (!IsBool()) {
        throw std::logic_error("bool type is expected");
    }
    return std::get<bool>(value_);
}

double Node::AsDouble() const {
    if (!IsDouble()) {
        throw std::logic_error("double type is expected");
    }
    if (IsInt()) {
        return static_cast<double>(std::get<int>(value_));
    }
    return std::get<double>(value_);
}

const std::string& Node::AsString() const {
    if (!IsString()) {
        throw std::logic_error("std::string type is expected");
    }
    return std::get<std::string>(value_);
}

const Array& Node::AsArray() const {
    if (!IsArray()) {
        throw std::logic_error("Array type is expected");
    }
    return std::get<Array>(value_);
}

const Dict& Node::AsMap() const {
    if (!IsMap()) {
        throw std::logic_error("Dict type is expected");
    }
    return std::get<Dict>(value_);
}

const Value& Node::GetValue() const {
    return value_;
}

bool Node::operator==(const Node& rhs) const {
    return value_ == rhs.value_;
}

bool Node::operator!=(const Node& rhs) const {
    return !(value_ == rhs.value_);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document& rhs) const {
    return root_ == rhs.root_;
}

bool Document::operator!=(const Document& rhs) const {
    return !(root_ == rhs.root_);
}

Document Load(istream& input) {
    return Document {LoadNode(input)};
}

// ---------------- Print Node Value -----------------------
// вывод double и int
void PrintValue(const int value, std::ostream& out) {
    out << value;
}

void PrintValue(const double value, std::ostream& out) {
    out << value;
}

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, std::ostream& out) {
    out << "null"sv;
}

void PrintValue(const std::string& value, std::ostream& out) {
    out << "\""sv;
    for (const char& c : value) {
        if (c == '\n') {
            out << "\\n"sv;
            continue;
        }
        if (c == '\r') {
            out << "\\r"sv;
            continue;
        }
        if (c == '\t') {
            out << "\\t"sv;
            continue;
        }
        if (c == '\"') {
            out << "\\"sv;
        }
        if (c == '\\') {
            out << "\\"sv;
        }
        out << c;
    }
    out << "\""sv;
}

void PrintValue(const bool value, std::ostream& out) {
    out << std::boolalpha << value;
}

void PrintValue(const Array& array, std::ostream& out) {
    out << "["sv;
    bool first = true;
    for (const auto& item : array) {
        if (first) {
            first = false;
        } else {
            out << ", "s;
        }
        std::visit([&out](const auto& value) {
            PrintValue(value, out);
        }, item.GetValue());
    }
    out << "]"sv;
}

void PrintValue(const Dict& dict, std::ostream& out) {
    out << "{ "sv;
    bool first = true;
    for (auto& [key, item_value] : dict) {
        if (first) {
            first = false;
        } else {
            out << ", "s;
        }
        out << "\"" << key << "\": ";
        std::visit([&out](const auto& value) {
            PrintValue(value, out);
        }, item_value.GetValue());
    }
    out << " }"sv;
}

//----------------------------------------------------------

void PrintNode(const Node& node, std::ostream& out) {
    std::visit([&out](const auto& value) {
            PrintValue(value, out);
        }, node.GetValue());
}

void Print(const Document& doc, std::ostream& out) {
    PrintNode(doc.GetRoot(), out);
}

}  // namespace json