#pragma once

#include <istream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std;

class Node {
public:
    Node(string name, unordered_map<string, string> attrs)
        : name_(move(name))
        , attrs_(move(attrs)) {
    }

    const vector<Node>& Children() const {
        return children_;
    }

    void AddChild(Node node) {
        children_.push_back(move(node));
    }

    string_view Name() const {
        return name_;
    }

    template <typename T>
    T AttributeValue(const string& name) const;

private:
    string name_;
    vector<Node> children_;
    unordered_map<string, string> attrs_;
};

class Document {
public:
    explicit Document(Node root)
        : root_(move(root)) {
    }

    const Node& GetRoot() const {
        return root_;
    }

private:
    Node root_;
};

pair<string_view, string_view> Split(string_view line, char by) {
    size_t pos = line.find(by);
    string_view left = line.substr(0, pos);

    if (pos < line.size() && pos + 1 < line.size()) {
        return {left, line.substr(pos + 1)};
    } else {
        return {left, string_view()};
    }
}

string_view Lstrip(string_view line) {
    while (!line.empty() && isspace(line[0])) {
        line.remove_prefix(1);
    }
    return line;
}

string_view Unquote(string_view value) {
    if (!value.empty() && value.front() == '"') {
        value.remove_prefix(1);
    }
    if (!value.empty() && value.back() == '"') {
        value.remove_suffix(1);
    }
    return value;
}

Node LoadNode(istream& input) {
    string root_name;
    getline(input, root_name);

    Node root(root_name.substr(1, root_name.size() - 2), {});
    for (string line; getline(input, line) && line[1] != '/';) {
        auto [node_name, attrs] = Split(Lstrip(line), ' ');
        attrs = Split(attrs, '>').first;
        unordered_map<string, string> node_attrs;
        while (!attrs.empty()) {
            const auto [head, tail] = Split(attrs, ' ');
            const auto [name, value] = Split(head, '=');
            if (!name.empty() && !value.empty()) {
                node_attrs[string(Unquote(name))] = string(Unquote(value));
            }
            attrs = tail;
        }

        root.AddChild(Node(string(node_name.substr(1)), move(node_attrs)));
    }
    return root;
}

Document Load(istream& input) {
    return Document{LoadNode(input)};
}

template <typename T>
inline T Node::AttributeValue(const string& name) const {
    istringstream attr_input(attrs_.at(name));
    T result;
    attr_input >> result;
    return result;
}