C_FILES=lines.c path.c hal.c graph/imgBalls.c graph/imgBoard.c graph/imgScreen.c graph/imgPlayer.c graph/imgPlayerWin.c graph/imgKingLose.c graph/imgTitle.c
H_FILES=lines.h path.h hal.h graph/imgBalls.h graph/imgBoard.h graph/imgScreen.h graph/imgPlayer.h graph/imgPlayerWin.h graph/imgKingLose.h graph/imgTitle.h

all: lines.com

lines.com: $(C_FILES) $(H_FILES)
	../../c8080 -DARCH_86RK -DARCH_MICRO80_COLOR $(C_FILES) -o lines.com

convert/convert: convert/*.cpp convert/*.h
	g++ -oconvert/convert --std=c++17 convert/convert.cpp convert/png.cpp -lpng convert/fs_tools.cpp 

format:
	clang-format -i $(C_FILES) $(H_FILES) convert/*.cpp convert/*.h

graph/imgBalls.c: ./convert/convert graph/imgBalls.png
	./convert/convert 1 graph/imgBalls.png

graph/imgBoard.c: ./convert/convert graph/imgBoard.png
	./convert/convert 1 graph/imgBoard.png

graph/imgTitle.c: ./convert/convert graph/imgTitle.png
	./convert/convert 0 graph/imgTitle.png

graph/imgScreen.c: ./convert/convert graph/imgScreen.png
	./convert/convert 0 graph/imgScreen.png

graph/imgPlayer.c: ./convert/convert graph/imgPlayer.png
	./convert/convert 2 graph/imgPlayer.png

graph/imgPlayerWin.c: ./convert/convert graph/imgPlayerWin.png
	./convert/convert 2 graph/imgPlayerWin.png

graph/imgKingLose.c: ./convert/convert graph/imgKingLose.png
	./convert/convert 2 graph/imgKingLose.png



