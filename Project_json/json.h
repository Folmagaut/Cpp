#pragma once
#include <array>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

class Node;
class Document;

// Сохраните объявления Dict и Array без изменения
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Value = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

// Эта ошибка должна выбрасываться при ошибках парсинга JSON
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class Node {
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
    bool IsMap() const;

    int AsInt() const;
    bool AsBool() const;
    double AsDouble() const; // Возвращает значение типа double, если внутри хранится double либо int. В последнем случае возвращается приведённое в double значение.
    const std::string& AsString() const;
    const Array& AsArray() const;
    const Dict& AsMap() const;
    // если другое, то выбрасывается std::logic_error

    const Value& GetValue() const;

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
void PrintValue(const int value, std::ostream& out);

void PrintValue(const double value, std::ostream& out);

// Перегрузка функции PrintValue для вывода значений null
void PrintValue(std::nullptr_t, std::ostream& out);

void PrintValue(const std::string&, std::ostream& out);

void PrintValue(const bool, std::ostream& out);

void PrintValue(const Array&, std::ostream& out);

void PrintValue(const Dict&, std::ostream& out);

//----------------------------------------------------------

void PrintNode(const Node& node, std::ostream& out);

void Print(const Document& doc, std::ostream& out);

}  // namespace json

/* json::Node LoadNode(std::istream& input);

std::string LoadString(std::istream& input);

json::Node LoadStringNode(std::istream& input);

json::Node LoadNumber(std::istream& input);

json::Node LoadArray(std::istream& input);

json::Node LoadDict(std::istream& input); */