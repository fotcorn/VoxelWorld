#ifndef DOMNODE_H
#define DOMNODE_H

#include <map>
#include <string>
#include <vector>

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
    std::vector<DOMNode> children;

private:
    std::string _name;
};

#endif /* DOMNODE_H */
