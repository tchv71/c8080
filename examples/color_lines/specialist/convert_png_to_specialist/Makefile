all: convert_png_to_specialist

convert_png_to_specialist: *.cpp *.h
	g++ -std=c++17 -O2 -o$@ -static *.cpp -lpng -lz

convert_png_to_specialist.exe: *.cpp *.h
	x86_64-w64-mingw32-g++ -std=c++17 -o$@ -static *.cpp -lpng -lz
