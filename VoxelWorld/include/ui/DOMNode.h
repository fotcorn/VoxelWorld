#ifndef DOMNODE_H
#define DOMNODE_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <yoga/Yoga.h>

class DOMNode {
public:
    DOMNode(const std::string& name) : _name(name) {
    }
    std::string name() const {
        return this->_name;
    }
    void name(const std::string& name) {
        this->_name = name;
    }

    std::map<std::string, std::string> attributes;
    std::map<std::string, std::string> styles;
    std::vector<std::shared_ptr<DOMNode>> children;

    YGNodeRef layoutNode = nullptr;

private:
    std::string _name;
};

#endif /* DOMNODE_H */
