/*
Final Project: Flashpaint
Author: Daniel Lindberg
Description: Determine how many paintings have an image displayed
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define BASE_HEIGHT  0.25
#define BASE_WIDE    0.05
#define LEN 8192  //  Maximum length of text string


//The default values for the camera
GLfloat x_camera = 0.0;
GLfloat y_camera = 2.0;
GLfloat z_camera = 4.0;

//The default values for the viewpoint
GLfloat x_viewpoint = 0.0;
GLfloat y_viewpoint = 2.0;
GLfloat z_viewpoint = 0.0;

//Base definition of PI
float pi = 3.14159;

// The variables in regards to the light switching around
int moving_to_right = 1;
float translater = 1.0/CLOCKS_PER_SEC;
float incrementer = 0.35;
float max_angle = 65.0;

//points for the game
int guess_num = 0;
float game_points = 10;
int active_paintings = 0;
int winning_num = 0;
time_t start_time;
time_t current_time;

int flower_sin_multiplier = 1;
int flower_cos_multiplier = 1;


// Texture patterns for the various services
GLubyte floor_tile[500][500][4];
GLubyte ceiling_tile[1000][1000][4];
GLubyte lamp_color[64][64][4];
GLubyte flower_texture[500][500][4];
GLubyte dinosaur_texture[500][500][4];
GLubyte chair_texture[500][500][4];
GLubyte total_black[500][500][4];

// Texture identifiers
GLuint texture0, texture1, texture2, texture3, texture4, texture5, texture6;

// set up the parameters for lighting 
GLfloat light_ambient[] = {0.3,0.3,0.3,1}; 
GLfloat light_diffuse[] = {1,1,1,1};
GLfloat light_specular[] = {0.5,0.5,0.5,1}; 
GLfloat light_pos[] = {0.3,0,-1,1}; 

bool left_side = false, right_side = false, middle_right = false, middle_left = false;

int portrait_values[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//Field of view and projection
int proj=1;       //  Projection type
int fov=60;       // Field of view
double asp=1;     //  Aspect ratio
double dim=1.0;   //  Size of world
/*
Generate the textures for:
Floor Tiles
Ceiling Tiles
SandleWood
Lamp Color
*/

#define GLERROR_CHECK() GLERROR(__FILE__, __LINE__)

#define GLERROR(file, line)  do { \
   int err = glGetError();  \
   if (err != GL_NO_ERROR) \
       printf ("%s:%d => %d GL Error\n", file, line, err); \
  } while (0)
    

void printString(float x, float y, float z, char *display_string)
{
  /*
  Takes in a x,y,z position to raster position the string
  Also takes in a display string of something to be displayed
  */
  glPushMatrix();
  glColor3f( 1.0f, 1.0f, 1.0f );
  // Set the x,y,z position of the string
  glRasterPos3f(x, y, z);
  int len, i;
  //Get the length of the character string
  len = (int)strlen(display_string);
  //Iterate through the character string
  for (i = 0; i < len; i++) {
  	//Display each character in helvetica
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, display_string[i]);
  }
  glPopMatrix();
}

//Reads the bmp files for the textures
unsigned char* readBMP(char* filename)
{
	//Take in a bmp file, but the path to it as a character string
    int resultant;
    int z;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    resultant = fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    //Ready in the bmp file after it finds the default size of each bmp
    int size = 3 * width * height;
    unsigned char* data = malloc(size*sizeof(unsigned char)); // allocate 3 bytes per pixel
    resultant = fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    if (resultant == -1)
    {
      printf("reading the source caused an issue\n");
    }
    fclose(f);

    for( z= 0; z < size; z += 3)
    {
            unsigned char tmp = data[z];
            data[z] = data[z+2];
            data[z+2] = tmp;
    }

    return data;
}
void genFloorTile(unsigned char * floor_data)
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
      //Reads in the floor_data tile given in resources
      floor_tile[i][j][0] = (GLubyte) floor_data[3* (i * 500 + j)]; //Red
      floor_tile[i][j][1] = (GLubyte) floor_data[3* (i * 500 + j) + 1]; //Green
      floor_tile[i][j][2] = (GLubyte) floor_data[3* (i * 500 + j) + 2];  //Blue
      floor_tile[i][j][3] = (GLubyte) 255;
    }
  };
}
void genCeilingTile(unsigned char * ceiling_data)
{
  for (int i=0; i<1000; i++)
  {
    for (int j=0; j<1000; j++) 
    {
       //Reads in the ceiling data tile given in resources
       ceiling_tile[i][j][0] = (GLubyte) ceiling_data[3* (i * 1000 + j)];
       ceiling_tile[i][j][1] = (GLubyte) ceiling_data[3* (i * 1000 + j)+1];
       ceiling_tile[i][j][2] = (GLubyte) ceiling_data[3* (i * 1000 + j)+2];
       ceiling_tile[i][j][3] = (GLubyte) 255;
    }
  };
}

