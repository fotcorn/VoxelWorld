#include "ui/DOM.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <gumbo.h>

#include <fmt/format.h>

#include <boost/algorithm/string.hpp>

static std::optional<DOMNode> buildDOM(std::shared_ptr<GumboNode> node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return std::nullopt;
    }
    DOMNode domNode(gumbo_normalized_tagname(node->v.element.tag));

    GumboVector children = node->v.element.children;
    for (unsigned int i = 0; i < children.length; i++) {
        auto childNode = buildDOM(std::shared_ptr<GumboNode>(static_cast<GumboNode*>(children.data[i])));
        if (childNode.has_value()) {
            domNode.children.push_back(childNode.value());
        }
    }

    GumboVector attributes = node->v.element.attributes;
    for (unsigned int i = 0; i < attributes.length; i++) {
        auto attribute = static_cast<GumboAttribute*>(attributes.data[i]);
        auto name = std::string(attribute->name);
        auto value = std::string(attribute->value);
        domNode.attributes[name] = value;

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
                domNode.styles[propertyName] = propertyValue;
            }
        }
    }
    return std::make_optional(domNode);
}

DOMNode loadDOM(const std::string& filename) {
    std::ifstream in(fmt::format("{}/{}", UI_PATH, filename), std::ios::in | std::ios::binary);
    if (!in) {
        throw std::runtime_error("Unable to open UI html file");
    }

    std::stringstream sstr;
    sstr << in.rdbuf();
    auto content = sstr.str();

    auto output = std::shared_ptr<GumboOutput>(gumbo_parse(content.c_str()));
    auto root = std::shared_ptr<GumboNode>(output->root);

    auto html = buildDOM(root).value();
    return html.children[1].children[0]; // html => body => div
}
