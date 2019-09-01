/*
 * Texture and lighting
 * Author: Daniel Lindberg
 * Course: CSCI 5229: Computer Graphics
 */

//Built in C includes
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

GLuint iron_man;
// The x coordinate viewpoint location
static GLfloat viewpoint_x_rotation = 10.0;
// The y coordinate viewpoint location
static GLfloat viewpoint_y_rotation = 30.0;

GLfloat pos[3] = {0,0,0};
GLfloat theta[3] = {0,0,0};
// The z coordinate viewpoint location
float pi = 3.14159;
/*
 *  Main
 */

void genCircle(GLfloat radius, GLfloat x_coord, GLfloat y_coord, GLfloat z_coord)
{
	GLfloat angle, x, y;
	glBegin(GL_POLYGON);
	for(angle = 0; angle < 2 * pi; angle+= 0.1)
	{
		x = (radius * cos(angle)) + x_coord;
		y = (radius * sin(angle)) + y_coord;
		glVertex2f(x,y);
	}
	glEnd();
}

void genCenter(GLfloat radius, GLfloat x_coord, GLfloat y_coord)
{
	GLfloat angle, x, y, z;
	glBegin(GL_POLYGON);
	for(z = -0.1; z < 0.1 ; z += 0.01)
	{
		for(angle = 0; angle < 2 * pi; angle += pi/100)
		{
			x = (radius * cos(angle)) + x_coord;
			y = (radius * sin(angle)) + y_coord;
			glVertex3f(x,y,z);
		}
	}
	for(angle = 0; angle < 2 * pi; angle += pi/100)
	{
		x = (radius * cos(angle)) + x_coord;
		y = (radius * sin(angle)) + y_coord;
		glVertex3f(x,y,z);
	}
	glEnd();
}

void genPetals(GLfloat x_coord, GLfloat y_coord, GLfloat radian, GLfloat radius)
{
	glPushMatrix();
	GLfloat degrees = radian * 180/pi;
	glRotatef(degrees, 0.0, 0.0, 1.0);
	glTranslatef(2*radius , 0.0, 0.0);

	glBegin(GL_TRIANGLE_FAN);
	float random_number_one = ((float)rand()/RAND_MAX) * 256;
	float random_number_two = ((float)rand()/RAND_MAX) * 256;
	float random_number_three = ((float)rand()/RAND_MAX) * 256;
	GLfloat random_triple[3] = {random_number_one, random_number_two, random_number_three};
	glColor3fv(random_triple);
	glVertex3f(-radius/5 , 0.0, -0.2);
	random_number_one = ((float)rand()/RAND_MAX) * 256;
	random_number_two = ((float)rand()/RAND_MAX) * 256;
	random_number_three = ((float)rand()/RAND_MAX) * 256;
	GLfloat random_triple_two[3] = {random_number_one, random_number_two, random_number_three};
	glColor3fv(random_triple_two);

    GLfloat angle, depth;
    for(angle = 0; angle < 2.01*pi; angle += 2*pi/100){
      if(angle <= pi) {
        depth = 0.2 * (1-(angle/pi));
    }
    else {
      depth = 0.2 * ((angle-pi)/pi);
    }
    glVertex3f(2*radius * cos(angle), radius/2 * sin(angle), depth);
    }
    glEnd();

    //pop the rotated matrix, so we go back to original
    glPopMatrix();
}

void lorenzIdle(void)
{
	/* Have to have an idle function, so that GLUT can
	perform background tasks, or continuous animation.
	Will redisplay 
	*/
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

/*static void objRedraw(int width, int height)
{
  //Get the resolution of the redraw. Resolution is always h/w
  GLfloat resolution = (GLfloat) height / (GLfloat) width;
  //Set the inital viewport to 0,0 and the maximum width and height
  glViewport(0, 0, (GLint) width, (GLint) height);
  // Set the display to be projected on the matrix stack
  glMatrixMode(GL_PROJECTION);
  //Replace current matrix with identity matrix, (clears out matrix stack)
  glLoadIdentity();
   This will multiply the matrix by a perspective matrix
  first param is left and to the right, I am doing 2 x to enlarge
  The third and fourth are the bottom and top, we are taking the resolution
  and multiplying it by 2, making this an equal matrix size
  last two parameters are nearVal and farVal. Specify distances
  near and far depth clipping planes
  
  //float multipler_gui = 2.0;
  //glFrustum(-multipler_gui, multipler_gui, -resolution*multipler_gui, resolution*multipler_gui, 1.0, 200.0);
  gluPerspective(60, resolution, 0.1, 1000.0);
  // Now instead of displaying identity, we display the model
  glMatrixMode(GL_MODELVIEW);
  // We will load the identity matrix into the stack
  glLoadIdentity();
  // Multiply current matrix by translation matrix
  glTranslatef(0.0, 0.0, -70.0);
}*/


void sceneDisplay(){
	/*Seems like via the OpenGL library documentation.
	that open gl keeps a stack of matrics to quickly apply
	and remove transformations. glPushMatrix copies
	the top matrix and pushes it onto the stack
	*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	//drawIronMan();

	glTranslatef(pos[0], pos[1], pos[2]);

	glutSwapBuffers();
	//Clear the stack
	glPopMatrix();
	glRotatef(theta[0], 1.0, 0.0, 0.0);
	glRotatef(theta[1], 0.0, 1.0, 0.0);
	glRotatef(theta[2], 0.0, 0.0, 1.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLfloat radius = 0.5;

	float random_number_one = ((float)rand()/RAND_MAX) * 256;
	float random_number_two = ((float)rand()/RAND_MAX) * 256;
	float random_number_three = ((float)rand()/RAND_MAX) * 256;
	GLfloat random_triple[3] = {random_number_one, random_number_two, random_number_three};
	glColor3fv(random_triple);

	int total_petals = 5;

	GLfloat angle, x, y;
	for (int i = 0; i < total_petals; i ++)
	{
		angle = i * (2 * pi / total_petals);
		x = radius * cos(angle);
		y = radius * sin(angle);
		genPetals(x, y, angle, radius);
	}
	glColor3fv(random_triple);
	genCenter(radius, 0, 0);

	glFlush();


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
   // We want to put glut into display going, adding depth for the 3D scene
   glutInitDisplayMode(GLUT_RGB| GLUT_DOUBLE| GLUT_DEPTH);
   // setting a default size
   glutInitWindowSize(1000,1000);
   glutCreateWindow("Lindberg Homework2");
   glEnable(GL_DEPTH_TEST);
   glClearColor(0,0,0,0);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(60, 1, 1.0, 100.0);
   glMatrixMode(GL_MODELVIEW);
   pos[2] = -5;
   //now we need to pass the display function 
   glutDisplayFunc(sceneDisplay);
   // sets reshape callback for current window
   // Triggered when a window is reshaped.
   //Need with the special function
   //glutReshapeFunc(objRedraw);
   // Invoked when keys are pressed, they have an API
   // to interface it
   glutSpecialFunc(lorenzSpecial);
   //This idle function allows GLUT to perform background processing
   //tasks , when there are no new events. helps keep up GUI
   glutIdleFunc(lorenzIdle);
   glutMainLoop();
   return 0;
}