void genDinosaurTexture(unsigned char * dinosaur_data)
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
      // Give it a dinosaur portrait texture
       dinosaur_texture[i][j][0] = (GLubyte) dinosaur_data[3* (i * 500 + j)];
       dinosaur_texture[i][j][1] = (GLubyte) dinosaur_data[3* (i * 500 + j)+1];
       dinosaur_texture[i][j][2] = (GLubyte) dinosaur_data[3* (i * 500 + j)+2];
       dinosaur_texture[i][j][3] = (GLubyte) 255;
    }
  };
}
void genChairTexture(unsigned char * chair_data)
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
      // Give it a chair portrait texture
       chair_texture[i][j][0] = (GLubyte) chair_data[3* (i * 500 + j)];
       chair_texture[i][j][1] = (GLubyte) chair_data[3* (i * 500 + j)+1];
       chair_texture[i][j][2] = (GLubyte) chair_data[3* (i * 500 + j)+2];
       chair_texture[i][j][3] = (GLubyte) 255;
    }
  };
}
void genFlowerTexture(unsigned char * flower_data)
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
      // Give it a flower portrait texture
       flower_texture[i][j][0] = (GLubyte) flower_data[3* (i * 500 + j)];
       flower_texture[i][j][1] = (GLubyte) flower_data[3* (i * 500 + j)+1];
       flower_texture[i][j][2] = (GLubyte) flower_data[3* (i * 500 + j)+2];
       flower_texture[i][j][3] = (GLubyte) 255;
    }
  };
}


void genLampColor()
{
  for (int i=0; i<64; i++)
  {
    for (int j=0; j<64; j++) 
    {
      // Give it a lamp color finish
       lamp_color[i][j][0] = (GLubyte) 166;
       lamp_color[i][j][1] = (GLubyte) 137;
       lamp_color[i][j][2] = (GLubyte) 61;
       lamp_color[i][j][3] = (GLubyte) 255;
    }
  };
}

void genTotalBlackColor()
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
      // Give it a sandlewood finish
       total_black[i][j][0] = (GLubyte) 0;
       total_black[i][j][1] = (GLubyte) 0;
       total_black[i][j][2] = (GLubyte) 0;
       total_black[i][j][3] = (GLubyte) 255;
    }
  };
}

void resetPortraitValues()
{
  // Resets the portrait values to have different textures displayed on them
	// Called at the beginning and end of the program
  active_paintings = 0;
  // Generate the random values for portraits
  for (int i = 0; i < 18; i++)
  {
    portrait_values[i] = rand() & 3;
  }
  winning_num = (rand() % 3)+1;
  //Can't be 0 or 4
  while(winning_num < 0 || winning_num > 3)
  {
  	winning_num = (rand() % 3)+1;
  }
  // Count how many are active
  for (int i = 0; i< 18; i++)
  {
    if (portrait_values[i] == winning_num)
    {
      active_paintings++;
    }
  }
}
void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   //  Orthogonal transformation
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project();
}

void lorenzIdle(void)
{
  /* Have to have an idle function, so that GLUT can
  perform background tasks, or continuous animation.
  Will resceneDisplay 
  */
  glutPostRedisplay();
}

