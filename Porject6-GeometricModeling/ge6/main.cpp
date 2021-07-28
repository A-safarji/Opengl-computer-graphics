#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4996)
//#include "glew.h"
#include "glut.h"
#endif

//#include <GL/gl.h>
//#include <GL/glu.h>
#include "glut.h"

float Time;
int resolution = 20;

// title of these windows:

const char *WINDOWTITLE = {"Project 6 - Geom"};
const char *GLUITITLE = {"Sample2015"};

// what the glui package defines as true and false:

const int GLUITRUE = {true};
const int GLUIFALSE = {false};

// the escape key:

#define ESCAPE 0x1b

// initial window size:

const int INIT_WINDOW_SIZE = {600};

// size of the box:

const float BOXSIZE = {2.f};

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
int Xmouse, Ymouse;  // mouse values
float Xrot, Yrot;    // rotation angles in degrees

bool Frozen = true;
bool controlPointsEnable = false;
bool controlLinesEnable = false;
bool numberOfPointsEnable = false;

struct Point
{
    float x0, y0, z0; // initial coordinates
    float x, y, z;    // animated coordinates
};

struct Curve
{
    float r, g, b;
    Point p0, p1, p2, p3;
};

Curve Curves[10]; // if you are creating a pattern of curves
Curve Stem;       // if you are not

#define MS_PER_CYCLE 2000
// function prototypes:

void Animate();
void Display();
Point initiatePoint(Point, float, float, float);
Curve initiateCurve(Curve, Point, Point, Point, Point, float, float, float);
void bezierWithCurve(struct Curve, float);
void DoBezierMenu(int);
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
// void InitLists();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Resize(int, int);
void Visibility(int);

void Axes(float);
void HsvRgb(float[3], float[3]);

// main program:

