#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996)
#include "freeglut_std"
#include "freeglut_ext.h"
#include "freeglut.h"
#endif


#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>


#include "loadobjfile.cpp"
float Time;
#include "sphere.cpp"
#include "setlight.cpp"



// title of these windows:

const char *WINDOWTITLE = {"Sample 2015"};


// what the glui package defines as true and false:

const int GLUITRUE = {true};
const int GLUIFALSE = {false};
const float BOXSIZE = { 2.f };
// the escape key:

#define ESCAPE 0x1b

// initial window size:

const int INIT_WINDOW_SIZE = {600};

// size of the box:



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = {1.};
const float SCLFACT = {0.005f};

// minimum allowable scale factor:

const float MINSCALE = {0.05f};

// active mouse buttons (or them together):

const int LEFT = {4};
const int MIDDLE = {2};
const int RIGHT = {1};

// which projection:

enum Projections
{
    ORTHO,
    PERSP
};

// which button:

enum ButtonVals
{
    RESET,
    QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[] = {0., 0., 0., 1.};

// line width for the axes:

const GLfloat AXES_WIDTH = {3.};

// the color numbers:
// this order must match the radio button order

enum Colors
{
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    MAGENTA,
    WHITE,
    BLACK
};

char *ColorNames[] =
{
    "Red",
    "Yellow",
    "Green",
    "Cyan",
    "Blue",
    "Magenta",
    "White",
    "Black"};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[][3] =
{
    {1., 0., 0.}, // red
    {1., 1., 0.}, // yellow
    {0., 1., 0.}, // green
    {0., 1., 1.}, // cyan
    {0., 0., 1.}, // blue
    {1., 0., 1.}, // magenta
    {1., 1., 1.}, // white
    {0., 0., 0.}, // black
};

// fog parameters:

const GLfloat FOGCOLOR[4] = {.0, .0, .0, 1.};
const GLenum FOGMODE = {GL_LINEAR};
const GLfloat FOGDENSITY = {0.30f};
const GLfloat FOGSTART = {1.5};
const GLfloat FOGEND = {4.};

// non-constant global variables:
void house_color_menu(int id);
int ActiveButton;    // current button that is down
GLuint AxesList;     // list to hold the axes
int AxesOn;          // != 0 means to draw the axes
int DebugOn;         // != 0 means to print debugging info
int DepthCueOn;      // != 0 means to use intensity depth cueing
int DepthBufferOn;   // != 0 means to use the z-buffer
int DepthFightingOn; // != 0 means to use the z-buffer
GLuint BoxList;      // object display list
int MainWindow;      // window id for main graphics window
float Scale;         // scaling factor
int WhichColor;      // index into Colors[ ]
int WhichProjection; // ORTHO or PERSP
int        WhichSphere;
int Xmouse, Ymouse;  // mouse values
float Xrot, Yrot;    // rotation angles in degrees

bool Frozen;
int width = 1024;
int height = 512;
bool isTexture = true;
int idTexture = 0;
bool LIGHT0On = false;
bool LIGHT1On = false;
bool LIGHT2On = false;
bool LIGHT3On = false;
bool LIGHT4On = false;
bool LIGHTING = false;
int translate;

unsigned char *Texture1 = BmpToTexture("StoneWall1.bmp", &width, &height);
unsigned char *Texture2 = BmpToTexture("worldtex.bmp", &width, &height);
unsigned char *Texture5 = BmpToTexture("worldtex.bmp", &width, &height);
unsigned char *Texture3 = BmpToTexture("worldtex.bmp", &width, &height);
unsigned char *Texture = Texture1;
unsigned char *Textur = Texture2;


#define MS_PER_CYCLE 20000
// function prototypes:
void draw_mountain();
void draw_house();
void draw_building();
void Animate();
void Display();
void DoAxesMenu(int);
void DoColorMenu(int);
void DoDepthBufferMenu(int);
void DoDepthFightingMenu(int);
void DoDepthMenu(int);
void DoDistort(int);
void DoTexture(int);
void DoDebugMenu(int);
void DoMainMenu(int);
void DoProjectMenu(int);
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Resize(int, int);
void Visibility(int);
void draw_sky();
void polygon(int a,int b,int c,int d);
void Axes(float);
void HsvRgb(float[3], float[3]);
void draw_helipad();
// main program:
void light (void);
int main(int argc, char *argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)
    
    glutInit(&argc, argv);
    
    // setup all the graphics stuff:
    
    InitGraphics();
    
    // create the display structures that will not change:
    
    InitLists();
    
    // init all the global variables used by Display( ):
    // this will also post a redisplay
    
    Reset();
    
    // setup all the user interface stuff:
    
