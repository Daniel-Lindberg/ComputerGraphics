/*
 * Simple program to demonstrate generating coordinates
 * using the Lorenz Attractor
 * Author: Daniel Lindberg
 * Course: CSCI 5229: Computer Graphics
 */

//Built in C includes
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//Glut includes
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define SIZE_COORDINATES 50000
/*  Lorenz Parameters  */
double s  = 10;
double b  = 2.6666;
double r  = 28;

int counter = 0;
int increment = 10;

// The x coordinate viewpoint location
static GLfloat viewpoint_x_rotation = 10.0;
// The y coordinate viewpoint location
static GLfloat viewpoint_y_rotation = 30.0;
// The z coordinate viewpoint location
static GLfloat viewpoint_z_rotation = 50.0;

//We want a depth of 3
float coordinate_points[SIZE_COORDINATES][3];

/*
 *  Main
 */


static void lorenzAlgorithm(void){
   /* This is the professor's default code given in the
   assignment description. However I have added in some information
   it into an array */
   int i;
   /*  Coordinates  */
   double x = 1;
   double y = 1;
   double z = 1;
   coordinate_points[0][0] = x;
   coordinate_points[0][1] = y;
   coordinate_points[0][2] = z;
   /*  Time step  */
   double dt = 0.001;

   // We do not need the print statements
   //printf("%5d %8.3f %8.3f %8.3f\n",0,x,y,z);
   /*
    *  Integrate 50,000 steps (50 time units with dt = 0.001)
    *  Explicit Euler integration
    */
   for (i=0;i<SIZE_COORDINATES-1;i++)
   {
      float dx = s*(y-x);
      float dy = x*(r-z)-y;
      float dz = x*y - b*z;
      x += dt*dx;
      y += dt*dy;
      z += dt*dz;

      /* Iterate through all 3 dimensions of the coordinate
	  points, We are adding + 1 because we declared the base
	  ones at the top of this algorithm function
	  */
      coordinate_points[i+1][0] = x;
      coordinate_points[i+1][1] = y;
      coordinate_points[i+1][2] = z;
      //We don't need prints since I am adding them to
      // coordinates
      //printf("%5d %8.3f %8.3f %8.3f\n",i+1,x,y,z);
   }
}

void lorenzIdle(void)
{
	/* Have to have an idle function, so that GLUT can
	perform background tasks, or continuous animation.
	Will redisplay 
	*/
	counter = counter + increment;
	glutPostRedisplay();
}
static void lorenzSpecial(int key_button, int x, int y)
{
	/*
	Lorenz special helps move around the viewpoint
	location of the graphical display

	We don't use the x and y, but if not included
	produces a warning
	*/
	//Basic switch statements for the key buttons
	switch (key_button) {
		//Utilizes the GLUT key API
		case GLUT_KEY_UP:
			//Thought up and down would be z rotation
			// Found out it was x 
			viewpoint_x_rotation += 3.0;
			break;
		case GLUT_KEY_DOWN:
			viewpoint_x_rotation -= 3.0;
			break;
		case GLUT_KEY_LEFT:
			viewpoint_y_rotation += 3.0;
			break;
		case GLUT_KEY_RIGHT:
			viewpoint_y_rotation -= 3.0;
			break;
		default:
		// If none of the arrow keys move, do nothing
			return;
	}
glutPostRedisplay();
}

static void lorenzKeyHandler(unsigned char key, int x, int y)
{
	/*
	Lorenz keyhandler helps change the values of:
	s b and r
	key|   Up   |	Down
	s  |   q    |    a
	b  |   w    |    s
	r  |   e    |    d
	We don't use the x and y, but if not included
	produces a warning
	*/
	//Basic switch statements for the keyboard buttons
	//Utilizes the GLUT key API
	if (key == 'q')
	{
		//q and a will move s up and down
		s += 0.3;
	}
	else if (key == 'a')
	{
		s -= 0.3;
	}
	else if (key == 'w')
	{
		//w and s move b up and down
		b += 0.3;
	}
	else if (key == 's')
	{
		b -= 0.3;
	}
	else if (key == 'e')
	{
		//e and d move the r up and down 
		r += 0.3;
	}
	else if (key == 'd')
	{
		r -= 0.3;
	}
	printf("s:%f b:%f r:%f \n", s, b, r);
	glutPostRedisplay();
}


