MKDEP=/usr/X11R6/bin/makedepend -Y
CC = g++
CFLAGS = -g -Wall -Wno-deprecated
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lglut -lm
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglut32 -lglu32 -lopengl32
endif

HDRS = xvec.h xmat.h
SRCS = splines.cpp
HDRS_SLN = 
SRCS_SLN = drawsplines.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

splines: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* splines

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

splines.o: xvec.h
drawsplines.o: xvec.h xmat.h
xmat.o: xvec.h
