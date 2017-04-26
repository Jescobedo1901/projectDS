
CXX =		    g++

CFLAGS =	    -g -std=c++03 -Wall -Wextra -Wpedantic \
		    -Wno-unused-variable -Wno-unused-parameter

LFLAGS =	    ./libggfonts.a -lpthread -lm -lGLU -lGL -lrt -lX11

HEADERS =	    game.h fonts.h ppm.h

TARGET =	    ds

.PHONY:		    $(TARGET) all clean

all: jacobE.o main.o marcelF.o physics.o ppm.o render.o seanC.o #$(TARGET)

# Link
$(TARGET): jacobE.o jacobE.o main.o marcelF.o physics.o ppm.o ppm.o render.o seanC.o
	$(CXX) -o $(TARGET) $(LFLAGS) jacobE.o main.o marcelF.o physics.o ppm.o render.o seanC.o

jacobE.o: jacobE.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c jacobE.cpp -o jacobE.o

main.o: main.cpp  $(HEADERS)
	$(CXX) $(CFLAGS) -c main.cpp -o main.o

marcelF.o: marcelF.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c marcelF.cpp -o marcelF.o

omarO.o: omarO.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c omarO.cpp -o omarO.o

physics.o: physics.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c physics.cpp -o physics.o

ppm.o: ppm.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c ppm.cpp -o ppm.o

render.o: render.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c render.cpp -o render.o

seanC.o: seanC.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -c seanC.cpp -o seanC.o

clean:
	-rm -f $(TARGET) *.o