static void lorenzRedraw(int width, int height)
{
  //Get the resolution of the redraw. Resolution is always h/w
  GLfloat resolution = (GLfloat) height / (GLfloat) width;
  //Set the inital viewport to 0,0 and the maximum width and height
  glViewport(0, 0, (GLint) width, (GLint) height);
  // Set the display to be projected on the matrix stack
  glMatrixMode(GL_PROJECTION);
  //Replace current matrix with identity matrix, (clears out matrix stack)
  glLoadIdentity();
  /* This will multiply the matrix by a perspective matrix
  first param is left and to the right, I am doing 2 x to enlarge
  The third and fourth are the bottom and top, we are taking the resolution
  and multiplying it by 2, making this an equal matrix size
  last two parameters are nearVal and farVal. Specify distances
  near and far depth clipping planes
  */
  float multipler_gui = 2.0;
  glFrustum(-multipler_gui, multipler_gui, -resolution*multipler_gui, resolution*multipler_gui, 1.0, 200.0);
  // Now instead of displaying identity, we display the model
  glMatrixMode(GL_MODELVIEW);
  // We will load the identity matrix into the stack
  glLoadIdentity();
  // Multiply current matrix by translation matrix
  glTranslatef(0.0, 0.0, -70.0);
}

void lorenzDisplay(){
	/*Seems like via the OpenGL library documentation.
	that open gl keeps a stack of matrics to quickly apply
	and remove transformations. glPushMatrix copies
	the top matrix and pushes it onto the stack
	*/
	/*
	scanf("%lf %lf %lf", &s, &b, &r);
	if (s == 0.0)
	{
		printf("You have entered in 0 for s , defualting to 10\n");
		s = 10;
	}
	if (b == 0.0)
	{
		printf("You have entered in 0 for b , defualting to 2.6666\n");
		b = 2.6666;
	}
	if (r == 0.0)
	{
		printf("You have entered in 0 for r , defualting to 28\n");
		s = 28;
	}
	printf("You have entered: %lf for s %lf for b %lf for r. \n", s, b, r);
	*/
	//First thing we want to do is generate the lorenz algorithm
    lorenzAlgorithm();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	//Below are the rotations, noticed that we need sort of
	// a weird angle to see the inception of the lines
	//x rotation
	glRotatef(viewpoint_x_rotation, 1.0, 0.0, 0.0);
	//y rotation
	glRotatef(viewpoint_y_rotation, 0.0, 1.0, 0.0);
	//z rotation
	glRotatef(viewpoint_z_rotation, 0.0, 0.0, 1.0);
	glBegin(GL_LINE_STRIP);
	int i = 0;
	while ( i < counter && i < SIZE_COORDINATES)
	{
		/*I tried some random numbers to try and get a random color
		  Turns out by iterating the coordinate points we generate a random color
		*/
		/* 
		float random_number_one = ((float)rand()/RAND_MAX) * 256;
		float random_number_two = ((float)rand()/RAND_MAX) * 256;
		float random_number_three = ((float)rand()/RAND_MAX) * 256;
		GLfloat random_triple[3] = {random_number_one, random_number_two, random_number_three};
		*/
		glColor3fv(coordinate_points[i]);
		glVertex3fv(coordinate_points[i++]);
	}
	glEnd();
	if (counter < SIZE_COORDINATES)
	{
		/*We want to go through our counter and make sure
		  that if the incrementer goes over the max size
		  it resets it to the maximum of the size coordinates
		*/
		if (counter + increment > SIZE_COORDINATES)
		{
			counter = SIZE_COORDINATES;
		}
		else
		{
			//Keep incrementing the counter by the increment
			counter = counter + increment;
		}
	}
	//Flush the current matrix, and swap the buffers on
	//the stack
	glFlush();
	glutSwapBuffers();
	//Clear the stack
	glPopMatrix();
}



int main(int argc, char *argv[])
{
	// Get random seed going
	srand((unsigned)time(NULL));
   
   /*I took the following GUI intializaiton from the professors
   examples. I believe it was from ex6
   */
   // Next we want to get our display going
   glutInit(&argc, argv);
   // We want to put glut into display going
   glutInitDisplayMode(GLUT_RGB| GLUT_DOUBLE);
   // setting a default size
   glutInitWindowSize(1000,1000);
   glutCreateWindow("Lindberg Homework1");
   //now we need to pass the display function 
   glutDisplayFunc(lorenzDisplay);
   // sets reshape callback for current window
   // Triggered when a window is reshaped.
   //Need with the special function
   glutReshapeFunc(lorenzRedraw);
   // Invoked when keys are pressed, they have an API
   // to interface it
   glutSpecialFunc(lorenzSpecial);
   //Invoked when keyboard buttons are pressed, they have an PI
   // to interface it
   glutKeyboardFunc(lorenzKeyHandler);
   //This idle function allows GLUT to perform background processing
   //tasks , when there are no new events. helps keep up GUI
   glutIdleFunc(lorenzIdle);
   glutMainLoop();
   return 0;
}
