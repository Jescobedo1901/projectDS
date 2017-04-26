
CXX =		    g++

CFLAGS =	    -g -std=c++03 -Wall -Wextra -Wpedantic \
		    -Wno-unused-variable -Wno-unused-parameter

LFLAGS =	    -lpthread -lm -lGLU -lGL -lrt -lX11 ./libggfonts.a 

HEADERS =	    game.h fonts.h ppm.h

TARGET =	    ds

.PHONY:		    $(TARGET) all clean

all:		    $(TARGET)

# Link
$(TARGET): jacobE.o main.o marcelF.o omarO.o physics.o ppm.o render.o seanC.o
	$(CXX) physics.o render.o jacobE.o main.o marcelF.o omarO.o seanC.o ppm.o $(LFLAGS)  -o $(TARGET) 

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
