INCLUDES =	    -I ./

SOURCES =	    $(wildcard *.cpp)

CXX =		    g++

CFLAGS =	    -g -std=c++03 -Wall -Wextra -Wpedantic \
		    -Wno-unused-variable -Wno-unused-parameter
	
LFLAGS =	    ./libggfonts.a -lpthread -lm -lGLU -lGL -lrt -lX11

TARGET =	    ds    

.PHONY:		    $(TARGET) all clean

all: $(TARGET)

# Link
$(TARGET):  $(SOURCES)
	$(CXX) $(SOURCES) $(CFLAGS) $(LFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