void genFloor()
{
  //Has a lot of logic to genrate a Floor
  // Essentially draw a square
  glPushMatrix(); 
  glScalef(4, 0.01, 4); 
  glBegin(GL_QUADS); 
  // Use the normalize 3f to create a normalized pattern
  glNormal3f(0,1,0); 
  glTexCoord2f(0,0); 
  glVertex3f(-0.5, 0, 0.5);
  //These normals are perpendicular from the direction of the square
  glNormal3f(0,1,0); 
  glTexCoord2f(0,1); 
  glVertex3f(0.5, 0, 0.5);
  glNormal3f(0,1,0);
  glTexCoord2d(1,1); 
  glVertex3f(0.5, 0 , -0.5);
  glNormal3f(0,1,0);
  glTexCoord2d(1,0); 
  glVertex3f(-0.5, 0, -0.5);
  glEnd(); 
  glPopMatrix(); 
}
void genCeiling()
{
  //Has a lot of logic to genrate a Ceiling
  // Essentially draw a square
  glPushMatrix(); 
  glScalef(4, 0.01, 4); 
  glBegin(GL_QUADS); 
  // Use the normalize 3f to create a normalized pattern
  glNormal3f(0,-1,0); 
  glTexCoord2f(0,0); 
  glVertex3f(-0.5, 0, 0.25);
  glNormal3f(0,-1,0); 
  glTexCoord2f(0,10); 
  glVertex3f(0.5, 0, 0.25);
  glNormal3f(0,-1,0);
  glTexCoord2f(10,10); 
  glVertex3f(0.5, 0 , -0.75);
  glNormal3f(0,-1,0);
  glTexCoord2d(10,0); 
  glVertex3f(-0.5, 0, -0.75);
  glEnd(); 
  glPopMatrix(); 
}

// Generate the wall
void genWall()
{
  // Generate a rectangle via creating a cube
  // Color the wall to be generally a lighter brown
  glPushMatrix(); 
  glColor3f(0.6823f, 0.6039f, 0.4666f); 
  glScalef(3, 0.01, 4); 
  glutSolidCube(1.0);
  glPopMatrix(); 
}

// Generate the portrait
void genPortrait(int display_photo, bool left_side)
{
  // Generate the portrait based off of value sent in
  glPushMatrix();
  glScalef(1, 0.01, 1);
  if (display_photo == 0)
  {
  	// 0 Would indicate it is a totally black portrait
  	// 3 is unique identifier for texture
  	glBindTexture(GL_TEXTURE_2D, texture3);
  }
  else if (display_photo == 1)
  {
    // 1 would indicate it is a flower portrait
    // 4 is unique identifier for texture
    glBindTexture(GL_TEXTURE_2D, texture4);
  }
  else if (display_photo == 2)
  {
    // 2 would indicate it is a dinosaur portrait
    // 5 is unique identifier for texture
    glBindTexture(GL_TEXTURE_2D, texture5); 
 
  }
  else if (display_photo == 3)
  {
    // 3 would indicate it is a chair texture
    // 6 is unique identifier for texture
  	glBindTexture(GL_TEXTURE_2D, texture6);
     
  }
  glBegin(GL_QUADS); 
  //On the left side of the room, the texture are rotated
  // Generate a square image and have the normals be perpendicular
  if(left_side)
  {
  	glTexCoord2f(0,0);
  	glNormal3f(0, -1, 0);
  }
  else
  {
  	glTexCoord2f(0,1);
  	glNormal3f(0, -1, 0);
  }
  //Have our normals be perpendicular
  glVertex3f(-0.5, 0, -0.5);
  if(left_side)
  {
  	glTexCoord2f(1,0);
  	glNormal3f(0, -1, 0);
  }
  else
  {
  	glTexCoord2f(0,0);	
  	glNormal3f(0, -1, 0);
  }
  glVertex3f(0.5, 0, -0.5);
  if(left_side)
  {
  	glTexCoord2f(1,1);
  	glNormal3f(0, -1, 0);
  }
  else
  {
  	glTexCoord2f(1,0);
  	glNormal3f(0, -1, 0);
  }
  
  glVertex3f(0.5, 0 , 0.5);
  if(left_side)
  {
  	glTexCoord2f(0,1);
  	glNormal3f(0, -1, 0);
  }
  else
  {
  	glTexCoord2f(1,1);
  	glNormal3f(0, -1, 0);
  }
  glVertex3f(-0.5, 0, 0.5);
  glEnd(); 
  glPopMatrix(); 

}

/*
 *  Function to print any errors encountered
 */
void ErrCheck(char* where)
{
   // I am getting goose egg errors when I run with the current version
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}


// Generate the base of the lamp
void genBase() 
{
  GLUquadricObj *p = gluNewQuadric();  
  glPushMatrix(); 
  glScalef(BASE_WIDE*3,BASE_WIDE*3,BASE_HEIGHT);  
  gluCylinder(p, 1, 1, 1, 30, 30); 
  glPopMatrix(); 
}

