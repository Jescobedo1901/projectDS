INCLUDES =	    -I ./include -I ./ext/include

# Workaround for X11 system headers
ifeq ($(shell uname -s),Darwin)
INCLUDES  +=	-isystem /opt/X11/include
endif #End workaround

DEPEXT =	    dep

SOURCES =	    $(wildcard src/*.cpp) \
		    $(wildcard *.cpp) \
		    $(wildcard src/ds/*/*.cpp) \
		    $(wildcard ext/src/*.cpp)

HEADERS =	    $(wildcard include/ds/*/*.h)

OBJECTS =	    $(SOURCES:.cpp=.o)

CXX =		    g++

CFLAGS =	    -std=c++1y -Wall -Wextra -Wpedantic \
		    -rdynamic				\
		    -Wno-unused-variable -Wno-unused-parameter \
		    -pipe
ifeq ($(RELEASE),1)
    CFLAGS +=	    -march=haswell -ofast \
		    -flto -ffunction-sections -fdata-sections -fdce \
		    -floop-parallelize-all -ftree-parallelize-loops=4
else
    CFLAGS +=	    -g -fdiagnostics-color=auto
endif

LFLAGS_DIRS =	    

LFLAGS =	    ./ext/lib/libggfonts.a -lpthread -lm -lGLU -lGL -lrt -lX11

TARGET =	    ds    

.PHONY:		    $(TARGET) all clean

all: $(TARGET)

# Link
$(TARGET):  $(OBJECTS)
	$(CXX) $(OBJECTS) $(INCLUDES) $(CFLAGS) $(LFLAGS_DIRS) $(LFLAGS) -o $(TARGET)

# Compile
%.o: %.cpp
	$(CXX) $(INCLUDES) $(CFLAGS) -c $< -o $@
	@# Generate compile dependency graph on file changes
	$(CXX) -MM $(INCLUDES) $(CFLAGS) -c $< > $(patsubst %.o, %.$(DEPEXT), $@)

#Include object dependency override if any
-include $(OBJECTS:.o=.$(DEPEXT))

clean:
	rm -f $(SOURCES:.cpp=.$(DEPEXT)) $(OBJECTS) $(TARGET)