int main(int argc, char *argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)
    
    glutInit(&argc, argv);
    
    // setup all the graphics stuff:
    
    InitGraphics();
    
    // create the display structures that will not change:
    
    //InitLists();
    
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

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

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
    
    // set the viewing volume:
    // remember that the Z clipping  values are actually
    // given as DISTANCES IN FRONT OF THE EYE
    // USE gluOrtho2D( ) IF YOU ARE DOING 2D !
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
    gluLookAt(0, 0, 80, 0., 0., 0., 0., 1., 0.);
    
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
    
    // possibly draw the axes:
    if (AxesOn != 0)
    {
        glColor3fv(&Colors[WhichColor][0]);
        glCallList(AxesList);
    }
    
    // since we are using glScalef( ), be sure normals get unitized:
    
    glEnable(GL_NORMALIZE);
    
    struct Point p0;
    struct Point p01;
    struct Point p02;
    struct Point p03;
    
    float r, g, b;
    
    // Kirby is pink
    r = 0.5;
    g = 0.1;
    b = 0.2;
    
    
    // Draw the circle
    float xFace04 = 22;
    float xFace12 = 20;
    float yFace = 30;
    float zFace = 0;
    
    p0 = initiatePoint(p0, -xFace04, 0, zFace);
    p01 = initiatePoint(p01, -xFace12, yFace, zFace);
    p02 = initiatePoint(p02, xFace12, yFace, zFace);
    p03 = initiatePoint(p03, xFace04, 0, zFace);
    
    struct Curve cTopFace = initiateCurve(cTopFace, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cTopFace, resolution);
    
    p01 = initiatePoint(p01, -xFace12, -yFace, zFace);
    p02 = initiatePoint(p02, xFace12, -yFace, zFace);
    struct Curve cBottomFace = initiateCurve(cBottomFace, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cBottomFace, resolution);
    
  
    // Draw the circle right arm
    float yArm04 = 8;
    float yArm12 = 6;
    float xArm = 12 + xFace04;
    float zArm = 0;
    
    p0 = initiatePoint(p0, xFace04 - 2, -yArm04, zArm);
    p01 = initiatePoint(p01, xArm + sin(Time) * 10, -yArm12 + sin(Time) * 20, zArm + sin(Time) * 10);
    p02 = initiatePoint(p02, xArm + sin(Time) * 10, yArm12 + sin(Time) * 20, zArm + sin(Time) * 10);
    p03 = initiatePoint(p03, xFace04 - 2, yArm04, zArm);
    
    struct Curve cRightArm = initiateCurve(cRightArm, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightArm, resolution);
    
    // Draw the circle which is the left arm of Kirby
    p0 = initiatePoint(p0, -xFace04 + 2, -yArm04, zArm);
    p01 = initiatePoint(p01, -xArm, -yArm12 + sin(Time) * 20, zArm + sin(Time) * 10);
    p02 = initiatePoint(p02, -xArm, yArm12 + sin(Time) * 20, zArm + sin(Time) * 10);
    p03 = initiatePoint(p03, -xFace04 + 2, yArm04, zArm);
    
    struct Curve cLeftArm = initiateCurve(cLeftArm, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cLeftArm, resolution);
    
   
    // Draw the circle
    float xEyes0 = 8;
    float xEyes1 = 8;
    float xEyes2 = 8;
    float xEyes3 = 8;
    
    float yEyes0 = 8;
    float yEyes1 = 9;
    float yEyes2 = 12;
    float zEyes = 9;
    
    p0 = initiatePoint(p0, xEyes0, yEyes0, zEyes);
    p01 = initiatePoint(p01, xEyes1, yEyes1, zEyes);
    p02 = initiatePoint(p02, xEyes2, yEyes1, zEyes);
    p03 = initiatePoint(p03, xEyes3, yEyes0, zEyes);
    
    struct Curve cRightEyeTop = initiateCurve(cRightEyeTop, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightEyeTop, resolution);
    
    p01 = initiatePoint(p01, xEyes1, yEyes2, zEyes);
    p02 = initiatePoint(p02, xEyes2, yEyes2, zEyes);
    struct Curve cRightEyeBottom = initiateCurve(cRightEyeBottom, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightEyeBottom, resolution);
    
    // sttright Right
    p0 = initiatePoint(p0, xEyes0 + 1, yEyes0 + 5, zEyes + Time * 2);
    p01 = initiatePoint(p01, xEyes1, yEyes1 - 5, zEyes + Time * 2);
    p02 = initiatePoint(p02, xEyes2, yEyes1 - 5, zEyes + Time * 2);
    p03 = initiatePoint(p03, xEyes3 - 1, yEyes0 + 5, zEyes + Time * 2);
    struct Curve cRightEyePupilTop = initiateCurve(cRightEyePupilTop, p0, p01, p02, p03, 0.8, 0.8, 1);
    bezierWithCurve(cRightEyePupilTop, resolution);
    
    p01 = initiatePoint(p01, xEyes1, yEyes1 - 10, zEyes + Time * 2);
    p02 = initiatePoint(p02, xEyes2, yEyes1 - 10, zEyes + Time * 2);
    struct Curve cRightEyePupilBottom = initiateCurve(cRightEyePupilBottom, p0, p01, p02, p03, 0.8, 0.8, 1);
    bezierWithCurve(cRightEyePupilBottom, resolution);
    
    // Draw the circle
    p0 = initiatePoint(p0, -xEyes0, yEyes0, zEyes);
    p01 = initiatePoint(p01, -xEyes1, yEyes1, zEyes);
    p02 = initiatePoint(p02, -xEyes2, yEyes1, zEyes);
    p03 = initiatePoint(p03, -xEyes3, yEyes0, zEyes);
    
    struct Curve cLeftEyeTop = initiateCurve(cLeftEyeTop, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cLeftEyeTop, resolution);
    
    p01 = initiatePoint(p01, -xEyes1, yEyes2, zEyes);
    p02 = initiatePoint(p02, -xEyes2, yEyes2, zEyes);
    struct Curve cLeftEyeBottom = initiateCurve(cLeftEyeBottom, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cLeftEyeBottom, resolution);
    
    // Pupil left
    
    p0 = initiatePoint(p0, -xEyes0 - 1, yEyes0 + 5, zEyes + Time * 2);
    p01 = initiatePoint(p01, -xEyes1, yEyes1 - 5, zEyes + Time * 2);
    p02 = initiatePoint(p02, -xEyes2, yEyes1 - 5, zEyes + Time * 2);
    p03 = initiatePoint(p03, -xEyes3 + 1, yEyes0 + 5, zEyes + Time * 2);
    
    struct Curve cLeftEyePupilTop = initiateCurve(cLeftEyePupilTop, p0, p01, p02, p03, 0.8, 0.8, 1);
    bezierWithCurve(cLeftEyePupilTop, resolution);
    
    p01 = initiatePoint(p01, -xEyes1, yEyes1 - 10, zEyes + Time * 2);
    p02 = initiatePoint(p02, -xEyes2, yEyes1 - 10, zEyes + Time * 2);
    
    struct Curve cLeftEyePupilBottom = initiateCurve(cLeftEyePupilBottom, p0, p01, p02, p03, 0.8, 0.8, 1);
    bezierWithCurve(cLeftEyePupilBottom, resolution);
    
   
    
    //  right red
    glTranslatef(0, -22, 8);
    p0 = initiatePoint(p0, 8, 10, zEyes);
    p01 = initiatePoint(p01, 9, -4, zEyes);
    p02 = initiatePoint(p02, 14, 4, zEyes);
    p03 = initiatePoint(p03, -15, 0, zEyes);
    p0 = initiatePoint(p0, -8, -10, zEyes);
    p01 = initiatePoint(p01, -9, -4, zEyes);
    p02 = initiatePoint(p02, -14, 3, zEyes);
    p03 = initiatePoint(p03, 15, 5, zEyes);
    
    struct Curve cRightCheekTop = initiateCurve(cRightCheekTop, p0, p01, p02, p03, 1, 0, 0);
    bezierWithCurve(cRightCheekTop, resolution);
    
    p01 = initiatePoint(p01, 9, -4, zEyes);
    p02 = initiatePoint(p02, 14, -4, zEyes);
    struct Curve cRightCheekBottom = initiateCurve(cRightCheekBottom, p0, p01, p02, p03, 1, 0, 0);
    bezierWithCurve(cRightCheekBottom, resolution);
    
    // Draw  left
    glTranslatef(11, 43, 0);
    p0 = initiatePoint(p0, -5, 9, zEyes);
    p01 = initiatePoint(p01, -9, 4, zEyes);
    p02 = initiatePoint(p02, -14, 4, zEyes);
    p03 = initiatePoint(p03, -15, 0, zEyes);
    
    struct Curve cLeftCheekTop = initiateCurve(cLeftCheekTop, p0, p01, p02, p03, 1, 0, 0);
    bezierWithCurve(cLeftCheekTop, resolution);
    
    p01 = initiatePoint(p01, -9, -4, zEyes);
    p02 = initiatePoint(p02, -14, -4, zEyes);
    struct Curve cLeftCheekBottom = initiateCurve(cLeftCheekBottom, p0, p01, p02, p03, 1, 0, 0);
    bezierWithCurve(cLeftCheekBottom, resolution);
    
   
    
    glTranslatef(0, 0, 0);
   // p0 = initiatePoint(p0, 2, -10, 0);
   // p01 = initiatePoint(p01, 10 + Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
    //p02 = initiatePoint(p02, 14 + Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
   // p03 = initiatePoint(p03, 10, -10, 0);
    
    struct Curve cRightFootTop = initiateCurve(cRightFootTop, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightFootTop, resolution);
    
    // Draw  right
     glTranslatef(4, 5, 0);
    p0 = initiatePoint(p0, 2, -30, 0);
    p01 = initiatePoint(p01, -10 - Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
    p02 = initiatePoint(p02, -20 - Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
    p03 = initiatePoint(p03, -30, -30, 0);
    
    struct Curve cRightFootBottom = initiateCurve(cRightFootBottom, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightFootBottom, resolution);
    
    
    glTranslatef(2, -2, 9);
    p0 = initiatePoint(p0, 2, -30, 0);
    p01 = initiatePoint(p01, -10 - Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
    p02 = initiatePoint(p02, -20 - Time * 10, -40 - sin(Time) * 10, 0 + sin(Time) * 10);
    p03 = initiatePoint(p03, -30, -30, 0);
    
  //  struct Curve cRightFootBottom = initiateCurve(cRightFootBottom, p0, p01, p02, p03, r, g, b);
    bezierWithCurve(cRightFootBottom, resolution);
    
    
    /////////////  //////////////////double i;
    double i;
      glTranslatef(-20, 40, -20);
    glColor3f(0.13333,0.545098,0.13333);// green
    
    //glBegin(GL_LINES); // mesh
    for(i=-60;i<=100;i+=0.2)
    {
        glVertex3d(-20,-12,i);
        glVertex3d(20,-12,i);
        glVertex3d(i,-12,-20);
        glVertex3d(i,-12,20);
    }
    // Draw mouth curv
    glTranslatef(11, 10, 0);
    for (int i=0; i<=50; i++) {
        p0 = initiatePoint(p0, i, 30, zEyes);
        p01 = initiatePoint(p01, i, -i + sin(Time) * 10, zEyes);
        p02 = initiatePoint(p02, -i, -i + sin(Time) * 10, zEyes);
        p03 = initiatePoint(p03, -i, -i, zEyes);
        for (int x=i; x<=50; x+=2) {
            p0 = initiatePoint(p0, x,10, zEyes);
            p01 = initiatePoint(p01, -x, x + sin(Time) * 10, zEyes);
            p02 = initiatePoint(p02, i-x, x + sin(Time) * 10, zEyes);
            p03 = initiatePoint(p03, -i, -i, zEyes);
            
        }
        p0 = initiatePoint(p0, i, 30, zEyes);
        p01 = initiatePoint(p01, i, -i + sin(Time) * 10, zEyes);
        p02 = initiatePoint(p02, -i, -i + sin(Time) * 10, zEyes);
        p03 = initiatePoint(p03, -i, -i, zEyes);
        
    }
   
    
    
    
    struct Curve cMouthTop = initiateCurve(cMouthTop, p0, p01, p02, p03, 0.1, 0.2, 0.3);
    bezierWithCurve(cMouthTop, resolution);
    
    p01 = initiatePoint(p01, 2, -14 - sin(Time) * 10, zEyes);
    p02 = initiatePoint(p02, -2, -14 - sin(Time) * 10, zEyes);
    struct Curve cMouthBottom = initiateCurve(cMouthBottom, p0, p01, p02, p03, 0.1, 0.2, b);
    bezierWithCurve(cMouthBottom, resolution);
    
    if (DepthFightingOn != 0)
    {
        glPushMatrix();
        glRotatef(90., 0., 1., 0.);
        glCallList(BoxList);
        glPopMatrix();
    }
    
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
    DoRasterString(5., 5., 0., "Safarji");
    char txt[32];
    sprintf(txt, "Resolution:%6d", resolution);
    DoRasterString(80., 5., 0., txt);
    
    glutSwapBuffers();
    
    
    
    glFlush();
}

Curve initiateCurve(Curve c, Point p0, Point p1, Point p2, Point p3, float r, float g, float b)
{
    c.p0 = p0;
    c.p1 = p1;
    c.p2 = p2;
    c.p3 = p3;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

Point initiatePoint(Point p, float x0, float y0, float z0)
{
    p.x = x0;
    p.y = y0 + sin(Time) * 20;
    p.z = z0 + Time * 50;
    return p;
}

void bezierWithCurve(struct Curve c, float NUMPOINTS)
{
    if (controlPointsEnable)
    {
        glColor3f(1, 0, 0);
        glPointSize(5.0f);
        glBegin(GL_POINTS);
        {
            glVertex3f(c.p0.x, c.p0.y, c.p0.z);
            glVertex3f(c.p1.x, c.p1.y, c.p1.z);
            glVertex3f(c.p2.x, c.p2.y, c.p2.z);
            glVertex3f(c.p3.x, c.p3.y, c.p3.z);
        }
        glEnd();
    }
    if (controlLinesEnable)
    {
        glPointSize(5.0f);
        glBegin(GL_LINE_STRIP);
        {
            glColor3f(0.6, 0.6, 0.6);
            glVertex3f(c.p0.x, c.p0.y, c.p0.z);
            glVertex3f(c.p1.x, c.p1.y, c.p1.z);
            glVertex3f(c.p2.x, c.p2.y, c.p2.z);
            glVertex3f(c.p3.x, c.p3.y, c.p3.z);
        }
        glEnd();
    }
    glLineWidth(3.);
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_LINE_STRIP);
    
    for (int it = 0; it <= NUMPOINTS; it++)
    {
        float t = (float)it / (float)NUMPOINTS;
        float omt = 1.f - t;
        float x = omt * omt * omt * c.p0.x + 3.f * t * omt * omt * c.p1.x + 3.f * t * t * omt * c.p2.x + t * t * t * c.p3.x;
        float y = omt * omt * omt * c.p0.y + 3.f * t * omt * omt * c.p1.y + 3.f * t * t * omt * c.p2.y + t * t * t * c.p3.y;
        float z = omt * omt * omt * c.p0.z + 3.f * t * omt * omt * c.p1.z + 3.f * t * t * omt * c.p2.z + t * t * t * c.p3.z;
        glVertex3f(x, y, z);
    }
    glEnd();
    glLineWidth(1.);
}

void DoBezierMenu(int id)
{
    if (id == 0) {
        controlPointsEnable = !controlPointsEnable;
    }
    if (id == 1) {
        controlLinesEnable = !controlLinesEnable;
    }
    
    glutSetWindow(MainWindow);
    glutPostRedisplay();
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
    
    int beziermenu = glutCreateMenu(DoBezierMenu);
    glutAddMenuEntry( "Control points",  0 );
    glutAddMenuEntry( "Control lines",  1 );
    
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
    
    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Bezier", beziermenu);
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
        case 'c':
        case 'C':
            controlPointsEnable = !controlPointsEnable;
            break;
        case 'l':
        case 'L':
            controlLinesEnable = !controlLinesEnable;
            break;
        case 'p':
        case 'P':
            WhichProjection = PERSP;
            break;
        case 'f':
        case 'F':
            Frozen = !Frozen;
            break;
        case '+':
            resolution = resolution + 1;
            break;
        case '-':
            resolution = resolution - 1;
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