// Generate the stem of the lamp
void genArm() 
{
  glPushMatrix(); 
  glScalef(BASE_WIDE*1, BASE_WIDE*1, BASE_HEIGHT*2); 
  glutSolidCube(1.0);
  glPopMatrix(); 
}

// Generate the Top to the lamp
void genTop() 
{
  glPushMatrix();  
  glRotatef(180, 0, 1, 0);
  glutSolidCone(0.3, 0.2, 19, 100); 
  glPopMatrix(); 
}

// Generate a lamp 
void genLamp()
{
  // This is the rotation logic
  /*
  Checks to see if it is rotating to the right
  if it is, it moves to the right. And when it hits the max
  it switches to the left. and vice versa
  */
  if (moving_to_right == 1)
  {
    glRotatef(translater, 1, 0 , 0);
    translater += incrementer;
    if (translater > max_angle)
    {
      game_points -= 0.5;
      moving_to_right = 0;
    }
  }
  else
  {
    glRotatef(translater, 1, 0 , 0);
    translater -= incrementer;
    if (translater < -max_angle)
    {
      game_points -= 0.5;
      moving_to_right = 1;
    }
  }
 genBase(); 
 glTranslatef(0,  0, BASE_HEIGHT*2); 

 // Generate the arm of the lamps
 genArm(); 
 
 glTranslatef(0,0,BASE_HEIGHT); 
 //Generate the top of the lamp
 genTop();

 // set up the parameters for lighting 

 glEnable(GL_LIGHTING); 
 //Rotate the lamp
 //Get the light going, is a spotlight
 glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
 glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

 glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90.0f);
 glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 1.0f);
 GLfloat spotDir[] = {0, 0, 1};
 glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);
 //Set the light model to be local, is a boolean
 glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);
}

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
      glNormal3f(0,0,-z); 
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
    glVertex3f(x, y, z);
    glNormal3f(0,0,-z);
  }
  glEnd();

}


//draws a petal of the flower. the point of petal that meets the circle is at startx, starty. radians is the angle on the center circle at which the petal will be drawn, so it can be oriented in the correct direction. It is constructed from two arcs of 2 very big circle with radius radius.
void genPetal(GLfloat startx, GLfloat starty, GLfloat radians, GLfloat radius) 
{
  glPushMatrix();
  // Give the radian values and confert to degrees
  GLfloat degrees = radians * 180/pi;

  if (degrees < 90 || degrees > 270)
  {
  	flower_cos_multiplier = 1;
  }
  else
  {
  	flower_cos_multiplier = -1;
  }

  glRotatef(degrees, 0.0, 0.0, 1.0);
  glTranslatef(2*radius, 0.0, 0.0);

  //Use the triangle GL
  glBegin(GL_TRIANGLE_FAN);
  
  //Use my random color petals
  glColor3f(0.8f, 0.0f, 0.0f);
  glVertex3f(-radius/3, 0.0, -0.1);
  glNormal3f(0.0, radius/3, -0.1);
  glColor3f(1.0f, 0.0f, 0.0f);

  GLfloat angle;
  for(angle = 0; angle < 2.02*pi; angle += 2*pi/100)
  {
  	//glScalef(1.10, 1.0, 1.0);
    //glNormal3f(flower_cos_multiplier * 2*radius*cos(angle), radius/2 * sin(angle),0);
    glNormal3f(0,0, -1);
    glVertex3f(2*radius * cos(angle), radius/2 * sin(angle), 0);
  }
  glEnd();

  //pop the rotated matrix, so we go back to original
  glPopMatrix();
}

void genDinosaurHead()
{
  //Generate the head of the dinosaur
  glPushMatrix(); 
  glColor4f(0,0.8, 0,1); 
  glRotatef(90, 0, 0, 1);
  glTranslatef(0, 0, 2);
  glScalef(0.2, 0.1, 0.1); 
  glutSolidSphere(1.0, 10, 10);
  glPopMatrix();
}
void genDinosaurBody()
{
  //Generate the body of the dinosaur
  glPushMatrix(); 
  glColor4f(0,0.8, 0,1); 
  glRotatef(90, 0, 0, 1);
  glRotatef(90, 0, 1, 0);
  glTranslatef(-2.3, 0, -0.1);
  glScalef(0.30, 0.1, 0.1); 
  glutSolidSphere(1.0, 10, 10);
  glPopMatrix();

}
void genDinosaurArms(GLfloat x_pos, GLfloat y_pos)
{
  //Generate the arms of the dinosaur
  glPushMatrix(); 
  glColor4f(0,0.8, 0,1); 
  glRotatef(90, 0, 0, 1);	
  glTranslatef(0, x_pos, y_pos);
  glScalef(0.1, 0.05, 0.05); 
  glutSolidSphere(1.0, 10, 10);
  glPopMatrix();
}

