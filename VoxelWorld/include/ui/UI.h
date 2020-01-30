#ifndef UI_H
#define UI_H

#include <memory>

#include "2d/Sprite.h"
#include "ui/DOMNode.h"

enum class Key {
    ONE,
    TWO,
    THREE,
};

enum class BlockType {
    NONE,
    STONE,
    GRASS,
    WATER,
};

class UI {
public:
    UI(const int width, const int height);
    void windowChanged(const int width, const int height);
    void render();
    void rebuild();

    void handleKeyUp(Key key);
    BlockType selectedBlockType();

private:
    std::shared_ptr<DOMNode> root;
    std::vector<std::unique_ptr<Sprite>> sprites;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    BlockType blockType = BlockType::NONE;
    int width;
    int height;
};

#endif /* UI_H */
