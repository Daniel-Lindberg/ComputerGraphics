#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


//Is the flower position for the first one
GLfloat flower_pos_1[3] = {1,1,1};

//Is the flower position for the second one, this one is in a different location
GLfloat flower_pos_2[3] = {3,2,1};


//Is the flower position for the third one, change the size
GLfloat flower_pos_3[3] = {-5,-7,-2};

//Is the first diamond position
GLfloat diamond_pos_1[3] = {0, 0, 0};

//Is the tree position
GLfloat banner_pos_1[3] = {0 , 0 , 0};


//The default values for the viewpoint
GLfloat x_viewpoint = 0.0;
GLfloat y_viewpoint = 0.0;
GLfloat z_viewpoint = 0.0;


//These are the random color triples for the color
GLfloat outer_petal_color[3], inner_petal_color[3], inner_circle_color[3], arrow_color[3], diamond_color[3];

//The banner color
GLfloat tree_color[3] = {0.60f, 0.40f, 0.12f};
GLfloat banner_color[3] = {0.0f, 0.87f, 0.0f};


float pi = 3.14159;


//draws the center of the flower.  This is basically a short column, made up of many circles.
void genCenter(GLfloat r, GLfloat x_coord, GLfloat y_coord) 
{
  GLfloat angle, x, y, z;
  // Use the polygon option to color effects on down
  glBegin(GL_POLYGON);
  // This produces the shading for the inner circle
  for(z = -0.1; z < 0.1; z += 0.01) 
  {
    for(angle = 0; angle < 2*pi; angle += pi/100) 
    {
    	//Calculate all X positions
      x = (r * cos(angle)) + x_coord;
      //Calculate all Y positions
      y = (r * sin(angle)) + y_coord;
      // Place the locations down
      glVertex3f(x, y, z);
    }
  }
  glEnd();
  //Use the GL polygon to apply colors
  glBegin(GL_POLYGON);
  for(angle = 0; angle < 2*pi; angle += pi/100) 
  {
  	// Calculate all X positions
    x = (r * cos(angle)) + x_coord;
    y = (r * sin(angle)) + y_coord;
    glVertex3f(x, y,z);
  }
  glEnd();

}


//draws a petal of the flower. the point of petal that meets the circle is at startx, starty. radians is the angle on the center circle at which the petal will be drawn, so it can be oriented in the correct direction. It is constructed from two arcs of 2 very big circle with radius radius.
void genPetal(GLfloat startx, GLfloat starty, GLfloat radians, GLfloat radius) 
{
  glPushMatrix();
  // Give the radian values and confert to degrees
  GLfloat degrees = radians * 180/pi;

  glRotatef(degrees, 0.0, 0.0, 1.0);
  glTranslatef(2*radius, 0.0, 0.0);

  //Use the triangle GL
  glBegin(GL_TRIANGLE_FAN);
  
  //Use my random color petals
  glColor3fv(outer_petal_color);
  glVertex3f(-radius/3, 0.0, -0.1);
  glColor3fv(inner_petal_color);

  GLfloat angle, z_value;
  for(angle = 0; angle < 2.02*pi; angle += 2*pi/100)
  {
    if(angle <=pi) 
    {
    	// Translate the values for the angle to its coordinate
    	//Produce depth forward and depth backwards
      z_value = 0.3 * (1-(angle/pi));
    }
    else 
    {
      z_value = 0.3 * ((angle-pi)/pi);
    }
    glVertex3f(2*radius * cos(angle), radius/2 * sin(angle), z_value);
  }
  glEnd();

  //pop the rotated matrix, so we go back to original
  glPopMatrix();
}

void genDiamond(GLint center_x, GLint center_y, GLint center_z, float size)
{
     
    //Clear the OpenGL matrix
	glPushMatrix();
	
	//Begin the opengl matrix drawing
	glBegin(GL_POLYGON);
	//Color this the color of the diamonds
	glColor3fv(diamond_color);
	//x_pos, y_pos, z_pos , multiply each by the size
    GLfloat x_pos = (center_x) * size;
    GLfloat y_pos = (center_y) * size; 
    GLfloat z_pos = (center_z) * size;

    //Do the drawing relative the position given
    glVertex3f(x_pos - 0.1, y_pos - 0.5, z_pos );
    glVertex3f(x_pos - 0.5, y_pos - 0.1, z_pos );
    glVertex3f(x_pos - 0.9, y_pos - 0.5, z_pos );
    glVertex3f(x_pos - 0.5, y_pos - 0.9, z_pos );
    
   	//End the drawing session and then push the matrix
    glEnd();
    glPopMatrix();
}

