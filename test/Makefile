
MODULE_DIRECTORY ?= ../cpp_modules/

all: LDFLAGS= -fuse-ld=gold -L../  -lantlr4thresher -lc -lstdc++exp -lz -ldl -pthread 
all: build/src/formatting_test.o build/src/generator_test.o test
	$(CXX) $(LDFLAGS) -o test $(wildcard build/src/*.o)  $(wildcard build/src/parse/*.o) $(wildcard build/src/dsl/*.o) $(wildcard build/src/publishing/*.o)  

build/src/formatting_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -fconcepts-diagnostics-depth=2 -I../include -I../cpp_modules/doctest -I../src -I../src/parsing -I../cpp_modules/mio/include -I../cpp_modules/graph-v2/include -I../cpp_modules/zlib/dist/include -I../cpp_modules/antlr4/runtime/Cpp/run/include/antlr4-runtime -MMD -Wl,--allow-multiple-definition
build/src/formatting_test.o: src/formatting_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/formatting_test.o src/formatting_test.cpp

build/src/generator_test.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -fconcepts-diagnostics-depth=2 -I../include -I../cpp_modules/doctest -I../src -I../src/parsing -I../cpp_modules/mio/include -I../cpp_modules/graph-v2/include -I../cpp_modules/zlib/dist/include -MMD -Wl,--allow-multiple-definition
build/src/generator_test.o: src/generator_test.cpp
	$(CXX) $(CXXFLAGS) -c -o build/src/generator_test.o src/generator_test.cpp

test: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -I../include -I../cpp_modules/fmt/dist/include -I../cpp_modules/graph-v2/include -I../cpp_modules/mio/include -I../cpp_modules/doctest -MMD -MP
test: src/test.cpp
	pwd
	$(CXX) $(CXXFLAGS) -c -o build/src/test.o src/test.cpp 

clean:
