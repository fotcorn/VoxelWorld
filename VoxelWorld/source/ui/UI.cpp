#include "ui/UI.h"
#include "ui/DOM.h"
#include "ui/Layout.h"

#include <glm/gtc/matrix_transform.hpp>

#include <fmt/format.h>

UI::UI(const int width, const int height) : root(loadDOM("main.html")) {
    this->windowChanged(width, height);
}

void UI::windowChanged(const int width, const int height) {
    this->rects = calculateLayout(root, width, height);
    this->projectionMatrix =
        glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, -100.0f, 100.0f);
}

void UI::render() {
    for (auto& rect : rects) {
        rect.render(projectionMatrix, false);
    }
}
