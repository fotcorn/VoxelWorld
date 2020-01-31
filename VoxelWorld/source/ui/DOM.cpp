#include "ui/DOM.h"

#include <fstream>
#include <memory>
#include <optional>
#include <sstream>

#include <gumbo.h>

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>

namespace {
std::shared_ptr<DOMNode> buildDOM(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return std::shared_ptr<DOMNode>();
    }
    auto domNode = std::make_shared<DOMNode>(gumbo_normalized_tagname(node->v.element.tag));

    GumboVector children = node->v.element.children;
    for (unsigned int i = 0; i < children.length; i++) {
        auto childNode = buildDOM(static_cast<GumboNode*>(children.data[i]));
        if (childNode) {
            domNode->children.push_back(childNode);
        }
    }

    GumboVector attributes = node->v.element.attributes;
    for (unsigned int i = 0; i < attributes.length; i++) {
        auto attribute = static_cast<GumboAttribute*>(attributes.data[i]);
        auto name = std::string(attribute->name);
        auto value = std::string(attribute->value);
        domNode->attributes[name] = value;

        if (name == "style") {
            std::vector<std::string> properties;
            boost::split(properties, value, boost::is_any_of(";"));

            for (const std::string& prop : properties) {
                const auto property = boost::trim_copy(boost::to_lower_copy(prop));
                if (property == "") {
                    continue;
                }
                std::vector<std::string> nameValue;
                boost::split(nameValue, property, boost::is_any_of(":"));
                if (nameValue.size() != 2) {
                    throw std::runtime_error(fmt::format("invalid css property: {}", property));
                }

                auto propertyName = boost::trim_copy(nameValue[0]);
                auto propertyValue = boost::trim_copy(nameValue[1]);
                domNode->styles[propertyName] = propertyValue;
            }
        }
    }
    return domNode;
}
} // namespace

std::shared_ptr<DOMNode> loadDOM(const std::string& filename) {
    std::ifstream in(fmt::format("{}/{}", UI_PATH, filename), std::ios::in | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Unable to open UI html file");
    }

    std::stringstream sstr;
    sstr << in.rdbuf();
    auto content = sstr.str();

    GumboOutput* output = gumbo_parse(content.c_str());
    auto html = buildDOM(output->root);
    gumbo_destroy_output(&kGumboDefaultOptions, output);
    return html->children[1]->children[0]; // html => body => div
}

std::shared_ptr<DOMNode> getElementById(const std::shared_ptr<DOMNode> root, const std::string& id) {
    for (const auto child : root->children) {
        const auto attribute = child->attributes.find("id");
        if (attribute != child->attributes.end()) {
            return child;
        }
        const auto node = getElementById(child, id);
        if (node) {
            return node;
        }
    }
    return std::shared_ptr<DOMNode>();
}
