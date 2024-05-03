SOURCES=$(wildcard src/*.cpp) $(wildcard src/*/*.cpp)
OBJECTS=$(patsubst src/%,build/%,${SOURCES:.cpp=.o})
DIRS=$(sort $(dir ${OBJECTS}))

CXXFLAGS=
LDFLAGS=-lGL -lglfw -lglad -lopenal

NAME=kate
BINARY=out/${NAME}

VARIANT?=cosmac
ifeq ($(VARIANT),cosmac)
CXXFLAGS+=-DQUIRK_F
endif

.PHONY: all
all: dirs ${BINARY}

${BINARY}: ${OBJECTS}
	g++ ${LDFLAGS} -o $@ $^

build/%.o: src/%.cpp
	g++ ${CXXFLAGS} -o $@ -c $<

.PHONY: dirs
dirs:
	mkdir -p build/ ${DIRS}
	mkdir -p out/

.PHONY: clean
clean:
	-rm -r build/
	-rm -r out/
