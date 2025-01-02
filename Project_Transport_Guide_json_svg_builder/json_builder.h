#pragma once

#include <string>
#include <vector>

#include "json.h"

namespace json {

class Builder;
class DictItemContext;
class KeyItemContext;
class ArrayItemContext;

class BaseContext {
public:
    BaseContext(Builder& builder);

    DictItemContext StartDict();
    ArrayItemContext StartArray();
    Builder& EndDict();
    Builder& EndArray();
    KeyItemContext Key(std::string key);
    Builder& Value(Node node);

private:
    Builder& builder_;
};

class DictItemContext : public BaseContext {
public:
    DictItemContext(Builder& builder);
    // Builder& EndDict(); - разрешён
    // KeyItemContext Key(std::string key); - разрешён
    DictItemContext StartDict() = delete;
    ArrayItemContext StartArray() = delete;
    Builder& EndArray() = delete;
    Builder& Value(Node node) = delete;
};

class KeyItemContext : public BaseContext {
public:
    KeyItemContext(Builder& builder);

    // DictItemContext StartDict(); - разрешён
    // ArrayItemContext StartArray(); - разрешён
    Builder& EndDict() = delete;
    Builder& EndArray() = delete;
    Builder& Key(std::string key) = delete;
    DictItemContext Value(Node node); // возвращаемся в контекст словаря
};

class ArrayItemContext : public BaseContext {
public:
    ArrayItemContext(Builder& builder);

    // DictItemContext StartDict(); - разрешён
    // ArrayItemContext StartArray(); - разрешён
    // Builder& EndArray(); - разрешён
    Builder& EndDict() = delete;
    KeyItemContext Key(std::string key) = delete;
    ArrayItemContext Value(Node node); // остаёмся в контексте массива
};

class Builder {
public:
    Builder();
    DictItemContext StartDict();
    ArrayItemContext StartArray();
    KeyItemContext Key(std::string key);
    //Builder& StartDict();
    //Builder& Key(std::string key);
    Builder& Value(Node node);
    //Builder& StartArray();
    Builder& EndDict();
    Builder& EndArray();
    Node Build();

private:
    Node root_;
    std::vector<Node*> nodes_stack_;
};

} // json