    InitMenus();
    
    // draw the scene once and wait for some interaction:
    // (this will never return)
    
    glutSetWindow(MainWindow);
    glutMainLoop();
    
    // this is here to make the compiler happy:
    
    return 0;
}
GLfloat vertices[8][3]={{-500.0,-50,-500.0},{500.0,-50,-500.0},{500.0,500.0,-500.0},{-500.0,500.0,-500.0},
    {-500.0,-50,500.0},{500.0,-50,500.0},{500.0,500.0,500.0},{-500.0,500.0,500.0}};

//color of cube for sky
GLdouble colors[8][4]={{0.8862745098039216,1.0,1.0,0.5},{0.8862745098039216,1.0,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},
    {0.8862745098039216,1.0,1.0,0.5},{0.8862745098039216,1.0,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5}};
// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
GLint sub_menu[5];
GLfloat global_ambient[]={0.1,0.1,0.1,1.0};;
GLfloat light0_pos[]={2.0,6.0,3.0,0.0};
GLfloat ambient0[]={1.0,1.0,1.0,1.0};
GLfloat diffuse0[]={1.0,1.0,1.0,1.0}; //white
GLfloat specular0[]={1.0,1.0,1.0,1.0};
GLfloat material_specular[]={0.8,0.8,0.8,1.0};//gray

GLfloat baseAmbientLight[]={0.2,0.2,0.2,1.0};//grey

GLfloat baseDiffuseLight[3][4]={{0.5411764705,0.168627450980,0.88627456,1.0}, //violet
    {1.0,0.0,0.0,1.0}, //red
    {1.0,0.82745098039215686274509803921569,0.6078431372549019607843137254902,1.0}};//wood

GLfloat roofAmbientLight[]={0.2,0.2,0.2,1.0};//grey

GLfloat roofDiffuseLight[3][4]={{1.0,0.89411764705882352941176470588235,0.88235294117647058823529411764706,1.0},//rose
    {0.6078431372549019607843137254902,1.0,1.0,1.0},//blue
    {1.0,0.54901960784313725490196078431373,0.0,1.0}};//orange

// this is typically where animation parameters are set
int start=0,stop=0,forward_tilt=0,left_tilt=0,right_tilt=0,help=0,bc=0,hc=0;

//viewer
GLfloat viewer[]={0.0,60.0,-5.0};
//at coordinates
GLfloat at[]={0.0,0.0,0.0};

void draw_helipad()
{
    glPushMatrix();
    
    glDisable(GL_LIGHTING);
    glColor3f(0.125,0.125,0.125);
    
    
    glTranslatef(0.0,-9.0,-1.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    glScalef(1.0,1.0,0.1);
    glutSolidTorus(13.0,13.0,100,100);
    glScalef(1.0,1.0,1.0);
    
    glLineWidth(25.0);
    glColor3f(1.0,1.0,1.0);
    
    glBegin(GL_QUADS);
    //write H
    glVertex3f(-9.5,-1.5,16);
    glVertex3f(-9.5,1.5,16);
    glVertex3f(9.5,1.5,16);
    glVertex3f(9.5,-1.5,16);
    
    glVertex3f(9.5,11.5,16);
    glVertex3f(9.5,-11.5,16);
    glVertex3f(11.5,-11.5,16);
    glVertex3f(11.5,11.5,16);
    
    glVertex3f(-9.5,11.5,16);
    glVertex3f(-9.5,-11.5,16);
    glVertex3f(-11.5,-11.5,16);
    glVertex3f(-11.5,11.5,16);
    glEnd();
    
    glEnable(GL_LIGHTING);
    
    
    glPopMatrix();
    
}
void house_color_menu(int id)
{
    switch(id)
    {
        case 1:bc=7;
            break;
        case 2:bc=6;
            break;
        case 3:bc=9;
            break;
            
    }
    glutPostRedisplay();
}

void polygon(int a,int b,int c,int d)
{
    glPushMatrix();
    glDisable(GL_LIGHTING);
    
    glBegin(GL_POLYGON);
    glColor4dv(colors[a]);
    glVertex3fv(vertices[a]);
    glColor4dv(colors[b]);
    glVertex3fv(vertices[b]);
    glColor4dv(colors[c]);
    glVertex3fv(vertices[c]);
    glColor4dv(colors[d]);
    glVertex3fv(vertices[d]);
    glEnd();
    glEnable(GL_LIGHTING);
    
    
    glPopMatrix();
}
void draw_sky()
{
    //sky is drawn by creating a cube whose interior is of blue gradient
    glPushMatrix();
    polygon(0,3,2,1);
    polygon(2,3,7,6);
    polygon(0,4,7,3);
    polygon(1,2,6,5);
    polygon(4,5,6,7);
    glPopMatrix();
    
}

void draw_building()
{
    
    glPushMatrix();
    float x,z;
    for(z=100;z<=1000;z+=150)
        for(x=-400;x<=400;x+=150)
        {
            
            glPushMatrix();
            
            glTranslatef(x,0.0,z);
            draw_house();
            glTranslatef(-x,0.0,-z);
            
            glTranslatef(-x,0.0,z);
            draw_house();
            glTranslatef(x,0.0,-z);
            
            
            
            glPopMatrix();
        }
    
    glPushMatrix();
    glTranslatef(0.0,0.0,1010.0);
    draw_helipad();
    glTranslatef(0.0,0.0,-1010.0);
    glPopMatrix();
    
    glPopMatrix();
    
}

//int start=0,stop=0,forward_tilt=0,left_tilt=0,right_tilt=0,help=0,bc=0,hc=0;
void draw_house()
{
    
    glPushMatrix();
    
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,baseAmbientLight);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,baseDiffuseLight[bc]);
    glTranslatef(0.0,5.0,0.0);
    glutSolidCube(30.0);
    glTranslatef(0.0,-5.0,0.0);
    
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,roofAmbientLight);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,roofDiffuseLight[bc]);
    glTranslatef(0.0,15.0,0.0);
    glRotatef(-90.0,1.0,0.0,0.0);
    gluCylinder(gluNewQuadric(),25.0,0.0,25.0,32,32);
    glTranslatef(0.0,-15.0,0.0);
    
    glPopMatrix();
    
}

void Animate()
{
    if (!Frozen)
    {
        // put animation stuff in here -- change some global variables
        // for Display( ) to find:
        int ms = glutGet(GLUT_ELAPSED_TIME);
        ms %= MS_PER_CYCLE;
        Time = (float)ms / (float)MS_PER_CYCLE; // [0.,1.)
    }
    // force a call to Display( ) next time it is convenient:
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// draw the complete scene:

void Display()
{
   //  light();
    GLuint tex;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    
    glBindTexture(GL_TEXTURE_2D, tex); // make tex texture current
    // and set its parameters
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
    
    
    GLuint tex0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex0);
    
    glBindTexture(GL_TEXTURE_2D, tex0); // make tex texture current
    // and set its parameters
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    
    glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Textur);
    //glEnable(GL_TEXTURE_CUBE_MAP);
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    
    if (DebugOn != 0)
    {
        fprintf(stderr, "Display\n");
    }
    
    // set which window we want to do the graphics into:
    
    glutSetWindow(MainWindow);
    
    // erase the background:
    
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (DepthBufferOn != 0)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    
    // specify shading to be flat:
    
    glShadeModel(GL_FLAT);
    
    // set the viewport to a square centered in the window:
    
    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy; // minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);
    
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO)
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    else
        gluPerspective(90., 1., 0.1, 1000.);
    
    // place the objects into the scene:
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // set the eye position, look-at position, and up-vector:
    gluLookAt(90, 40, 0, 0., 0., 0., 0., 1., 0.);
    
    // rotate the scene:
    
    glRotatef((GLfloat)Yrot, 0., 1., 0.);
    glRotatef((GLfloat)Xrot, 1., 0., 0.);
    
    // uniformly scale the scene:
    
    if (Scale < MINSCALE)
        Scale = MINSCALE;
    glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
    
    // set the fog parameters:
    
    
    
    
    if (DepthCueOn != 0)
    {
        glFogi(GL_FOG_MODE, FOGMODE);
        glFogfv(GL_FOG_COLOR, FOGCOLOR);
        glFogf(GL_FOG_DENSITY, FOGDENSITY);
        glFogf(GL_FOG_START, FOGSTART);
        glFogf(GL_FOG_END, FOGEND);
        glEnable(GL_FOG);
    }
    else
    {
        glDisable(GL_FOG);
    }
    
    glShadeModel(GL_SMOOTH);
    if (LIGHTING)
        glEnable(GL_LIGHTING);
    else
        glDisable(GL_LIGHTING);
    
    
    glShadeModel(GL_FLAT);
    glColor3f(0, 1, 0);
    glTranslatef(0, 0, 14);
    SetMaterial(0, 1, 0, 1);
    glutSolidTorus(3,
                   10,
                   64, 64);
   // glutSolidTeapot(5);
  //  glutSolid(3,
///0,/Users/safarji/Downloads/kk/worldtex.bmp
   //                6;
    

    glPushMatrix();
    glColor3f(0.5, 0.2, 1);
    glTranslatef(0, 0, -25);
    SetMaterial(0, 1, 0, 2);
    if (LIGHT2On)
    {
        SetSpotLight(GL_LIGHT2, 20, 20, 20, -1, -1, -1, 0.5, 0.2, 1);
    }
    else
    {
        glDisable(GL_LIGHT2);
    }
    
    glPushMatrix();
    glShadeModel(GL_SMOOTH);
  // glColor3f(1, 1, 1);
    glTranslatef(0, -10, 15);
   // SetMaterial(0, 1, 0, 2);
    //glutSolidTeapot(6);
//glutSolidCone(59,50,55,50);
    glTranslatef(0,-50, 0);
   
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex0);
   //MjbSphere(3, 50, 50);
    glScalef(1, 0.1, 1);
   
  glutSolidCube(300);
    
    //glTexCoord2f(100,20);
    glDisable(GL_TEXTURE_2D);
   // glDisable(GL_LIGHTING);
    glPopMatrix();
    
    
    
    // cone
    glPushMatrix();
    glColor3f(0.5, 0, 1);
    glTranslatef(0, 0, -21.5);
    MjbSphere(1, 50, 50);
    glPopMatrix();
    
    // Red Sphere
    glPushMatrix();
    glColor3f(1, 0, 0);
    SetMaterial(0, 1, 0, 2);
    glTranslatef(-12, -12.0, 2.0);
    glRotatef((float)Time * 4000, 1, 0, 0);
    glTranslatef(12, 12.0, 2.0);
    if (LIGHT0On)
    {
        glEnable(GL_LIGHT0);
        SetPointLight(GL_LIGHT0, 3,3, 3, 2, 0, 0);
    }
    else
        glDisable(GL_LIGHT0);
    MjbSphere(1.5, 50, 50);
    glPopMatrix();
    //  red sphere
    glPushMatrix();
    glColor3f(1, 0, 0);
    glTranslatef(-12, -12.0, 2.0);
    glRotatef((float)Time * 4000, 1, 0, 0);
    glTranslatef(12, 12.0, 2.0);
    MjbSphere(0.5, 50, 50);
    glPopMatrix();
    
   glPushMatrix();
    draw_building();
  draw_helipad();
    
   // glPushMatrix();
    
    glEnable(GL_LIGHTING);
    glScalef(100,100,100);
    glColor3f(0.8, 0.2, 1.0);
    
    
    glTranslatef(15.0, 5, 10.0);
    //  glRotated(10, 0.0, 0.0, 0.0);
    glCallList(BoxList);
    glDisable(GL_LIGHTING);
   // glPopMatrix();
    
    
    
      glPopMatrix();
     glPushMatrix();
       glTranslatef(-40, 0, -50);
   draw_house();
    glPopMatrix();
    // Blue Sphere
    glPushMatrix();
    glColor3f(0, 0, 1);
    SetMaterial(0, 1, 0, 2);
    glTranslatef(0, 0, -14.0);
    glTranslatef(10, 10.0, -10.0);
    glRotatef((float)Time * 4000, 1, 0, 0);
    glTranslatef(-10, -10.0, -10.0);
    if (LIGHT1On)
    {
        glEnable(GL_LIGHT1);
        SetPointLight(GL_LIGHT1, 2, 2, 2, 0, 0, 1);
    }
    else
        glDisable(GL_LIGHT1);
    MjbSphere(1.5, 50, 50);
    glPopMatrix();
    // Small  the blu sphere
    glPushMatrix();
    glColor3f(0, 0, 1);
    glTranslatef(0, 0, -14.0);
    glTranslatef(10, 10.0, -10.8);
    glRotatef((float)Time * 4000, 1, 0, 0);
    glTranslatef(-10, -10.0, -10.8);
    MjbSphere(0.5, 50, 50);
    glPopMatrix();
    
    // S texture
    glPushMatrix();
    glShadeModel(GL_SMOOTH);
    
    glColor3f(1, 1, 1);
    glTranslatef(0, -10, 15);
    SetMaterial(0, 1, 0, 2);
    if (isTexture)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
       MjbSphere(1000, 300, 300);
        
       // glTexCoord2f(100,20);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        // the blob-ish object
        glDisable(GL_TEXTURE_2D);
        glColor3f(0.5, 0.5, 0.5);
       MjbSphere(3, 50, 50);
    }
    if (LIGHT3On)
    {
        SetSpotLight(GL_LIGHT3, 10, 10, 10, -1, -1, -1, 1, 1, 1);
    }
   
    
    else
    {
        glDisable(GL_LIGHT3);
    }
    
    
    
 glPopMatrix();
 
   //glPushMatrix();
 //glDisable(GL_LIGHTING);
   
  //  glColor3f(0.722, 0.525, 0.043);
    //glPushMatrix();
    glTranslatef(0, -10, 0);
    //glDisable(GL_LIGHTING);
    glColor3f(0.13333,0.545098,0.13333);//green
    // glEnable(GL_LIGHT0);
    light();
    double i;
    glBegin(GL_TRIANGLES);//GL_LINES
    for(i=-1000;i<=1000;i+=0.5)
    {
        glVertex3d(-2500,-11,i);
        glVertex3d(2500,-11,i);
        glVertex3d(i,-11,-2500);
        glVertex3d(i,-11,2500);
    }
    glEnd();
  //  glEnable(GL_LIGHTING);
    
   //glPopMatrix();
    
    //the sky cube moves with the viewer
    if (LIGHT4On)
    {
        draw_sky();
        //   SetSpotLight(GL_LIGHT3, 10, 10, 10, -1, -1, -1, 1, 1, 1);
    }
   
   
    if (AxesOn != 0)
    {
        glColor3fv(&Colors[WhichColor][0]);
        glCallList(AxesList);
    }
    
    // since we are using glScalef( ), be sure normals get unitized:
    
    glEnable(GL_NORMALIZE);
    
   
    // draw the current object:
  //  glCallList(BoxList);
    
    if (DepthFightingOn != 0)
    {
        glPushMatrix();
        glRotatef(90., 0., 1., 0.);
        glCallList(BoxList);
       // draw_building();
        //draw_helipad();
        glPopMatrix();
        
    }
    glPushMatrix();
    glScalef(1.0, 1.0, 2.0);
    glutSolidSphere(5.0, 32, 32);
    glScalef(1.0, 1.0, 0.5);
    glPopMatrix();
    // draw some gratuitous text that just rotates on top of the scene:
    
    glDisable(GL_DEPTH_TEST);
    glColor3f(1., 1., 1.);
    DoRasterString(0., 1., 0., "");
    
    // draw some gratuitous text that is fixed on the screen:
    //
    // the projection matrix is reset to define a scene whose
    // world coordinate system goes from 0-100 in each axis
    //
    // this is called "percent units", and is just a convenience
    //
    // the modelview matrix is reset to identity as we don't
    // want to transform these coordinates
   
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 100., 0., 100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1., 1., 1.);
    DoRasterString(5., 5., 0., "Safarji@ best desgin");
    
    // swap the double-buffered framebuffers:
    
    glutSwapBuffers();
    
    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !
 
    glFlush();
    
}
void light (void)
{
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,global_ambient);// default ambient light when all sources are turned off
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);//calculate light at true position of viewer
    glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,material_specular);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,128.0);
    
    
}
void DoAxesMenu(int id)
{
    AxesOn = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoColorMenu(int id)
{
    WhichColor = id - RED;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDebugMenu(int id)
{
    DebugOn = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthBufferMenu(int id)
{
    DepthBufferOn = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthFightingMenu(int id)
{
    DepthFightingOn = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthMenu(int id)
{
    DepthCueOn = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}
void DoDistort(int id)
{
    Distort = (bool)id;
}
void DoTexture(int id)
{
    idTexture = id;
    switch (id)
    {
        case 0:
            Texture = Texture1;
            isTexture = true;
            break;
        case 1:
            Texture = Texture2;
            isTexture = true;
            break;
        case 2:
            Texture = Texture3;
            isTexture = true;
            break;
        case 3:
            isTexture = false;
            break;
    }
}

// main menu callback:

void DoMainMenu(int id)
{
    switch (id)
    {
        case RESET:
            Reset();
            break;
            
        case QUIT:
            // gracefully close out the graphics:
            // gracefully close the graphics window:
            // gracefully exit the program:
            glutSetWindow(MainWindow);
            glFinish();
            glutDestroyWindow(MainWindow);
            exit(0);
            break;
            
        default:
            fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoProjectMenu(int id)
{
    WhichProjection = id;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:

void DoRasterString(float x, float y, float z, char *s)
{
    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);
    
    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

// use glut to display a string of characters using a stroke font:

void DoStrokeString(float x, float y, float z, float ht, char *s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

// return the number of seconds since the start of the program:

float ElapsedSeconds()
{
    // get # of milliseconds since the start of the program:
    
    int ms = glutGet(GLUT_ELAPSED_TIME);
    
    // convert it to seconds:
    
    return (float)ms / 1000.f;
}

// initialize the glui window:

void InitMenus()
{
    glutSetWindow(MainWindow);
    
    int numColors = sizeof(Colors) / (3 * sizeof(int));
    int colormenu = glutCreateMenu(DoColorMenu);
    for (int i = 0; i < numColors; i++)
    {
        glutAddMenuEntry(ColorNames[i], i);
    }
    
    int axesmenu = glutCreateMenu(DoAxesMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int depthcuemenu = glutCreateMenu(DoDepthMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int debugmenu = glutCreateMenu(DoDebugMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int projmenu = glutCreateMenu(DoProjectMenu);
    glutAddMenuEntry("Orthographic", ORTHO);
    glutAddMenuEntry("Perspective", PERSP);
    
    int distortmenu = glutCreateMenu(DoDistort);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int texturemenu = glutCreateMenu(DoTexture);
    glutAddMenuEntry("StoneWall", 0);
    glutAddMenuEntry("Earth", 1);
    glutAddMenuEntry("Mars", 2);
    glutAddMenuEntry("No texture", 3);
    
    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Texture", texturemenu);
    glutAddSubMenu("Distortion", distortmenu);
    glutAddSubMenu("Axes", axesmenu);
    glutAddSubMenu("Colors", colormenu);
    glutAddSubMenu("Depth Buffer", depthbuffermenu);
    glutAddSubMenu("Depth Fighting", depthfightingmenu);
    glutAddSubMenu("Depth Cue", depthcuemenu);
    glutAddSubMenu("Projection", projmenu);
    glutAddMenuEntry("Reset", RESET);
    glutAddSubMenu("Debug", debugmenu);
    glutAddMenuEntry("Quit", QUIT);
    
    // attach the pop-up menu to the right mouse button:
    
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//    also setup display lists and callback functions

void InitGraphics()
{
    // request the display modes:
    // ask for red-green-blue-alpha color, double-buffering, and z-buffering:
    
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    
    // set the initial window configuration:
    
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);
    
    // open the window and set its title:
    
    MainWindow = glutCreateWindow(WINDOWTITLE);
    glutSetWindowTitle(WINDOWTITLE);
    
    // set the framebuffer clear values:
    
    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);
    
    // setup the callback functions:
    // DisplayFunc -- redraw the window
    // ReshapeFunc -- handle the user resizing the window
    // KeyboardFunc -- handle a keyboard input
    // MouseFunc -- handle the mouse button going down or up
    // MotionFunc -- handle the mouse moving with a button down
    // PassiveMotionFunc -- handle the mouse moving with a button up
    // VisibilityFunc -- handle a change in window visibility
    // EntryFunc    -- handle the cursor entering or leaving the window
    // SpecialFunc -- handle special keys on the keyboard
    // SpaceballMotionFunc -- handle spaceball translation
    // SpaceballRotateFunc -- handle spaceball rotation
    // SpaceballButtonFunc -- handle spaceball button hits
    // ButtonBoxFunc -- handle button box hits
    // DialsFunc -- handle dial rotations
    // TabletMotionFunc -- handle digitizing tablet motion
    // TabletButtonFunc -- handle digitizing tablet button hits
    // MenuStateFunc -- declare when a pop-up menu is in use
    // TimerFunc -- trigger something to happen a certain time from now
    // IdleFunc -- what to do when nothing else is going on
    
    glutSetWindow(MainWindow);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(NULL);
    glutVisibilityFunc(Visibility);
    glutEntryFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpaceballMotionFunc(NULL);
    glutSpaceballRotateFunc(NULL);
    glutSpaceballButtonFunc(NULL);
    glutButtonBoxFunc(NULL);
    glutDialsFunc(NULL);
    glutTabletMotionFunc(NULL);
    glutTabletButtonFunc(NULL);
    glutMenuStateFunc(NULL);
    glutTimerFunc(-1, NULL, 0);
    glutIdleFunc(Animate);
    
    // init glew (a window must be open to do this):
    
#ifdef WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "glewInit Error\n");
    }
    else
        fprintf(stderr, "GLEW initialized OK\n");
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void InitLists()
{
    //glLoadIdentity();
    
    float dx = BOXSIZE / 7.f;
    float dy = BOXSIZE / 9.f;
    float dz = BOXSIZE / 8.f;
    
    
    
    
    glPushMatrix();
    
    glScalef(50, 50, 1.0f);
    //glScalef(1.0f,1.0f,rotate_x);
  //  glRotatef(Time, -1.0f, 1.5f, -5.0f);
    BoxList = glGenLists(1);
    glNewList(BoxList, GL_COMPILE);
    // glTranslatef( 6, 5, 7 );
   // glRotatef(Time, 0., 0., 1.);
    glTranslatef(0.0, -10.0, -5.0);
    //glShadeModel( GL_FLAT );
    // glNormal3f( 5, 3,6);
    glBegin(GL_POLYGON);
    glNormal3f( 1, 1, 1 );
    glVertex2f(0.90, 0.60);
   // glRotatef(Time, -1.0f, 1.5f, -5.0f);
    glNormal3f( 2, 2, 2 );
    glVertex2f(0.50, 0.120);
    glVertex2f(0.10, 0.80);
    glVertex2f(0.80, 0.10);
    glBegin(GL_LINE_LOOP);//GL_QUADS GL_TRIANGLE_STRIP or GL_QUAD_STRIP GL_TRIANGLES GL_QUADS GL_LINE_LOOP
    
    
    
    
    glColor3f(0., 0., 1.);
    
    glColor3f(2., 0., 1.);
    glNormal3f(0., 0., 1.);
    glScalef(2.0f, 2.0f, 1.0f);
    glVertex3f(-dx, -dy, dz);
    glVertex3f(dx, -dy, dz);
    glVertex3f(dx, dy, dz);
    glNormal3f(-1., 0., 0.);
    glVertex3f(-dx, -dy, dz);
    glVertex3f(-dx, dy, dz);
    glVertex3f(-dx, dy, -dz);
    glVertex3f(-dx, -dy, -dz);
    for (dx = dy; dx <= 6.6; dx++) {
        glColor3f(0., 0., 1.);
        for (int i =0; i<10; i++) {
            glColor3f(i, 4., 1.);
            glVertex3f(1, i, dy);
            glVertex3f(i, 1, dx);
            glVertex3f(2, dx, -i);
            glVertex3f(-i, 2, -dz);
        }
        glScalef(2.0f, 2.0f, 1.0f);
        glVertex3f(-dx, dy, dz);
        glVertex3f(dx, dy, dz);
        glVertex3f(-dx, dy, -dz);
        glVertex3f(-dx, dy, -dz);
        glNormal3f(0., 0., -1.);
        glTexCoord2f(0., 0.);
        glVertex3f(-dx, -dy, -dz);
        glTexCoord2f(0., 1.);
        glVertex3f(-dx, dy, -dz);
        glTexCoord2f(1., 1.);
        glVertex3f(dx, dy, -dz);
        glTexCoord2f(1., 0.);
        glVertex3f(dx, -dy, -dz);
        glColor3f(4., 1., 0.);
        glNormal3f(0., 1., 0.);
        glVertex3f(-dx, dy, dz);
        glVertex3f(dx, dy, dz);
        glVertex3f(dx, dy, -dz);
        glVertex3f(-dx, dy, -dz);
    }
    glVertex3f(-dx, dy, dz);
    
    for (dy = dx; dy <= 6.6; dy++) {
        glColor3f(0., 4., 1.);
        glVertex3f(-1, dy, dz);
        glVertex3f(2, 1, 2);
        glVertex3f(2, 1, -dz);
        glVertex3f(-2, 2, -dz);
        for (int i =0; i<10; i++) {
            glColor3f(i, 4., 1.);
            glVertex3f(1, i, dy);
            glVertex3f(i, 1, dx);
            glVertex3f(2, dx, -i);
            glVertex3f(-i, 2, -dz);
        }
    }
    
    
    glEnd();
    glEndList();
    glPopMatrix();
    
     glMatrixMode(GL_MODELVIEW);
    // create the axes:
    AxesList = glGenLists(1);
    glNewList(AxesList, GL_COMPILE);
    glLineWidth(AXES_WIDTH);
    Axes(15.5);
    glLineWidth(1.);
    glEndList();
    
    
}

// the keyboard callback:

void Keyboard(unsigned char c, int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);
    
    switch (c)
    {
        case 'o':
        case 'O':
            WhichProjection = ORTHO;
            break;
            
        case 'p':
        case 'P':
            WhichProjection = PERSP;
            break;
        case 't':
        case 'T':
            if (idTexture < 3)
            {
                DoTexture(idTexture + 1);
            }
            else
            {
                DoTexture(0);
                idTexture = 0;
            }
            break;
        case 'd':
        case 'D':
            Distort = !Distort;
            break;
        case 'f':
        case 'F':
            Frozen = !Frozen;
            break;
        case '0':
            LIGHTING = !LIGHTING;
            break;
        case '1':
            LIGHT0On = !LIGHT0On;
            break;
        case '2':
            LIGHT1On = !LIGHT1On;
            break;
        case '3':
            LIGHT2On = !LIGHT2On;
            break;
        case '4':
            LIGHT3On = !LIGHT3On;
            break;
        case '5':
            LIGHT4On = !LIGHT4On;
        
            break;
        case 'q':
        case 'Q':
        case ESCAPE:
            DoMainMenu(QUIT); // will not return here
            break;            // happy compiler
            
        default:
            fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
    }
    
    // force a call to Display( ):
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y)
{
    int b = 0; // LEFT, MIDDLE, or RIGHT
    
    if (DebugOn != 0)
        fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);
    
    // get the proper button bit mask:
    
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            b = LEFT;
            break;
            
        case GLUT_MIDDLE_BUTTON:
            b = MIDDLE;
            break;
            
        case GLUT_RIGHT_BUTTON:
            b = RIGHT;
            break;
            
        default:
            b = 0;
            fprintf(stderr, "Unknown mouse button: %d\n", button);
    }
    
    // button down sets the bit, up clears the bit:
    
    if (state == GLUT_DOWN)
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b; // set the proper bit
    }
    else
    {
        ActiveButton &= ~b; // clear the proper bit
    }
}

// called when the mouse moves while a button is down:

void MouseMotion(int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "MouseMotion: %d, %d\n", x, y);
    
    int dx = x - Xmouse; // change in mouse coords
    int dy = y - Ymouse;
    
    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
    }
    
    if ((ActiveButton & MIDDLE) != 0)
    {
        Scale += SCLFACT * (float)(dx - dy);
        
        // keep object from turning inside-out or disappearing:
        
        if (Scale < MINSCALE)
            Scale = MINSCALE;
    }
    
    Xmouse = x; // new current position
    Ymouse = y;
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void Reset()
{
    ActiveButton = 0;
    AxesOn = 1;
    DebugOn = 0;
    DepthBufferOn = 1;
    DepthFightingOn = 0;
    DepthCueOn = 0;
    Scale = 1.0;
    WhichColor = WHITE;
    WhichProjection = PERSP;
    Xrot = Yrot = 0.;
}

// called when user resizes the window:

void Resize(int width, int height)
{
    if (DebugOn != 0)
        fprintf(stderr, "ReSize: %d, %d\n", width, height);
    
    // don't really need to do anything since window size is
    // checked each time in Display( ):
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// handle a change to the window's visibility:

void Visibility(int state)
{
    if (DebugOn != 0)
        fprintf(stderr, "Visibility: %d\n", state);
    
    if (state == GLUT_VISIBLE)
    {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
    0.f, 1.f, 0.f, 1.f};

static float xy[] = {
    -.5f, .5f, .5f, -.5f};

static int xorder[] = {
    1, 2, -3, 4};

static float yx[] = {
    0.f, 0.f, -.5f, .5f};

static float yy[] = {
    0.f, .6f, 1.f, 1.f};

static int yorder[] = {
    1, 2, 3, -2, 4};

static float zx[] = {
    1.f, 0.f, 1.f, 0.f, .25f, .75f};

static float zy[] = {
    .5f, .5f, -.5f, -.5f, 0.f, 0.f};

static int zorder[] = {
    1, 2, 3, 4, -5, 6};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//    Draw a set of 3D axes:
//    (length is the axis length in world coordinates)

void Axes(float length)
{
    glBegin(GL_LINE_STRIP);
    glVertex3f(length, 0., 0.);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., length, 0.);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., length);
    glEnd();
    
    float fact = LENFRAC * length;
    float base = BASEFRAC * length;
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++)
    {
        int j = xorder[i];
        if (j < 0)
        {
            
            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
    }
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 5; i++)
    {
        int j = yorder[i];
        if (j < 0)
        {
            
            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
    }
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++)
    {
        int j = zorder[i];
        if (j < 0)
        {
            
            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
    }
    glEnd();
}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//        glColor3fv( rgb );

void HsvRgb(float hsv[3], float rgb[3])
{
    // guarantee valid input:
    
    float h = hsv[0] / 60.f;
    while (h >= 6.)
        h -= 6.;
    while (h < 0.)
        h += 6.;
    
    float s = hsv[1];
    if (s < 0.)
        s = 0.;
    if (s > 1.)
        s = 1.;
    
    float v = hsv[2];
    if (v < 0.)
        v = 0.;
    if (v > 1.)
        v = 1.;
    
    // if sat==0, then is a gray:
    
    if (s == 0.0)
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }
    
    // get an rgb from the hue itself:
    
    float i = floor(h);
    float f = h - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * f);
    float t = v * (1.f - (s * (1.f - f)));
    
    float r, g, b; // red, green, blue
    switch ((int)i)
    {
        case 0:
            r = v;
            g = t;
            b = p;
            break;
            
        case 1:
            r = q;
            g = v;
            b = p;
            break;
            
        case 2:
            r = p;
            g = v;
            b = t;
            break;
            
        case 3:
            r = p;
            g = q;
            b = v;
            break;
            
        case 4:
            r = t;
            g = p;
            b = v;
            break;
            
        case 5:
            r = v;
            g = p;
            b = q;
            break;
    }
    
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}
