#include <iostream>
using namespace std;

#include "render_loop.h"

int main() {
    Program program;
    program.init();
    program.mainLoop();
    return 0;
}
