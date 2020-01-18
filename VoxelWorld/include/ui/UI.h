#ifndef UI_H
#define UI_H

#include <memory>

#include "ui/DOMNode.h"

class UI {
public:
    UI(const int width, const int height);
    void windowChanged(const int width, const int height);
    void render();

private:
    DOMNode root;
};

#endif /* UI_H */
