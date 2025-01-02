#pragma once

#include <optional>
#include <string>
#include <vector>

#include "json.h"

namespace json {

class Builder {
public:
    Builder();
    Builder& StartDict();
    Builder& Key(std::string key);
    Builder& Value(Node node);
    Builder& StartArray();
    Builder& EndDict();
    Builder& EndArray();
    Node Build();
    //Node NodeVisitor(const Node& node);

private:
    Node root_;
    std::vector<Node*> nodes_stack_;
    //std::optional<std::string> current_key_ = std::nullopt;
};

} // json