CPP_FILES=$(shell find . -name "*.cpp" -not \( -path "./test*" -or -path "./build*" \))
H_FILES=  $(shell find . -name "*.h"   -not \( -path "./test*" -or -path "./build*" \))
OBJ_FILES=$(addprefix build/,$(CPP_FILES:%.cpp=%.o))
W64_FILES=$(addprefix build.w64/,$(CPP_FILES:%.cpp=%.o))

all: c8080 files

.PHONY: clean
clean:
	rm -r -f build build.w64 c8080.creator.user c8080.files

.PHONY: files
files:
	find . -type f >c8080.files

c8080: ${OBJ_FILES}
	g++ -Wall -Wno-switch -g -static --std=c++17 -o $@ $^

build/%.o: %.cpp
	mkdir -p $(dir $@)
	g++ -MD -Wall -Wno-switch -g -static --std=c++17 -c -o $@ $<

-include $(OBJ_FILES:.o=.d)

c8080.exe: ${W64_FILES}
	x86_64-w64-mingw32-g++ -O2 -static --std=c++17 -o $@ $^

build.w64/%.o: %.cpp
	mkdir -p $(dir $@)
	x86_64-w64-mingw32-g++ -O2 -static --std=c++17 -c -o $@ $<

-include $(W64_FILES:.o=.d)

.PHONY: check
check: 
	cppcheck --language=c++ ${CPP_FILES} ${H_FILES}

.PHONY: format
format:
	clang-format -i ${CPP_FILES} ${H_FILES} $(shell find test -name "*.c" -or -name "*.h") $(shell find include -name "*.c" -or -name "*.h")


