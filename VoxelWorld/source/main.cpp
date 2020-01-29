#include "RenderLoop.h"

#include <iostream>

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

void update() {
    static RenderLoop loop;
    loop.mainLoop();
}

int main() {
#ifdef EMSCRIPTEN
    emscripten_set_main_loop(update, 0, 1);
#else
    RenderLoop loop;
    loop.mainLoop();
#endif
    return 0;
}
