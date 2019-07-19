#include <iostream>
using namespace std;

#include "RenderLoop.h"

int main() {
    Program program;
    program.init();
    program.mainLoop();
    return 0;
}
