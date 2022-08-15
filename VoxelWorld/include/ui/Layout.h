#ifndef LAYOUT_H
#define LAYOUT_H

#include <memory>
#include <vector>

#include "2d/Sprite.h"
#include "ui/DOMNode.h"

std::vector<std::unique_ptr<Sprite>> calculateLayout(std::shared_ptr<DOMNode> root, int width, int height);

#endif /* LAYOUT_H */
