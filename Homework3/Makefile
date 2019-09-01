LDLIBS =
#GLDLIBS = -framework glut OpenGL -framework GLU -framework GL
LDFLAGS  = $(LDLIBS) $(GLDLIBS) -lGL -lGLU -lglut -lm
# -lglfw

CC = gcc -O3 -Wall

PROGS = hw3

default: $(PROGS)

hw3: Homework3_Lindberg.o
	$(CC) -o $@ Homework3_Lindberg.o   $(LDFLAGS)

clean::	
	rm Homework3_Lindberg.o
	rm hw3

%.o:: %.c
	$(CC) -c  $< -o $@
