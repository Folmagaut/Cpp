// Библиотеку я оставил свою из прошлого спринта;
#pragma once
#include <array>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

// Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
struct PrintContext {
    std::ostream& out;
    int indent_step = 4;
    int indent = 0;

    void PrintIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    // Возвращает новый контекст вывода с увеличенным смещением
    PrintContext Indented() const {
        return {out, indent_step, indent_step + indent};
    }
};

class Node;
class Document;

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node final {
public:
    Node() = default;
    Node(std::nullptr_t);
    Node(std::string value);
    Node(int value);
    Node(double value);
    Node(bool value);
    Node(Array array);
    Node(Dict map);
    
    bool IsInt() const;
    bool IsDouble() const; // true, если int или double
    bool IsPureDouble() const; // true, если double
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsDict() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const; // Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsDict() const;
    // если другое, то выбрасывается std::logic_error

    const Value& GetValue() const;
    Value& GetValue();

    // operator== Значения равны, если внутри них значения имеют одинаковый тип и содержимое.
    // и != если наоборот
    bool operator==(const Node& rhs) const;
    bool operator!=(const Node& rhs) const;

private:
    Value value_;
};

class Document {
public:
    explicit Document(Node root);

    const Node& GetRoot() const;

    bool operator==(const Document& rhs) const;
    bool operator!=(const Document& rhs) const;

private:
    Node root_;
};

Document Load(std::istream& input);

// ---------------- Print Node Value -----------------------
// Шаблон, подходящий для вывода double и int
void PrintValue(const int value, const PrintContext& context);

void PrintValue(const double value, const PrintContext& context);

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, const PrintContext& context);

void PrintValue(const std::string&, const PrintContext& context);

void PrintValue(const bool, const PrintContext& context);

void PrintValue(const Array&, const PrintContext& context);

void PrintValue(const Dict&, const PrintContext& context);

//----------------------------------------------------------

void PrintNode(const Node& node, const PrintContext& context);

void Print(const Document& doc, std::ostream& out);

}  // namespace json