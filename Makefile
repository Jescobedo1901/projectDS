
CXX =		    g++

CFLAGS =	    -g -std=c++03 -Wall -Wextra -Wpedantic \
		    -Wno-unused-variable -Wno-unused-parameter \
		    -o2

LFLAGS =	    -lpthread -lm -lGLU -lGL -lrt -lX11 ./libggfonts.a

OALFLAGS =	    -lopenal -lalut

HEADERS =	    game.h fonts.h ppm.h

TARGET =	    ds
TARGET_NO_AUDIO	=   ds_noaudio

.PHONY:		    all clean

all:		    $(TARGET) $(TARGET_NO_AUDIO)

# Link
$(TARGET): jacobE.o main.o marcelF.o omarO.o physics.o ppm.o render.o seanC.o audio.o
	$(CXX) physics.o render.o jacobE.o main.o marcelF.o omarO.o seanC.o audio.o ppm.o $(LFLAGS) $(OALFLAGS) -o $(TARGET)

$(TARGET_NO_AUDIO): jacobE.o main.o marcelF.o omarO.o physics.o ppm.o render.o seanC.o audio_disabled.o
	$(CXX) physics.o render.o jacobE.o main.o marcelF.o omarO.o seanC.o audio_disabled.o ppm.o  $(LFLAGS) -o $(TARGET_NO_AUDIO)

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

audio.o: audio.cpp $(HEADERS)
	$(CXX) $(CFLAGS) -D USE_OPENAL_SOUND -c audio.cpp -o audio.o
	
audio_disabled.o: audio.cpp $(HEADERS)	
	$(CXX) $(CFLAGS) -D DISABLE_AUDIO -c audio.cpp -o audio_disabled.o

clean:
	-rm -f $(TARGET) $(TARGET_NO_AUDIO) *.o
