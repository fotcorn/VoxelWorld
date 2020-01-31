#include "RenderLoop.h"

#ifdef EMSCRIPTEN

#include <emscripten.h>
#include <iostream>

std::shared_ptr<RenderLoop> loop;

void update() {
    loop->mainLoop();
}

int main() {
    try {
        loop = std::make_shared<RenderLoop>();
    } catch (std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }
    emscripten_set_main_loop(update, 0, 1);
    return 0;
}
#else

int main() {
    RenderLoop loop;
    loop.mainLoop();
    return 0;
}

#endif
