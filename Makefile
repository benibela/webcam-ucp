PROGNAME = anaglyph_render

RM=/bin/rm -f
CXX=g++

INCLUDES=  -I. 
LIBS = -LGL -lGL -lglut 

DEFINES= $(INCLUDES) 
CFLAGS= -O3 -g -Wall $(DEFINES) `pkg-config --cflags --libs opencv`
SOURCES = main.cpp app.cpp camera.cpp realcamera.cpp
HEADERS = glh_linear.h app.h camera.h realcamera.h

OBJS = $(SOURCES:.cpp=.o) 

.cpp.o:
	$(RM) $@
	$(CXX) $(CFLAGS) -c $*.cpp -o $@

all: $(PROGNAME) 

$(PROGNAME) : $(OBJS) $(HEADERS) 
	$(CXX) $(CFLAGS) -o $(PROGNAME) $(OBJS) $(LIBS)

clean:
	$(RM) $(OBJS) $(PROGNAME) *~
