#ifndef LAYOUT_H
#define LAYOUT_H

#include <vector>

#include "2d/Rect.h"
#include "ui/DOMNode.h"

std::vector<Rect> calculateLayout(DOMNode& root, const int width, const int height);

#endif /* LAYOUT_H */
