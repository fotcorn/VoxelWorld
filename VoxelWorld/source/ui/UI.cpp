#include "ui/UI.h"
#include "ui/DOM.h"
#include "ui/Layout.h"

#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

UI::UI(const int width, const int height) : root(loadDOM("main.html")) {
    this->windowChanged(width, height);
}

void UI::windowChanged(const int width, const int height) {
    this->width = width;
    this->height = height;
    this->projectionMatrix = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -1.0f, 1.0f);
    this->rebuild();
}

void UI::rebuild() {
    this->rects = calculateLayout(root, width, height);
}

void UI::render() {
    for (auto it = rects.rbegin(); it != rects.rend(); it++) {
        it->render(projectionMatrix, false);
    }
}

namespace {
std::string blockTypeToString(BlockType type) {
    switch (type) {
    case BlockType::NONE:
        return "";
    case BlockType::STONE:
        return "stone";
    case BlockType::GRASS:
        return "grass";
    case BlockType::LAVA:
        return "lava";
    }
    return "";
}
} // namespace

void UI::handleKeyUp(Key key) {
    switch (key) {
    case Key::ONE:
        blockType = BlockType::GRASS;
        break;
    case Key::TWO:
        blockType = BlockType::STONE;
        break;
    case Key::THREE:
        blockType = BlockType::LAVA;
        break;
    default:
        blockType = BlockType::NONE;
        break;
    }

    if (blockType == BlockType::NONE) {
        return;
    }

    std::string blockName = blockTypeToString(blockType);

    auto node = getElementById(root, "blocks");
    if (!node) {
        return;
    }
    for (auto child : node->children) {
        if (child->attributes[std::string("id")] == blockName) {
            child->styles["visibility"] = "visible";
        } else {
            child->styles["visibility"] = "hidden";
        }
    }

    rebuild();
}

BlockType UI::selectedBlockType() {
    return blockType;
}
