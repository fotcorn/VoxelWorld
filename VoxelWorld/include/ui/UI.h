#ifndef UI_H
#define UI_H

#include <memory>

#include "2d/Rect.h"
#include "ui/DOMNode.h"

class UI {
public:
    UI(const int width, const int height);
    void windowChanged(const int width, const int height);
    void render();

private:
    std::shared_ptr<DOMNode> root;
    std::vector<Rect> rects;
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
};

#endif /* UI_H */
