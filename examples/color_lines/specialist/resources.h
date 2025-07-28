#include <stdint.h>

static const unsigned imgPlayerWidth = 40;
static const unsigned imgPlayerHeight = 50;
static const unsigned imgPlayerSize = imgPlayerWidth + (imgPlayerHeight << 8);
extern uint8_t imgPlayer[500];

static const unsigned imgScreenWidth = 386;
static const unsigned imgScreenHeight = 256;
static const unsigned imgScreenSize = imgScreenWidth + (imgScreenHeight << 8);
extern uint8_t imgScreen[1215];
extern uint8_t imgScreenColors[214];

static const unsigned imgBallsWidth = 16;
static const unsigned imgBallsHeight = 14;
static const unsigned imgBallsSize = imgBallsWidth + (imgBallsHeight << 8);
extern uint8_t imgBalls[17][28];

static const unsigned imgTitleWidth = 384;
static const unsigned imgTitleHeight = 256;
static const unsigned imgTitleSize = imgTitleWidth + (imgTitleHeight << 8);
extern uint8_t imgTitle[2934];
extern uint8_t imgTitleColors[598];

static const unsigned imgKingLoseWidth = 48;
static const unsigned imgKingLoseHeight = 62;
static const unsigned imgKingLoseSize = imgKingLoseWidth + (imgKingLoseHeight << 8);
extern uint8_t imgKingLose[744];

static const unsigned imgKingWidth = 48;
static const unsigned imgKingHeight = 126;
static const unsigned imgKingSize = imgKingWidth + (imgKingHeight << 8);
extern uint8_t imgKing[1512];

static const unsigned imgPlayerDWidth = 40;
static const unsigned imgPlayerDHeight = 4;
static const unsigned imgPlayerDSize = imgPlayerDWidth + (imgPlayerDHeight << 8);
extern uint8_t imgPlayerD[40];

static const unsigned imgFreeWidth = 24;
static const unsigned imgFreeHeight = 20;
static const unsigned imgFreeSize = imgFreeWidth + (imgFreeHeight << 8);
extern uint8_t imgFree[60];

static const unsigned imgPlayerWinWidth = 16;
static const unsigned imgPlayerWinHeight = 16;
static const unsigned imgPlayerWinSize = imgPlayerWinWidth + (imgPlayerWinHeight << 8);
extern uint8_t imgPlayerWin[64];
