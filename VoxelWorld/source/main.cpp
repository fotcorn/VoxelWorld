#include "RenderLoop.h"

#include <iostream>
int main() {
    try {
        RenderLoop loop;
        loop.mainLoop();
    } catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