void genTube(int total_angles, int angle_size, float size)
{
	// Go through and generate tubes
   int i, j, k;
   double s_angle_diff, t_fix_lines, x, y, z, two_pi;

   // two pi is quite literally two radians
   two_pi = 2 * pi;
   for (i = 0; i < total_angles; i++) 
   {
   		// Use QUAD strip to get the polygons created
      glBegin(GL_QUAD_STRIP);
      // Use the arrow color
      glColor3fv(arrow_color);
      // arrow size is the amount of points in a circle to draw in between
      // More points means nicer looking tube
      for (j = 0; j <= angle_size; j++) 
      {
         for (k = 1; k >= 0; k--) 
         {
            s_angle_diff = (i + k) % total_angles + size;
            t_fix_lines = j % angle_size;

            x = (size+0.1*cos(s_angle_diff*two_pi/total_angles))*cos(t_fix_lines*two_pi/angle_size);
            y = (size+0.1*cos(s_angle_diff*two_pi/total_angles))*sin(t_fix_lines*two_pi/angle_size);
            z = 0.1 * sin(s_angle_diff * two_pi / total_angles);
            glVertex3f(x, y, z);
         }
      }
      glEnd();
    }
}

void genBanner(GLfloat x_coord, GLfloat y_coord, GLfloat z_coord, float size)
{
	glBegin(GL_QUADS);
	glColor3fv(banner_color);
	GLfloat x_pos = (x_coord) * size;
	GLfloat y_pos = (y_coord) * size;
	GLfloat z_pos = (z_coord) * size;

	glVertex3f(x_pos-5.05 , y_pos-40.5, z_pos + 15);
	glVertex3f(x_pos-5.05 , y_pos-20.5, z_pos - 15);
	glVertex3f(x_pos-9.02 , y_pos-40.5, z_pos + 15);
	glVertex3f(x_pos-9.02 , y_pos-20.5, z_pos - 15);
	glEnd();
}

/*void genTwig(int tree_seed)
{
	if ((tree_seed) == 0)
	{
		glLineWidth(5.0);
		glBegin(GL_LINE_STRIP);
		glColor3fv(tree_color);
		glVertex2f(-0.10, -80.5);
		glVertex2f(-0.10, -100.5);
		glEnd();
	}
}*/

