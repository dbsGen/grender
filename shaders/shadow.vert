R"(
#include "simple.vert"

attribute float A_SIZE_WIDTH;
attribute float A_SIZE_HEIGHT;
attribute float A_BORDER_WIDTH;
attribute float A_CORNER;

varying float SIZE_WIDTH;
varying float SIZE_HEIGHT;
varying float BORDER_WIDTH;
varying float CORNER;

void main(void) {
    simple();
    SIZE_WIDTH = A_SIZE_WIDTH;
    SIZE_HEIGHT = A_SIZE_HEIGHT;
    BORDER_WIDTH = A_BORDER_WIDTH;
    CORNER = A_CORNER;
}

)"
