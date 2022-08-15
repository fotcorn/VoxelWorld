#ifndef DOM_H
#define DOM_H

#include "ui/DOMNode.h"

#include <memory>

std::shared_ptr<DOMNode> loadDOM(const std::string& filename);
std::shared_ptr<DOMNode> getElementById(const std::shared_ptr<DOMNode>& root, const std::string& id);

#endif /* DOM_H */
