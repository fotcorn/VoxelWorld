#include "ui/DOM.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <gumbo.h>

#include <fmt/format.h>
using namespace fmt;

static std::optional<DOMNode> buildDOM(std::shared_ptr<GumboNode> node) {
    if (node->type != GUMBO_NODE_ELEMENT) {
        return std::nullopt;
    }
    DOMNode domNode(gumbo_normalized_tagname(node->v.element.tag));

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; ++i) {
        auto childNode = buildDOM(std::shared_ptr<GumboNode>(static_cast<GumboNode*>(children->data[i])));
        if (childNode.has_value()) {
            domNode.children.push_back(childNode.value());
        }
    }

    // TODO: parse attributes
    // TODO: parse style attribute

    return std::make_optional(domNode);
}

DOMNode loadDOM(const std::string& filename) {
    std::ifstream in(format("{}/{}", UI_PATH, filename), std::ios::in | std::ios::binary);
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
