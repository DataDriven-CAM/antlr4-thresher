ext=so
libprefix=lib
ifeq ($(OS),Windows_NT)
  ext=dll
  libprefix=
endif
LD=ld.exe

MODULE_DIRECTORY ?= ./cpp_modules/

all: build/src/parse/G4Reader.o build/src/dsl/Morpher.o $(libprefix)antlr4thresher.$(ext) build/tool/src/main.o cantlrtool
$(libprefix)antlr4thresher.$(ext): CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26  -Iinclude -MMD 
$(libprefix)antlr4thresher.$(ext): LDFLAGS= -shared  -Wl,--allow-multiple-definition -L`pwd` -lc -lstdc++exp 
ifeq ($(OS),Windows_NT)
$(libprefix)antlr4thresher.$(ext): LDFLAGS=" -Wl,--export-all-symbols ${LDFLAGS}"
endif
$(libprefix)antlr4thresher.$(ext): build/src/parse/G4Reader.o build/src/dsl/Morpher.o 
	@mkdir -p $(@D)
	#ld --help
	$(CXX) $(LDFLAGS) -o $(libprefix)antlr4thresher.$(ext) $(wildcard build/src/parse/*.o)  $(wildcard build/src/dsl/*.o)

cantlrtool: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26  -Iinclude -MMD 
cantlrtool: LDFLAGS= -Wl,-rpath,"$$ORIGIN" -Wl,--allow-multiple-definition -L../antlr4-thresher -L`pwd` -lantlr4thresher -lc -lstdc++exp 
cantlrtool: build/tool/src/main.o
	$(CXX) $(LDFLAGS) -o cantlrtool $(wildcard build/tool/src/*.o)

build/src/parse/G4Reader.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -I./include -I$(MODULE_DIRECTORY)/fmt/dist/include -I$(MODULE_DIRECTORY)/graph-v2/include -I$(MODULE_DIRECTORY)/mio/include -MMD -MP
build/src/parse/G4Reader.o: ./src/parse/G4Reader.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/parse/G4Reader.o ./src/parse/G4Reader.cpp

build/src/dsl/Morpher.o: CXXFLAGS= -DNDEBUG -O3 -fPIC -pthread -std=c++26 -I./include -I$(MODULE_DIRECTORY)/fmt/dist/include -I$(MODULE_DIRECTORY)/graph-v2/include -I$(MODULE_DIRECTORY)/mio/include -MMD -MP
build/src/dsl/Morpher.o: ./src/dsl/Morpher.cpp 
	pwd
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o build/src/dsl/Morpher.o ./src/dsl/Morpher.cpp

#build/src/publishing/CodeGenerator.o: CXXFLAGS= -DNDEBUG -O3 -pthread -std=c++26 -I../include -I$(MODULE_DIRECTORY)/fmt/dist/include -I$(MODULE_DIRECTORY)/graph-v2/include -I$(MODULE_DIRECTORY)/mio/include -MMD -MP
#build/src/publishing/CodeGenerator.o: ./src/publishing/CodeGenerator.cpp 
#	pwd
#	@mkdir -p $(@D)
#	$(CXX) $(CXXFLAGS) -c -o build/src/publishing/CodeGenerator.o ./src/publishing/CodeGenerator.cpp

build/tool/src/main.o : CXXFLAGS=-std=c++26 -DNDEBUG -O3 -fPIC -fpermissive -I./include -I$(MODULE_DIRECTORY)/CLI11/include -I$(MODULE_DIRECTORY)/fmt/dist/include -I$(MODULE_DIRECTORY)/graph-v2/include -MMD
build/tool/src/main.o : ./tool/src/main.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o build/tool/src/main.o -c ./tool/src/main.cpp

clean:
	rm -rf build/src

