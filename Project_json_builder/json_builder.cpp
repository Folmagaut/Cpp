//#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "json_builder.h"

using namespace std::literals;

namespace json {

// BaseContext
BaseContext::BaseContext(Builder& builder) : builder_(builder) {
}

DictItemContext BaseContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext BaseContext::StartArray() {
    return builder_.StartArray();
}

Builder& BaseContext::EndArray() {
    return builder_.EndArray();
}

Builder& BaseContext::EndDict() {
    return builder_.EndDict();
}

KeyItemContext BaseContext::Key(std::string key) {
    return builder_.Key(std::move(key));
}

Builder& BaseContext::Value(Node node) {
    return builder_.Value(std::move(node));
}

// DictItemContext
DictItemContext::DictItemContext(Builder& builder) : BaseContext(builder) {
}

// KeyItemContext
KeyItemContext::KeyItemContext(Builder& builder) : BaseContext(builder) {
}

DictItemContext KeyItemContext::Value(Node node) {
    return BaseContext::Value(std::move(node));
}

// ArrayItemContext
ArrayItemContext::ArrayItemContext(Builder& builder) : BaseContext(builder) {
}

ArrayItemContext ArrayItemContext::Value(Node node) {
    return BaseContext::Value(std::move(node));
}

Builder::Builder() : root_(nullptr) {
    nodes_stack_.emplace_back(&root_);
}

DictItemContext Builder::StartDict() {
    auto* current_node = nodes_stack_.back();

    if (nodes_stack_.empty() || (!current_node->IsNull() && !current_node->IsArray())) {
        throw std::logic_error("Impossible to StartDict not in empty Node (of Dict[key]) or not in Array"s);
    }
    if (current_node->IsNull()) {
        current_node->GetValue() = Dict();
    } else if (current_node->IsArray()) {
        auto& array = std::get<Array>(current_node->GetValue());
        array.emplace_back(Dict());
        nodes_stack_.emplace_back(&array.back());
    } else {
        *current_node = Dict();
        nodes_stack_.pop_back();
    }

    return *this;
}

KeyItemContext Builder::Key(std::string key) {
    auto* current_node = nodes_stack_.back();

    if (nodes_stack_.empty() || !current_node->IsDict()) {
        throw std::logic_error("Key can only be used within a Dict"s);
    }
    nodes_stack_.emplace_back(&const_cast<Dict&>(current_node->AsDict())[std::move(key)]);

    return *this;
}

Builder& Builder::Value(Node node) {
    auto* current_node = nodes_stack_.back();

    if (nodes_stack_.empty() || (!current_node->IsNull() && !current_node->IsArray())) {
        throw std::logic_error("Impossible to add Value in empty object or not in empty Node (of Dict[key]) or Array");
    }

    if (root_.IsNull()) {
        root_ = std::move(node);
    } else if (current_node->IsArray()) {
        auto& array = std::get<Array>(current_node->GetValue());
        array.emplace_back(node);
        current_node = &array.back();
    } else {
        *current_node = std::move(node);
        nodes_stack_.pop_back();
    }
    
    return *this;
}

ArrayItemContext Builder::StartArray() {
    auto* current_node = nodes_stack_.back();
    
    if (nodes_stack_.empty() || (!current_node->IsNull() && !current_node->IsArray())) {
        throw std::logic_error("Impossible to StartArray in empty object or not in Array, or empty Node (of Dict[key])"s);
    }
    
    if (current_node->IsArray()) {
        auto& array = std::get<Array>(current_node->GetValue());
        array.emplace_back(Array());
        nodes_stack_.emplace_back(&array.back());
    } else if (current_node->IsNull()) {
        current_node->GetValue() = Array();
    } else {
        *current_node = Array();
        nodes_stack_.pop_back();
    }

    return *this;
}

Builder& Builder::EndDict() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
        throw std::logic_error("EndDict can only be used within a Dict"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw std::logic_error("EndArray can only be used within an Array"s);
    }
    nodes_stack_.pop_back();
    return *this;
}

Node Builder::Build() {
    if (root_.IsNull() || nodes_stack_.size() > 1) {
        throw std::logic_error("Wrong Build()"s);
    }
    return root_;
}

} // json