void genDinosaurLegs(GLfloat x_pos, GLfloat y_pos)
{
  //Generate the legs of the dinosaur
  glPushMatrix(); 
  glColor4f(0,0.8, 0,1); 
  glRotatef(90, 0, 0, 1);	
  glRotatef(90, 0, 1, 0);
  glTranslatef(y_pos, x_pos, -0.1);
  glScalef(0.1, 0.05, 0.05); 
  glutSolidSphere(1.0, 10, 10);
  glPopMatrix();
}

void genDinosaurTail()
{
  //Generate the tail of the dinosaur
  glPushMatrix(); 
  glColor4f(0,0.8, 0,1); 
  glRotatef(90, 0, 0, 1);
  glTranslatef(-0.3, 0, 2.4);
  glScalef(0.15, 0.075, 0.075); 
  glutSolidSphere(1.0, 10, 10);
  glPopMatrix();

}

//Generate all of the components of the dinosaur
void genDinosaur()
{
  genDinosaurHead();
  genDinosaurBody();
  genDinosaurArms(-0.1, 2.25);
  genDinosaurArms(0.1, 2.25);
  genDinosaurLegs(-0.5, -2.3);
  genDinosaurLegs(0.5, -2.3);
  genDinosaurTail();
}


//Create a table leg
void genChairLeg(float table_x, float table_y, float table_z)
{
  glPushMatrix();
  glTranslatef(table_x, table_y, table_z); 
  glScalef(0.1, 1, 0.1); 
  glutSolidCube(1.0); 
  glPopMatrix(); 
}

//Generate a table 
void genChair()
{
  glColor4f(.5,.5, .5,1); 

  glPushMatrix(); 
  glScalef(2, 0.1, 1); 
  glutSolidCube(1.0); 
  glPopMatrix(); 

  //Generate 4 table legs
  genChairLeg(0.45,-0.5,-0.45); 
  genChairLeg(0.45,-0.5,0.45); 
  genChairLeg(-0.45,-0.5,-0.45); 
  genChairLeg(-0.45,-0.5,0.45);

  //Generate the backboards
  genChairLeg(-0.55, 0.5, 0);
  genChairLeg(-0.55, 0.5, -0.15);
  genChairLeg(-0.55, 0.5, -0.3);
  genChairLeg(-0.55, 0.5, -0.45);
  genChairLeg(-0.55, 0.5, 0.15);
  genChairLeg(-0.55, 0.5, 0.3);
  genChairLeg(-0.55, 0.5, 0.45);

  glPushMatrix();
  glTranslatef(-0.5, 1, 0);
  glScalef(0.25, 0.1, 1);
  glutSolidCube(1.0);
  glPopMatrix();


}

