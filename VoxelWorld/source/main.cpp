#include <iostream>
using namespace std;

#include "render_loop.h"

#include "perlin_noise.h"

int main() {
    Program program;
    program.init();
    program.mainLoop();
    return 0;
}
