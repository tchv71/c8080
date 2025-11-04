all: convert_png_to_micro80

convert_png_to_micro80: *.cpp *.h
	g++ -std=c++17 -O2 -o$@ -static *.cpp -lpng -lz

convert_png_to_micro80.exe: *.cpp *.h
	x86_64-w64-mingw32-g++ -std=c++17 -o$@ -static *.cpp -lpng -lz

convert_png_to_micro80.macos: *.cpp *.h
	g++ -std=c++17 -O2 -o$@ *.cpp -lpng -lz