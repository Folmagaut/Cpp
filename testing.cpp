#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "json_builder.h"

using namespace std::literals;

namespace json {

Builder::Builder() : root_(nullptr) {
    nodes_stack_.emplace_back(&root_);
}

Builder& Builder::StartDict() {
    auto* current_node = nodes_stack_.back();

    if (nodes_stack_.empty() || (!current_node->IsNull() && !current_node->IsArray())) {
        throw std::logic_error("Impossible to start Dict not in empty Node or not in Array"s);
    }
    if (current_node->IsNull()) {
        current_node->GetValue() = Dict();
    } /* else if (current_node->IsDict()) {
        if (!current_key_.has_value()) {
            throw std::logic_error("Key must be specified before StartDict in a Dict"s);
        }
        auto& dict = std::get<Dict>(current_node->GetValue());
        auto emplace_result = dict.emplace(std::move(current_key_.value()), Dict());
        nodes_stack_.emplace_back(&emplace_result.first->second);
        current_key_ = std::nullopt;
    } */ else if (current_node->IsArray()) {
        auto& array = std::get<Array>(current_node->GetValue());
        array.emplace_back(Dict());
        nodes_stack_.emplace_back(&array.back());
    } else {
        *current_node = Dict();
        nodes_stack_.pop_back();
        /* throw std::logic_error("Previous node must be Null, Array or Dict"s); */
    }

    return *this;
}

Builder& Builder::Key(std::string key) {
    // рабочий вариант
    auto* current_node = nodes_stack_.back();

    if (nodes_stack_.empty() || !current_node->IsDict()) {
        throw std::logic_error("Key can only be used within a Dict"s);
    }
    nodes_stack_.emplace_back(&const_cast<Dict&>(current_node->AsDict())[std::move(key)]);
    /* if (current_key_.has_value()) {
        throw std::logic_error("Key is already set"s);
    }

    current_key_ = std::move(key); */

    return *this;
}

Builder& Builder::Value(Node node) {
    auto* current_node = nodes_stack_.back();
    if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
        throw std::logic_error("Try add Value in ready object or not in Node and Array");
    }

    if (root_.IsNull()) {
        root_ = std::move(node);//std::move(NodeVisitor(node));
    } /* else if (current_node->IsDict()) {
        if (!current_key_.has_value()) {
            throw std::logic_error("Key must be specified before Value in a Dict"s);
        }
        auto& dict = std::get<Dict>(current_node->GetValue());
        dict.emplace(std::move(current_key_.value()), std::move(NodeVisitor(node)));
        current_key_ = std::nullopt;
    } */ else if (current_node->IsArray()) {
        auto& array = std::get<Array>(current_node->GetValue());
        array.emplace_back(node);
        current_node = &array.back();
    } else {
        *current_node = std::move(node);
        nodes_stack_.pop_back();
        /* throw std::logic_error("Value can only be used within a Dict or Array"s); */
    }
    
    return *this;
}

Builder& Builder::StartArray() {
    auto* current_node = nodes_stack_.back();
    
    if (nodes_stack_.empty() || (!nodes_stack_.back()->IsNull() && !nodes_stack_.back()->IsArray())) {
        throw std::logic_error("Try to start Array in empty object or not in Array and Node"s);
    }
    /* if (current_node->IsDict()) {
        if (!current_key_) {
            throw std::logic_error("Impossible to StartArray() for Dict without Key"s);
        }
        auto& dict = std::get<Dict>(current_node->GetValue());
        auto [it, success] = dict.emplace(std::move(current_key_.value()), Array());
        current_key_ = std::nullopt;
        nodes_stack_.emplace_back(&it->second);
    } else */ if (current_node->IsArray()) {
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
        throw std::logic_error("EndDict can only be used within a dictionary");
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder& Builder::EndArray() {
    if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
        throw std::logic_error("EndArray can only be used within an array"s);
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

/* Node Builder::NodeVisitor(const Node& node) {
    if (node.IsInt()) {
        return Node(node.AsInt());
    }
    if (node.IsDouble()) {
        return Node(node.AsDouble());
    }
    if (node.IsString()) {
        return Node(node.AsString());
    }
    if (node.IsNull()) {
        return Node(nullptr);
    }
    if (node.IsBool()) {
        return Node(node.AsBool());
    }
    if (node.IsDict()) {
        return Node(node.AsDict());
    }
    if (node.IsArray()) {
        return Node(node.AsArray());
    }
    return {};
} */

} // json