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

    rects.push_back(Rect(x, y, width, height));

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
