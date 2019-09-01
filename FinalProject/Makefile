LDLIBS =
#GLDLIBS = -framework glut OpenGL -framework GLU -framework GL
LDFLAGS  = $(LDLIBS) $(GLDLIBS) -lGL -lGLU -lglut -lm
# -lglfw

CC = gcc -O0 -Wall

PROGS = final 

default: $(PROGS)

final: FinalProj_Lindberg.o
	$(CC) -o $@ FinalProj_Lindberg.o   $(LDFLAGS)

clean::	
	rm FinalProj_Lindberg.o
	rm final 

%.o:: %.c
	$(CC) -c  $< -o $@