void lorenzIdle(void)
{
	/* Have to have an idle function, so that GLUT can
	perform background tasks, or continuous animation.
	Will resceneDisplay 
	*/
	glutPostRedisplay();
}
static void lorenzSpecial(int key_button, int x, int y)
{
	/*
	Lorenz special helps move around the viewpoint
	location of the graphical sceneDisplay

	We don't use the x and y, but if not included
	produces a warning
	*/
	//Basic switch statements for the key buttons
	switch (key_button) 
	{
		//Utilizes the GLUT key API
		case GLUT_KEY_UP:
			//Thought up and down would be z rotation
			// Found out it was x 
			x_viewpoint = 90.0;
			y_viewpoint = 0.0;
			z_viewpoint = 0.0;
			//x_viewpoint += 3.0;
			break;
		case GLUT_KEY_DOWN:
			//x_viewpoint = -90.0;
			//y_viewpoint = 0.0;
			break;
		case GLUT_KEY_LEFT:
			x_viewpoint = 0.0;
			y_viewpoint = 0.0;
			z_viewpoint = 0.0;
			//gluPerspective(100, 1.0, 1.0, 100.0);
			break;
		case GLUT_KEY_RIGHT:
			x_viewpoint = 90.0;
			y_viewpoint = -90.0;
			z_viewpoint = 0.0;
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
	Value 		 |   Up   |	Down
	--------------------------------
	x_viewpoint  |   q    |    a
	y_viewpoint  |   w    |    s
	z_viewpoint  |   e    |    d
	We don't use the x and y, but if not included
	produces a warning
	*/
	//Basic switch statements for the keyboard buttons
	//Utilizes the GLUT key API
	if (key == 'q')
	{
		//q and a will move x_viewpoint up and down
		x_viewpoint += 0.3;
	}
	else if (key == 'a')
	{
		x_viewpoint -= 0.3;
	}
	else if (key == 'w')
	{
		//w and s move y_viewpoint up and down
		y_viewpoint += 0.3;
	}
	else if (key == 's')
	{
		y_viewpoint -= 0.3;
	}
	else if (key == 'e')
	{
		//e and d move the z_viewpoint up and down 
		z_viewpoint += 0.3;
	}
	else if (key == 'd')
	{
		z_viewpoint -= 0.3;
	}
	glutPostRedisplay();
}


void sceneDisplay(void) 
{
	/*Seems like via the OpenGL library documentation.
	that open gl keeps a stack of matrics to quickly apply
	and remove transformations. glPushMatrix copies
	the top matrix and pushes it onto the stack
	*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glMatrixMode(GL_MODELVIEW);

  glLoadIdentity();
 

  //Move the x y z position to what is specified
  glTranslatef(flower_pos_1[0], flower_pos_1[1], flower_pos_1[2]);
  glRotatef(x_viewpoint, 1.0, 0.0, 0.0);
  glRotatef(y_viewpoint, 0.0, 1.0, 0.0);
  glRotatef(z_viewpoint, 0.0, 0.0, 1.0);



  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  GLfloat radius = 0.5;

  glColor3fv(inner_circle_color);
  //draw petals
  GLint numPetals = 7;
  GLint i;
  GLfloat angle,x,y;
  for(i = 0; i < numPetals; i++)
  {
  	//Generate a petal based off of the number of petals
    angle = i*(2*pi / numPetals);
    x = radius * cos(angle);
    y = radius * sin(angle);
    // Generate a petal for everyone
    genPetal(x, y, angle, radius);
  }

  glColor3fv(inner_circle_color);
  //draw center circle
  genCenter(radius, 0, 0);

  genTube(4, 250, 5);

  //This is the start of the second flower
  //-----------------------------------------------------
  
  //Move the x y z position to what is specified
  glTranslatef(flower_pos_2[0], flower_pos_2[1], flower_pos_2[2]);
  glRotatef(x_viewpoint, 1.0, 0.0, 0.0);
  glRotatef(y_viewpoint, 0.0, 1.0, 0.0);
  glRotatef(z_viewpoint, 0.0, 0.0, 1.0);



  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glColor3fv(inner_circle_color);
  for(i = 0; i < numPetals; i++)
  {
  	//Generate a petal based off of the number of petals
    angle = i*(2*pi / numPetals);
    x = radius * cos(angle);
    y = radius * sin(angle);
    // Generate a petal for everyone
    genPetal(x, y, angle, radius);
  }

  glColor3fv(inner_circle_color);
  //draw center circle
  genCenter(radius, 0, 0);

  genTube(80, 25, 5);

  //This is the end of the second flower
  //-----------------------------------------------------

  //This is the start of the third flower
  //-----------------------------------------------------

  //Move the x y z position to what is specified
  glTranslatef(flower_pos_3[0], flower_pos_3[1], flower_pos_3[2]);
  glRotatef(x_viewpoint, 0.2, 0.0, 0.0);
  glRotatef(y_viewpoint, 0.0, 0.2, 0.0);
  glRotatef(z_viewpoint, 0.0, 0.0, 0.2);

  radius = 0.1;


  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glColor3fv(inner_circle_color);
  for(i = 0; i < numPetals; i++)
  {
  	//Generate a petal based off of the number of petals
    angle = i*(2*pi / numPetals);
    x = radius * cos(angle);
    y = radius * sin(angle);
    // Generate a petal for everyone
    genPetal(x, y, angle, radius);
  }

  glColor3fv(inner_circle_color);
  //draw center circle
  genCenter(radius, 0, 0);

  //This is the end of the third flower
  //-----------------------------------------------------

  
  genTube(80, 25, 1);


  //This is the start of the first diamond
  //-----------------------------------------------------
  GLint center_x = diamond_pos_1[0]+3;
  GLint center_y = diamond_pos_1[1]+3;
  GLint center_z = diamond_pos_1[2]+3;
  float sizeofdim = 1.0;
  genDiamond(center_x, center_y, center_z, sizeofdim);
  //End of first diamond
  //-----------------------------------------------------

   //This is the start of the second diamond
  //-----------------------------------------------------
  center_x = diamond_pos_1[0]+1;
  center_y = diamond_pos_1[1]-1;
  center_z = diamond_pos_1[2]-1;
  sizeofdim = 1.0;
  genDiamond(center_x, center_y, center_z, sizeofdim);
  //End of second diamond
  //-----------------------------------------------------

   //This is the start of the third diamond
  //-----------------------------------------------------
  center_x = diamond_pos_1[0]+2;
  center_y = diamond_pos_1[1]+3;
  center_z = diamond_pos_1[2]-3;
  sizeofdim = 9.75;
  genDiamond(center_x, center_y, center_z, sizeofdim);
  //End of third diamond
  //-----------------------------------------------------

  //This is the start of the first banner
  //-----------------------------------------------------
  GLint banner_x = banner_pos_1[0];
  GLint banner_y = banner_pos_1[1];
  GLint banner_z = banner_pos_1[2];
  float size_of_banner = 1.0;
  genBanner(banner_x, banner_y, banner_z, size_of_banner);
  //End of first banner
  //-----------------------------------------------------

  //This is the start of the second banner
  //-----------------------------------------------------
  banner_x = banner_pos_1[0] - 9;
  banner_y = banner_pos_1[1] + 9;
  banner_z = banner_pos_1[2] - 8;
  size_of_banner = 1.0;
  genBanner(banner_x, banner_y, banner_z, size_of_banner);
  //End of second banner
  //-----------------------------------------------------

  // Flush the gl stack and matrix
  glFlush();

}

int main(int argc, char *argv[]) 
{
  // Get random seed going
  srand((unsigned)time(NULL));

  //Initialize all of our colors to random values
  outer_petal_color[0] = ((float)rand()/RAND_MAX);
  outer_petal_color[1] = ((float)rand()/RAND_MAX);
  outer_petal_color[2] = ((float)rand()/RAND_MAX);

  outer_petal_color[0] = ((float)rand()/RAND_MAX);
  outer_petal_color[1] = ((float)rand()/RAND_MAX);
  outer_petal_color[2] = ((float)rand()/RAND_MAX);

  inner_circle_color[0] = ((float)rand()/RAND_MAX);
  inner_circle_color[1] = ((float)rand()/RAND_MAX);
  inner_circle_color[2] = ((float)rand()/RAND_MAX);

  arrow_color[0] = ((float)rand()/RAND_MAX);
  arrow_color[1] = ((float)rand()/RAND_MAX);
  arrow_color[2] = ((float)rand()/RAND_MAX);

  diamond_color[0] = ((float)rand()/RAND_MAX);
  diamond_color[1] = ((float)rand()/RAND_MAX);
  diamond_color[2] = ((float)rand()/RAND_MAX);

  /*I took the following GUI intializaiton from the professors
   examples. I believe it was from ex6
   */
   // Next we want to get our display going
  glutInit(&argc, argv);
  // We want to put glut into display going, adding depth for the 3D scene
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE );
  // setting a default size
  glutInitWindowSize(1000,1000);
  glutCreateWindow(argv[0]);

  glEnable(GL_DEPTH_TEST);

  //OpenGL init
  glClearColor(0, 0, 0, 0); //set background to black

  //camera is at (0,0,0) looking at negative y axis
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(100, 1, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  flower_pos_1[2] = -5;
  //callback functions
  glutDisplayFunc(sceneDisplay);
  //Need with the special function
  //glutReshapeFunc(objRedraw);
  // Invoked when keys are pressed, they have an API
  // to interface it
  glutSpecialFunc(lorenzSpecial);
  //Invoked when keyboard buttons are pressed, they have an PI
  // to interface it
  glutKeyboardFunc(lorenzKeyHandler);
  //This idle function allows GLUT to perform background processing
  //tasks , when there are no new events. helps keep up GUI
  glutIdleFunc(lorenzIdle);
  //kick off the continuous loop
  glutMainLoop();
  return 0;
}


