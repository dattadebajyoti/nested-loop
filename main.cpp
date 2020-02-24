#include "NestedLoop.hpp"
#include <stdio.h>
#define DEBUG_OUTPUT

int main() {
    MatMult::run(20);
    #if defined(DEBUG_OUTPUT)
        printf("Done\n");
    #endif
}