#include "ui/Layout.h"
#include "2d/Rect.h"

#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <regex>
#include <yoga/Yoga.h>

static int convertPixel(const std::string& pixel) {
    auto value = boost::replace_all_copy(pixel, "px", "");
    try {
        return std::stoi(value);
    } catch (std::invalid_argument) {
        throw std::runtime_error(fmt::format("Invalid pixel value: {}", pixel));
    }
}

static const std::regex numberRegex("^(0|[1-9]\\d*)$");
static const std::regex numberPXRegex("^(0|[1-9]\\d*)px$");
static const std::regex numberPercentRegex("^(0|[1-9]\\d*)%$");

static std::optional<float> parsePixel(const std::string& value) {
    std::smatch match;
    std::regex_match(value, match, numberPXRegex);
    if (match.size() != 2) {
        std::regex_match(value, match, numberRegex);
    }
    if (match.size() == 2) {
        return std::make_optional(std::stof(match[1].str()));
    } else {
        return std::nullopt;
    }
}

static std::optional<float> parsePercent(const std::string& value) {
    std::smatch match;
    std::regex_match(value, match, numberPercentRegex);
    if (match.size() == 2) {
        return std::make_optional(std::stof(match[1].str()));
    } else {
        return std::nullopt;
    }
}

static void buildYogaTree(DOMNode& node, const YGConfigRef config) {
    const YGNodeRef yogaNode = YGNodeNewWithConfig(config);

    for (const auto& [style, value] : node.styles) {
        if (style == "height") {
            auto pixel = parsePixel(value);
            if (pixel.has_value()) {
                YGNodeStyleSetHeight(yogaNode, pixel.value());
                continue;
            }

            auto percent = parsePercent(value);
            if (percent.has_value()) {
                YGNodeStyleSetHeightPercent(yogaNode, percent.value());
                continue;
            }

            throw std::runtime_error(fmt::format("Invalid height css value: {}", value));
        } else if (style == "width") {
            auto pixel = parsePixel(value);
            if (pixel.has_value()) {
                YGNodeStyleSetWidth(yogaNode, pixel.value());
                continue;
            }

            auto percent = parsePercent(value);
            if (percent.has_value()) {
                YGNodeStyleSetWidthPercent(yogaNode, percent.value());
                continue;
            }

            throw std::runtime_error(fmt::format("Invalid width css value: {}", value));
        } else if (style == "justify-content") {
            if (value == "flex-start") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifyFlexStart);
            } else if (value == "center") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifyCenter);
            } else if (value == "flex-end") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifyFlexEnd);
            } else if (value == "space-between") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifySpaceBetween);
            } else if (value == "space-around") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifySpaceAround);
            } else if (value == "space-evenly") {
                YGNodeStyleSetJustifyContent(yogaNode, YGJustifySpaceEvenly);
            }
        } else if (style == "flex-direction") {
            if (value == "column") {
                YGNodeStyleSetFlexDirection(yogaNode, YGFlexDirectionColumn);
            } else if (value == "column-reverse") {
                YGNodeStyleSetFlexDirection(yogaNode, YGFlexDirectionColumnReverse);
            } else if (value == "row") {
                YGNodeStyleSetFlexDirection(yogaNode, YGFlexDirectionRow);
            } else if (value == "row-reverse") {
                YGNodeStyleSetFlexDirection(yogaNode, YGFlexDirectionRowReverse);
            }
        } else if (boost::starts_with(style, "margin-")) {
            YGEdge edge;
            if (style == "margin-top") {
                edge = YGEdgeTop;
            } else if (style == "margin-bottom") {
                edge = YGEdgeBottom;
            } else if (style == "margin-left") {
                edge = YGEdgeLeft;
            } else if (style == "margin-right") {
                edge = YGEdgeRight;
            } else {
                throw std::runtime_error(fmt::format("invalud margin css property: {}", style));
            }
            int pixel = convertPixel(value);
            YGNodeStyleSetMargin(yogaNode, edge, static_cast<float>(pixel));
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

static void buildRects(const DOMNode& node, std::vector<Rect>& rects,
                       std::optional<std::tuple<float, float>> parentPosition) {
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

    if (parentPosition.has_value()) {
        x = x + std::get<0>(parentPosition.value());
        y = y + std::get<1>(parentPosition.value());
    }

    const auto renderProperty = node.styles.find("visibility");
    if (renderProperty == node.styles.end() || renderProperty->second != "hidden") {
        rects.push_back(Rect(x, y, 0.0f, width, height, color));
    }

    const auto position = std::make_optional(std::make_tuple(x, y));
    for (auto& child : node.children) {
        buildRects(child, rects, position);
    }
}

std::vector<Rect> calculateLayout(DOMNode& root, const int width, const int height) {
    const YGConfigRef config = YGConfigNew();

    buildYogaTree(root, config);

    YGNodeCalculateLayout(root.layoutNode, static_cast<float>(width), static_cast<float>(height), YGDirectionLTR);

    std::vector<Rect> rects;
    buildRects(root, rects, std::nullopt);

    YGNodeFreeRecursive(root.layoutNode);
    YGConfigFree(config);

    return rects;
}
