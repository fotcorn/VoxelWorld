#include "ui/UI.h"
#include "ui/DOM.h"

UI::UI(const int width, const int height) : root(loadDOM("main.html")) {
    this->windowChanged(width, height);
}

void UI::windowChanged(const int width, const int height) {
}

void UI::render() {
}
