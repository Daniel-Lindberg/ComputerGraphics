/*
Homework 3: Texture and Lighting
Author: Daniel Lindberg
Description: Generates a table in a room, with lighting effects
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>

#define BASE_HEIGHT  0.25
#define BASE_WIDE    0.05

//The default values for the camera
GLfloat x_camera = 0.0;
GLfloat y_camera = 1.0;
GLfloat z_camera = 3.0;

//The default values for the viewpoint
GLfloat x_viewpoint = 0.0;
GLfloat y_viewpoint = 1.0;
GLfloat z_viewpoint = 0.0;

unsigned int textures[3];

float pi = 3.14159;

/*
GLubyte floor_tile[64][64][4];
GLubyte ceiling_tile[64][64][4];
GLubyte sandlewood[64][64][4];
GLubyte lamp_color[64][64][4];
*/

GLubyte floor_tile[500][500][4];
GLubyte ceiling_tile[1000][1000][4];
GLubyte sandlewood[500][500][4];
GLubyte lamp_color[64][64][4];


// set up the parameters for lighting 
GLfloat light_ambient[] = {0,0,0,1}; 
GLfloat light_diffuse[] = {.6,.6,.6,1};
GLfloat light_specular[] = {1,1,1,1}; 
GLfloat light_pos[] = {0.3,0,0,0}; 

/*
Generate the textures for:
Floor Tiles
Ceiling Tiles
SandleWood
Lamp Color
*/
/*
void genFloorTile()
{
  for (int i=0; i<64; i++)
  {
    for (int j=0; j<64; j++) 
    {
      // Give it a striped pattern
      //Half of the vertical tiles are black
      //Half are sandlewood
       if ((i % 2 == 0) || (j % 2 == 0))
       {
         floor_tile[i][j][0] = (GLubyte) 151; //Red
         floor_tile[i][j][1] = (GLubyte) 149; //Green
         floor_tile[i][j][2] = (GLubyte) 134;  //Blue
         floor_tile[i][j][3] = (GLubyte) 255;
       } 
       else
       {
         floor_tile[i][j][0] = (GLubyte) 170; //red
         floor_tile[i][j][1] = (GLubyte) 170; //green
         floor_tile[i][j][2] = (GLubyte) 163; // blue
         floor_tile[i][j][3] = (GLubyte) 255;
       }
    }
  };
}
void genCeilingTile()
{
  for (int i=0; i<64; i++)
  {
    for (int j=0; j<64; j++) 
    {
      // Give it a sandlewood finish
       ceiling_tile[i][j][0] = (GLubyte) 49;
       ceiling_tile[i][j][1] = (GLubyte) 49;
       ceiling_tile[i][j][2] = (GLubyte) 49;
       ceiling_tile[i][j][3] = (GLubyte) 255;
    }
  };
}
void genSandlewood()
{
  for (int i=0; i<64; i++)
  {
    for (int j=0; j<64; j++) 
    {
      // Give it a sandlewood finish
       sandlewood[i][j][0] = (GLubyte) 139;
       sandlewood[i][j][1] = (GLubyte) 69;
       sandlewood[i][j][2] = (GLubyte) 19;
       sandlewood[i][j][3] = (GLubyte) 255;
    }
  };
}*/

unsigned char* readBMP(char* filename)
{
    int resultant;
    int z;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    resultant = fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

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
      // Give it a striped pattern
      //Half of the vertical tiles are black
      //Half are sandlewood
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
       // Give it a sandlewood finish
       ceiling_tile[i][j][0] = (GLubyte) ceiling_data[3* (i * 1000 + j)];
       ceiling_tile[i][j][1] = (GLubyte) ceiling_data[3* (i * 1000 + j)+1];
       ceiling_tile[i][j][2] = (GLubyte) ceiling_data[3* (i * 1000 + j)+2];
       ceiling_tile[i][j][3] = (GLubyte) 255;
    }
  };
}
void genSandlewood(unsigned char * table_data)
{
  for (int i=0; i<500; i++)
  {
    for (int j=0; j<500; j++) 
    {
       // Give it a sandlewood finish
       sandlewood[i][j][0] = (GLubyte) table_data[3* (i * 500 + j)];
       sandlewood[i][j][1] = (GLubyte) table_data[3* (i * 500 + j)+1];
       sandlewood[i][j][2] = (GLubyte) table_data[3* (i * 500 + j)+2];
       sandlewood[i][j][3] = (GLubyte) 255;
    }
  };
}


void genLampColor()
{
  for (int i=0; i<64; i++)
  {
    for (int j=0; j<64; j++) 
    {
      // Give it a sandlewood finish
       lamp_color[i][j][0] = (GLubyte) 166;
       lamp_color[i][j][1] = (GLubyte) 137;
       lamp_color[i][j][2] = (GLubyte) 61;
       lamp_color[i][j][3] = (GLubyte) 255;
    }
  };
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
      glColor3f(1.0f, 0.0f, 0.0f);
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

void lorenzIdle(void)
{
  /* Have to have an idle function, so that GLUT can
  perform background tasks, or continuous animation.
  Will resceneDisplay 
  */
  glutPostRedisplay();
}

//Generate the x,y,z planes
void genAxes()
{
  glLineWidth(2.0); 
  glBegin(GL_LINES); 
  glColor3f(1,0,0);  
  glVertex3f(0,0,0); 
  glVertex3f(.15,0,0); 
  glColor3f(0,1,0);  
  glVertex3f(0,0,0); 
  glVertex3f(0,.15,0); 
  glColor3f(0,0,1); 
  glVertex3f(0,0,0); 
  glVertex3f(0,0,.15); 
  glEnd(); 
  glLineWidth(1.0); 
}

void genFloor()
{
  //Has a lot of logic to genrate a Floor
  // Essentially draw a square
  glPushMatrix(); 
  glScalef(4, 0.01, 4); 
  glBegin(GL_QUADS); 
  // Use the normalize 3f to create a normalized pattern
  glNormal3f(-1,0,1); 
  glTexCoord2f(0,0); 
  glVertex3f(-0.5, 0, 0.5);
  glNormal3f(1,0,1); 
  glTexCoord2f(0,1); 
  glVertex3f(0.5, 0, 0.5);
  glNormal3f(1,0,-1);
  glTexCoord2d(1,1); 
  glVertex3f(0.5, 0 , -0.5);
  glNormal3f(-1, 0, -1);
  glTexCoord2d(1,0); 
  glVertex3f(-0.5, 0, -0.5);
  glEnd(); 
  glPopMatrix(); 
}
void genCeiling()
{
  //Has a lot of logic to genrate a ceiling
  // Essentially draw a square
  glPushMatrix(); 
  glScalef(4, 0.01, 4); 
  glBegin(GL_QUADS); 
  // Use the normalize 3f to create a normalized pattern
  glNormal3f(-10,0,10); 
  glTexCoord2f(0,0); 
  glVertex3f(-0.5, 0, 0.25);
  glNormal3f(10,0,10); 
  glTexCoord2f(0,10); 
  glVertex3f(0.5, 0, 0.25);
  glNormal3f(10,0,-10);
  glTexCoord2d(10,10); 
  glVertex3f(0.5, 0 , -0.75);
  glNormal3f(-10, 0, -10);
  glTexCoord2d(10,0); 
  glVertex3f(-0.5, 0, -0.75);
  glEnd(); 
  glPopMatrix(); 
}

// Generate the wall
void genWall()
{
  // Generate a rectangle , create a cube
  glColor3f(0.6823f, 0.6039f, 0.4666f); 
  glPushMatrix(); 
  glScalef(3, 0.01, 4); 
  glutSolidCube(1.0);
  glPopMatrix(); 
}

//Create a table leg
void genTableLeg(float table_x, float table_y, float table_z)
{
  glPushMatrix();
  glTranslatef(table_x, table_y, table_z); 
  glScalef(0.1, 1, 0.1); 
  glutSolidCube(1.0); 
  glPopMatrix(); 
}

//Generate a table 
void genTable()
{
  glColor4f(.5,.5, .5,1); 

  glPushMatrix(); 
  glScalef(2, 0.1, 2); 
  glutSolidCube(1.0); 
  glPopMatrix(); 

  //Generate 4 table legs
  genTableLeg(0.95,-0.5,-0.95); 
  genTableLeg(0.95,-0.5,0.95); 
  genTableLeg(-0.95,-0.5,-0.95); 
  genTableLeg(-0.95,-0.5,0.95);

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
  glutSolidCone(0.3, 0.1, 10, 10); 
  glPopMatrix(); 
}

// Generate a lamp 
void genLamp()
{
 glColor3f(1,0,0); 
 genBase(); 
 glColor3f(1,1,0); 
 glTranslatef(0,  0, BASE_HEIGHT*2); 
 genArm(); 
 glTranslatef(0,0,BASE_HEIGHT); 
 genTop();
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
      y_viewpoint += 1.0;
      break;
    case GLUT_KEY_DOWN:
      y_viewpoint -= 1.0;
      break;
    case GLUT_KEY_LEFT:
      // Move the X viewpoint
      x_viewpoint += 1.0;
      break;
    case GLUT_KEY_RIGHT:
      x_viewpoint -= 1.0;
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
  if (key == 'q')
  {
    //q and a will move x_camera up and down
    x_camera += 1.0;
  }
  else if (key == 'a')
  {
    x_camera -= 1.0;
  }
  else if (key == 'w')
  {
    //w and s move y_camera up and down
    y_camera += 1.0;
  }
  else if (key == 's')
  {
    y_camera -= 1.0;
  }
  else if (key == 'e')
  {
    //e and d move the z_camera up and down 
    z_camera += 1.0;
  }
  else if (key == 'd')
  {
    z_camera -= 1.0;
  }
  else if (key == 'y')
  {
    light_pos[0] += 0.3;
  }
  else if (key == 'h')
  {
    light_pos[0] -= 0.3;
  }
  else if (key == 'u')
  {
    light_pos[1] += 0.3;
  }
  else if (key == 'j')
  {
    light_pos[1] -= 0.3;
  }
  else if (key == 'i')
  {
    light_pos[2] += 0.3;
  }
  else if (key == 'k')
  {
    light_pos[2] -= 0.3;
  }
  glutPostRedisplay();
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
  
  glEnable(GL_COLOR_MATERIAL); 
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 

  // Show the projection matrix 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  gluPerspective(60, 1, .1, 100); 

  // Show the model matrix
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 

  // position the camera , and move it about
  gluLookAt(x_camera, y_camera, z_camera,  // GLDouble eye x,y,z
            x_viewpoint, y_viewpoint, -3,  // GLDouble reference x,y,z
            0, 1, 0); // GLDouble up x,y,z
  //x_camera = 0; y_camera = 0; z_camera = 0; x_viewpoint = 0; y_viewpoint = 0;
 
  float scale = 1.0;
  glScalef(scale, scale, scale); 
  //glPopMatrix();

  // First draw the axes

  genAxes();  

  GLfloat mat_specular[] = {.7, .7, .7,1}; 
  GLfloat mat_shine[] = {60}; 

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine); 
 
 //Generate the floor
  glEnable(GL_TEXTURE_2D);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      floor_tile);
  

  genFloor();                 // the floow is a 4x0.01x4 cube 
  

  glDisable(GL_TEXTURE_2D); 
  

  /* 
  Create the walls in the room
  Translate each wall. Rotate each wall
  Generate each wall
  */
  glPushMatrix(); 
  glTranslatef(-2,1.5,0); 
  glRotatef(90, 0, 0, 1); 
  genWall(); 

  glPushMatrix(); 
  glTranslatef(0,1.5,-2); 
  glRotatef(90, 1, 0, 0);
  glRotatef(90, 0, 1, 0); 
  genWall(); 

  glPushMatrix(); 
  glTranslatef(2,1.5,0); 
  glRotatef(90, 0, 0, 0);
  glRotatef(90, 0, 0, 0);
  glRotatef(90, 0, 0, 1); 
  genWall(); 

  glEnable(GL_TEXTURE_2D);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1000, 
            1000,0, GL_RGBA, GL_UNSIGNED_BYTE, 
            ceiling_tile);
  //Create the ceiling  
  //Rotate the ceiling
  glRotatef(90, 0, 0, -1);
  glRotatef(90, 0, 0, -1);
  // Move the ceiling about
  glTranslatef(0, -3, 1); 
  // Apply the texture of the ceiling
  
  
  // Generate the ceiling
  genCeiling();
  glDisable(GL_TEXTURE_2D); 

  //Create the Table
  //First rotate the table
  glRotatef(90, 1 , 0, 0);
  glRotatef(90, 0 , 1, 0);
  glRotatef(90, 0 , 0, 1);
  // Move it about
  glTranslatef(-1, -2, 0);
  // Apply the texture of the table
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 500, 
            500,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      sandlewood);
  
  // Generate the table
  genTable();

  //Create the lamp
  // First rotate the lamp
  glRotatef(90, -1 , 0, 0);
  glRotatef(90, 0 , 0, 0);
  glRotatef(90, 0 , 0, -1);
  // Then move the lamp
  glTranslatef(0, 0, 0);
  //Apply the texture of the lamp
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 
            64,0, GL_RGBA, GL_UNSIGNED_BYTE, 
      lamp_color);
  //Generate the lamp
  genLamp();

  /* Reflect the light position. */


  // set up the parameters for lighting 
  //light_pos[] = {1,0,0,0}; 

  glEnable(GL_LIGHTING); 
  //Rotate the lamp
  glRotatef(90, 0 , 0, 0);
  glRotatef(90, 0 , 0, 0);
  glRotatef(90, 0 , 0, 1);
  //Get the light going
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); 
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); 
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  //Set the light model to be local
  //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 0.5);


  //glutSwapBuffers();

  // Flush the gl stack and matrix
  glFlush();

}

int main(int argc, char **argv) 
{
  // Get random seed going
  srand((unsigned)time(NULL));

  unsigned char *floor_tex = readBMP("resources/marble_finish.bmp");
  unsigned char *ceiling_tex = readBMP("resources/ceiling_finish.bmp");
  unsigned char *table_tex = readBMP("resources/oak_finish.bmp");
  genFloorTile(floor_tex);
  genCeilingTile(ceiling_tex);
  genSandlewood(table_tex);
  genLampColor();

  //textures[0] = LoadTexture("resources/marble_finish.bmp", 500, 500, 1);
  //textures[1] = LoadTexture("resources/oak_finish.bmp", 500, 500, 2);
  //textures[2] = LoadTexture("resources/pebble_finish.bmp", 500, 500, 3);

  /*I took the following GUI intializaiton from the professors
   examples. I believe it was from ex6
   */
   // Next we want to get our display going
  glutInit(&argc, argv);
  // We want to put glut into display going, adding depth for the 3D scene
  glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_SINGLE );
  // setting a default size
  glutInitWindowSize(1000,1000);
  glutCreateWindow("Homework 3 Daniel Lindberg");

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


