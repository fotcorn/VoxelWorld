#ifndef DOM_H
#define DOM_H

#include "ui/DOMNode.h"

#include <memory>

std::shared_ptr<DOMNode> loadDOM(const std::string& filename);

#endif /* DOM_H */
