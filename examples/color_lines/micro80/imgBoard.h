#include <stdint.h>

static const unsigned imgBoardWidth = 3;
static const unsigned imgBoardHeight = 3;
static const unsigned imgBoardSize = imgBoardWidth + (imgBoardHeight << 8);

extern uint8_t imgBoard[6][18];