static void lorenzSpecial(int key_button, int x, int y)
{
  /*
  Lorenz special helps move around the viewpoint
  location of the graphical sceneDisplay

  We only use the x and y, but if not included
  produces a warning
  */
  //Basic switch statements for the key buttons
  switch (key_button) 
  {
    //Utilizes the GLUT key API
    case GLUT_KEY_UP:
      //Move Y viewpoint
      guess_num += 1.0;
      break;
    case GLUT_KEY_DOWN:
      guess_num -= 1.0;
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
  x_camera, y_camera, z_camera
  Value     |   Up   | Down
  --------------------------------
  x_camera  |   q    |    a
  y_camera  |   w    |    s
  z_camera  |   e    |    d

  light_pos[0,1,2]
  Value         |   Up   | Down
  --------------------------------
  light_pos[0]  |   u    |    j
  light_pos[1]  |   i    |    k
  light_pos[2]  |   o    |    l
  */
  //Basic if statements for the keyboard buttons
  //Utilizes the GLUT key API
  if (key == 'w')
  {
    y_viewpoint += 1.0;
  }
  else if (key == 's')
  {
    y_viewpoint -= 1.0;
  }
  else if (key == 'a')
  {
    //a and dmove the x viewpoint
    x_viewpoint -= 1.0;
  }
  else if (key == 'd')
  {
    x_viewpoint += 1.0;
  }
  else if (key == 13)
  {
    //This is the enter key
    if (guess_num == active_paintings)
    {
      game_points += 3;
    }
    else
    {
      game_points -= 3;
    }
    resetPortraitValues();
  }
  
  glutPostRedisplay();
}


void loadTextures()
{
	//First load all of the textures from the resources folder
  unsigned char *floor_tex = readBMP("resources/marble_finish.bmp");
  unsigned char *ceiling_tex = readBMP("resources/ceiling_finish.bmp");
  unsigned char *dinosaur_tex = readBMP("resources/Dinosaur.bmp");
  unsigned char *chair_tex = readBMP("resources/Chair.bmp");
  unsigned char *flower_tex = readBMP("resources/Flower.bmp");
  genFloorTile(floor_tex);
  genCeilingTile(ceiling_tex);
  genFlowerTexture(flower_tex);
  genChairTexture(chair_tex);
  genDinosaurTexture(dinosaur_tex);
  genLampColor();
  genTotalBlackColor();
  resetPortraitValues();
  //Load the active textures for each of our textures
  //Bind them, to be referenced later
  glGenTextures(1, &texture0);
  glBindTexture(GL_TEXTURE_2D, texture0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1000, 
            1000,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      ceiling_tile);
  GLERROR_CHECK();

  glGenTextures(1, &texture1);
  glBindTexture(GL_TEXTURE_2D, texture1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      floor_tile);

  glGenTextures(1, &texture2);
  glBindTexture(GL_TEXTURE_2D, texture2);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 
	             64,0, GL_RGBA, GL_UNSIGNED_BYTE, 
	             lamp_color);
  GLERROR_CHECK();

  glGenTextures(1, &texture3);
  glBindTexture(GL_TEXTURE_2D, texture3);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      total_black);
  GLERROR_CHECK();

  glGenTextures(1, &texture4);
  glBindTexture(GL_TEXTURE_2D, texture4);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      flower_texture);
  GLERROR_CHECK();

  glGenTextures(1, &texture5);
  glBindTexture(GL_TEXTURE_2D, texture5);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      dinosaur_texture);
  GLERROR_CHECK();

  glGenTextures(1, &texture6);
  glBindTexture(GL_TEXTURE_2D, texture6);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      chair_texture);
  GLERROR_CHECK();
}


