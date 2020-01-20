#include "ui/Layout.h"
#include "2d/Rect.h"

#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <yoga/Yoga.h>

static int convertPixel(const std::string& pixel) {
    auto value = boost::replace_all_copy(pixel, "px", "");
    try {
        return std::stoi(value);
    } catch (std::invalid_argument) {
        throw std::runtime_error(fmt::format("Invalid pixel value: {}", pixel));
    }
}

static void buildYogaTree(DOMNode& node, const YGConfigRef config) {
    const YGNodeRef yogaNode = YGNodeNewWithConfig(config);

    for (const auto& [style, value] : node.styles) {
        if (style == "height") {
            int pixel = convertPixel(value);
            YGNodeStyleSetHeight(yogaNode, static_cast<float>(pixel));
        } else if (style == "width") {
            int pixel = convertPixel(value);
            YGNodeStyleSetWidth(yogaNode, static_cast<float>(pixel));
        }
    }
    node.layoutNode = yogaNode;

    int i = 0;
    for (auto& child : node.children) {
        buildYogaTree(child, config);
        YGNodeInsertChild(yogaNode, child.layoutNode, i);
        i++;
    }
}

static void buildRects(const DOMNode& node, std::vector<Rect>& rects) {
    float x = YGNodeLayoutGetLeft(node.layoutNode);
    float y = YGNodeLayoutGetTop(node.layoutNode);
    float width = YGNodeLayoutGetWidth(node.layoutNode);
    float height = YGNodeLayoutGetHeight(node.layoutNode);

    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    auto colorProperty = node.styles.find("background-color");
    if (colorProperty != node.styles.end()) {
        auto hexColor = colorProperty->second;
        if (hexColor.length() != 7 || hexColor[0] != '#') {
            throw std::runtime_error(fmt::format("Invalid background color: {}", hexColor));
        }
        int r = std::stoi(hexColor.substr(1, 2), nullptr, 16);
        int g = std::stoi(hexColor.substr(3, 2), nullptr, 16);
        int b = std::stoi(hexColor.substr(5, 2), nullptr, 16);
        color =
            glm::vec3(static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f);
    }

    rects.push_back(Rect(x, y, width, height, color));

    for (auto& child : node.children) {
        buildRects(child, rects);
    }
}

std::vector<Rect> calculateLayout(DOMNode& root, const int width, const int height) {
    const YGConfigRef config = YGConfigNew();

    buildYogaTree(root, config);

    YGNodeCalculateLayout(root.layoutNode, static_cast<float>(width), static_cast<float>(height), YGDirectionLTR);

    std::vector<Rect> rects;
    buildRects(root, rects);

    YGNodeFreeRecursive(root.layoutNode);
    YGConfigFree(config);

    return rects;
}
