LDLIBS =
#GLDLIBS = -framework glut OpenGL -framework GLU -framework GL
LDFLAGS  = $(LDLIBS) $(GLDLIBS) -lGL -lGLU -lglut -lm

CC = gcc -O3 -Wall 

PROGS = Scene3D

default: $(PROGS)

Scene3D: Scene3D.o
	$(CC) -o $@ Scene3D.o   $(LDFLAGS)

clean::	
	rm Scene3D.o
	rm Scene3D

%.o:: %.c
	$(CC) -c  $< -o $@