void sceneDisplay(void) 
{
  /*
  Display a scene of a room with a table and a lamp
  */
  glClearColor(0,0,0,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);
  glEnable(GL_LIGHT0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  //This is the perspective 
  Project();
  // Show the model matrix
  glMatrixMode(GL_MODELVIEW); 
  // Show the projection matrix 
  glLoadIdentity(); 

  // position the camera , and move it about
  gluLookAt(x_camera, y_camera, z_camera,  // GLDouble eye x,y,z
            x_viewpoint, y_viewpoint, -3,  // GLDouble reference x,y,z
            0, 1, 0); // GLDouble up x,y,z

  float scale = 1.0;
  glScalef(scale, scale, scale);

  GLfloat mat_specular[] = {.7, .7, .7, 1}; 
  GLfloat mat_shine[] = {60}; 

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine); 
 
  //Generate the floor
  glDisable(GL_COLOR_MATERIAL);
  
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texture1);
  glEnable(GL_TEXTURE_2D);
  genFloor(); // the floor is a 4x0.01x4 cube 
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  
  //Create the walls in the room
  //Translate each wall. Rotate each wall
  //Generate each wall
  glEnable(GL_COLOR_MATERIAL);
  
  glPushMatrix(); 
  glTranslatef(-2,1.5,0); 
  glRotatef(90, 0, 0, 1); 
  genWall(); 
  glPopMatrix();

  glPushMatrix(); 
  glTranslatef(0,1.5,-2); 
  glRotatef(90, 1, 0, 0);
  glRotatef(90, 0, 1, 0); 
  genWall(); 
  glPopMatrix();

  glPushMatrix(); 
  glTranslatef(2,1.5,0); 
  glRotatef(90, 0, 0, 1); 
  genWall(); 
  glPopMatrix();
  glDisable(GL_COLOR_MATERIAL);

  //Create the ceiling  
  //Rotate the ceiling
  glRotatef(90, 0, 0, -1);
  glRotatef(90, 0, 0, -1);
  // Move the ceiling about
  glTranslatef(0, -3, 1); 
  // Apply the texture of the ceiling

  
  // Generate the ceiling
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texture0);   
  
  glEnable(GL_TEXTURE_2D);
  genCeiling();
  glDisable(GL_TEXTURE_2D); 
  glPopMatrix();
  
  
  // ---------------------------------------------
  // Doing the left wall portraits

  glRotatef(90, 1, 0, 0);
  glRotatef(90, 0, 0, 1);
  glTranslatef(-1, -1.9, -2);


  // Portrait 0 
  //Do the checks for the left, middle_left, middle_right and right
  if (translater < -max_angle + 20)
  {
    left_side = true;
    middle_left = false;
    middle_right = false;
    right_side = false;
  }
  else if((translater > -max_angle + 20) && (translater < 0))
  {
    left_side = false;
    middle_left = true;
    middle_right = false;
    right_side = false;
  }
  else if((translater > 0) && (translater < max_angle - 20))
  {
    left_side = false;
    middle_left = false;
    middle_right = true;
    right_side = false;
  }
  else 
  {
    left_side = false;
    middle_left = false;
    middle_right = false;
    right_side = true;
  }
  //Enable for the portraits
  glEnable(GL_TEXTURE_2D);
  
  if( left_side == true)
  {
	genPortrait(portrait_values[0], true);
	glTranslatef(-1.1, 0, 0);
	genPortrait(portrait_values[1], true);
	glTranslatef(0, 0, 1.1);
	genPortrait(portrait_values[2], true);
	glTranslatef(1.1, 0, 0);
	genPortrait(portrait_values[3], true);
	glTranslatef(1.1, 0, 0);
	genPortrait(portrait_values[4], true);
	glTranslatef(0,0,-1.1);
	genPortrait(portrait_values[5], true);
  }
  else
  {
	genPortrait(0, false);
 	glTranslatef(-1.1, 0, 0);
    genPortrait(0, true);
 	glTranslatef(0, 0, 1.1);
    genPortrait(0, true);
    glTranslatef(1.1, 0, 0);
    genPortrait(0, true);
    glTranslatef(1.1, 0, 0);
    genPortrait(0, true);
    glTranslatef(0,0,-1.1);
    genPortrait(0, true);
  }
  

  // Finish of the left wall portraits
  // ---------------------------------------------
  //Start of the back wall portraits
  
  glRotatef(90, 0, 0, 1);
  glRotatef(90, 0, 1, 0);
  glTranslatef(-1.1, 2.9, 0.6);
  
 

  if(middle_left == true)
  {
  	genPortrait(portrait_values[6], false);
	glTranslatef(1.1, 0, 0);
	genPortrait(portrait_values[7], false);
	glTranslatef(0, 0, 1.3);
  }
  else
  {
  	genPortrait(0, false);
    glTranslatef(1.1, 0, 0);
    genPortrait(0, false);
    glTranslatef(0, 0, 1.3);
  }
  if(middle_left == true || middle_right == true)
  {
  	genPortrait(portrait_values[8], false);
	glTranslatef(-1.1, 0, 0);
	genPortrait(portrait_values[9], false);
	glTranslatef(0, 0, 1.3);
  }
  else
  {
  	genPortrait(0, false);
	glTranslatef(-1.1, 0, 0);
	genPortrait(0, false);
	glTranslatef(0, 0, 1.3);
  }

  if(middle_right == true)
  {
  	genPortrait(portrait_values[10], false);
    glTranslatef(1.1, 0, 0);
    genPortrait(portrait_values[11], false);
  }
  else
  {
  	genPortrait(0, false);
    glTranslatef(1.1, 0, 0);
    genPortrait(0, false);
  }


  // Finish of the back wall portraits
  // ---------------------------------------------
  //Start of the right wall portraits

  glRotatef(90, 1, 0, 0);
  
  glTranslatef(0, 0.6, 0.7);

  if(right_side == true)
  {
  	genPortrait(portrait_values[12], false);
    glTranslatef(-1.1, 0, 0.0);
    genPortrait(portrait_values[13], false);
    glTranslatef(0, 0, 1.1);
    genPortrait(portrait_values[14], false);
    glTranslatef(1.1, 0, 0);
    genPortrait(portrait_values[15], false);
    glTranslatef(0, 0, 1.1);
    genPortrait(portrait_values[16], false);
    glTranslatef(-1.1, 0, 0);
    genPortrait(portrait_values[17], false);

  }
  else
  {
	genPortrait(0, false);
    glTranslatef(-1.1, 0, 0.0);
    genPortrait(0, false);
    glTranslatef(0, 0, 1.1);
    genPortrait(0, false);
    glTranslatef(1.1, 0, 0);
    genPortrait(0, false);
    glTranslatef(0, 0, 1.1);
    genPortrait(0, false);
    glTranslatef(-1.1, 0, 0);
    genPortrait(0, false);
  }
  // Finish of the right wall portraits
  // ---------------------------------------------
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  //Create the lamp
  // First rotate the lamp
  glRotatef(90, -2 , 0, 0);
  glRotatef(270, 0 , -4, 0);
  glRotatef(90, 0 , 0, -2);

  //Display the time
  current_time = (clock() - start_time)/(CLOCKS_PER_SEC);

  //Create the string output for the game
  char result[256];
  sprintf(result, "Score=%ld Timer=%.2f Guess=%d", current_time, game_points, guess_num);
  printString(0,2.3,1, result);


  if(winning_num == 1)
  {
  	glPushMatrix();
  	//Generate a flower
    glRotatef(90, 0, 1, 0);
    glTranslatef(-1.2, 1.9, 0.5);
    glScalef(0.1, 0.1, 0.1); 
    GLfloat radius = 0.5;
    glColor3f(0.0f, 0.0f, 0.0f);
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
      // Generate a petal for everyoneprintf()
      genPetal(x, y, angle, radius);
    }
  	//This is the flower case
  	glColor3f(1.0f, 1.0f, 1.0f);
	//draw center circle
	genCenter(radius, 0, 0);
	//end of gen flower ---------------------

	glScalef(10.0, 10.0, 10.0);
	glTranslatef(1.2, -1.9, -0.5);

	glRotatef(-90, 0, 1, 0);
	// Then move the lamp
	glTranslatef(-0.5, 1.9, -0.8);
	glPopMatrix();
  }
  else if (winning_num == 2)
  {
  	glPushMatrix();
  	glTranslatef(-0.5,1.9,-0.8);
  	genDinosaur();
  	glPopMatrix();

  }
  else if(winning_num == 3)
  {
  	glPushMatrix();
  	glRotatef(90, -1, 0, 0);
  	glTranslatef(0, -1.3, 1.85);
  	glScalef(0.2, 0.2, 0.2);
  	genChair();
  	glScalef(5, 5, 5);
  	glRotatef(90, 1, 0, 0);
  	glPopMatrix();
  	
  }
  glDisable(GL_COLOR_MATERIAL);
  glPushMatrix();
  
  glTranslatef(-0.5,1.9,-0.8);
  //Apply the texture of the lamp
  glBindTexture(GL_TEXTURE_2D, texture2);
  glEnable(GL_TEXTURE_2D);
  //Generate the lamp
  genLamp();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();

  ErrCheck("sceneDisplay");
  
  /* Reflect the light position. */

  // Flush the gl stack and matrix
  glFlush();

  if (game_points <= 0.0)
  {
  	printf("The timer has gone to 0, you lose. Your score is: %ld \n", current_time);
  	exit(0);
  }

}


int main(int argc, char **argv) 
{
  // Get random seed going
  srand((unsigned)time(NULL));

  // Check for the hard option to be specified
  for (int i = 0; i < argc; ++i) 
  {
  	if(strcmp(argv[i],"hard") == 0)
  	{
  		translater += 0.5/CLOCKS_PER_SEC;
  	}
  }
  // Get the start time of the clock
  start_time = clock();


  /*I took the following GUI intializaiton from the professors
   examples. I believe it was from ex6
   */

  // Next we want to get our display going
  glutInit(&argc, argv);


  // We want to put glut into display going, adding depth for the 3D scene
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE );

  // setting a default size
  glutInitWindowSize(1000,1000);
  glutCreateWindow("FinalProj Daniel Lindberg");
  loadTextures();
  //callback functions
  glutDisplayFunc(sceneDisplay);
  //Need with the special function
  glutReshapeFunc(reshape);
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


