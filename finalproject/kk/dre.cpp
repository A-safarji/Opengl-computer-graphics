#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <OpenGL/gl.h>

#include <OpenGl/glu.h>

#include <GLUT/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

void draw_heli();
void draw_body();
void draw_rotor();
void draw_tail();
void draw_leg();
void draw_tail_fan();
void draw_grass();
void draw_helipad();
void draw_building();
void draw_sky();
void polygon(int a,int b,int c,int d);
void draw_house();
void fly_effect();
void my_menu(int id);
void view_menu(int id);
void rotate_menu(int id);
void house_color_menu(int id);
void helicopter_color_menu(int id);
void help_page();
void draw_mountain();
void Sprint( int x, int y,float,float,float, char *st,void *font);

GLint sub_menu[5];//list of submenus

//lighting and shading
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

GLfloat helicopterAmbientLight[]={0.2,0.2,0.2,1.0};//grey

GLfloat helicopterDiffuseLight[3][4]={{0.4964028776978417266187050359,0.84509803921568627450980392,0.0,1.0},//green
    {0.74117647058823529411764705353,0.7176470588235294117647058823,0.41960784313725490196078431372,1.0},//khakhi
    {0.3882352941176470588235294117,0.7215686274509803921568627450,1.0,1.0}};//blue


//movement coordinates
GLfloat myx=0.0, myy=0.0, myz=0.0, rotation=0.0, rotor_angle=0.0, heli_tilt=0.0;

//coordinates of cube for sky
GLfloat vertices[8][3]={{-500.0,-50.0,-500.0},{500.0,-50.0,-500.0},{500.0,500.0,-500.0},{-500.0,500.0,-500.0},
    {-500.0,-50.0,500.0},{500.0,-50.0,500.0},{500.0,500.0,500.0},{-500.0,500.0,500.0}};

//color of cube for sky
GLdouble colors[8][4]={{0.8862745098039216,1.0,1.0,0.5},{0.8862745098039216,1.0,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},
    {0.8862745098039216,1.0,1.0,0.5},{0.8862745098039216,1.0,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5},{0.4,0.8784313725490196,1.0,0.5}};

//flags
int start=0,stop=0,forward_tilt=0,left_tilt=0,right_tilt=0,help=0,bc=0,hc=0;

//viewer
GLfloat viewer[]={0.0,60.0,-5.0};
//at coordinates
GLfloat at[]={0.0,0.0,0.0};


void init (void)
{
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
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

void display (void)
{
    
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(viewer[0],viewer[1],viewer[2],at[0],at[1],at[2],0.0,1.0,0.0);
    
    light();
    if(help)
    {
        help_page();
        
    }
    else
    {
        draw_grass();
        
        glTranslatef(myx+2,myy+2,myz+2);
        draw_sky(); //the sky cube moves with the viewer
        glTranslatef(-myx+2,-myy+2,-myz+2);
        
        draw_heli();
        
        draw_helipad();
        
        draw_building();
        
    }
    
    glFlush();
    
    glutSwapBuffers();
}

void draw_grass()
{
    glPushMatrix();
    
    glDisable(GL_LIGHTING);
    glColor3f(0.13333,0.545098,0.13333);//green
    double i;
    glBegin(GL_LINES);
    for(i=-2500.0;i<=2500;i+=0.5)
    {
        glVertex3d(-2500,-11,i);
        glVertex3d(2500,-11,i);
        glVertex3d(i,-11,-2500);
        glVertex3d(i,-11,2500);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    
    
    glPopMatrix();
    
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

void fly_effect()
{
    if(forward_tilt)
        glRotatef(10.0,1.0,0.0,0.0);
    
    if(right_tilt)
        glRotatef(30.0,0.0,0.0,1.0);
    
    if(left_tilt)
        glRotatef(-30.0,0.0,0.0,1.0);
    
    
}



void draw_heli()
{
    
    
    glPushMatrix();
    
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,helicopterAmbientLight);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,helicopterDiffuseLight[hc]);
    
    glTranslatef(myx, myy, myz);
    fly_effect();
    glRotatef(rotation, 0.0, 1.0, 0.0);  //rotation
    glTranslatef(-myx, -myy, -myz);
    glTranslatef(myx,myy,myz);
    glRotatef(heli_tilt, 0.0, 0.0, 1.0);  //tilting
    
    draw_body();
    
    draw_rotor();
    
    draw_tail();
    
    draw_leg();
    
    draw_tail_fan();
    
    glPopMatrix();
    
}

void draw_body()
{
    glPushMatrix();
    glScalef(1.0, 1.0, 2.0);
    glutSolidSphere(5.0, 32, 32);
    glScalef(1.0, 1.0, 0.5);
    glPopMatrix();
}

void draw_rotor()
{
    glPushMatrix();
    
    glDisable(GL_LIGHTING);
    glColor4f(0.75294117647058823529411764705882,0.75294117647058823529411764705882,0.75294117647058823529411764705882,1.0);//silver
    glLineWidth(5.0);
    
    glBegin(GL_LINES);//fan stick
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 10.0, 0.0);
    glEnd();
    glScalef(2.0, 1.0, 2.0);
    
    
    //rotor blades
    for(int i=0; i<4; i++)
    {
        glPushMatrix();
        glRotatef(rotor_angle, 0.0, 1.0, 0.0);
        
        
        glTranslatef(0.0, 10.0, 0.0);
        glRotatef(30.0, 1.0, 0.0, 0.0);
        glTranslatef(0.0, -10.0, 0.0);
        
        glBegin(GL_QUADS);
        
        glVertex3f(0.0, 10.0, -0.5);
        glVertex3f(10.0, 10.0, -1.0);
        glVertex3f(10.0, 10.0, 1.0);
        glVertex3f(0.0, 10.0, 0.5);
        
        glEnd();
        glPopMatrix();
        
        glRotatef(90.0, 0.0, 1.0, 0.0);
    }
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
}

void draw_tail()
{
    //tail of heli
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glColor4f(0.4,0.4,0.4,1.0);//grey
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(1.2,-2.0,-9.0);
    glVertex3f(1.2,2.0,-9.0);
    glVertex3f(0.2,0.2,-29.0);
    glVertex3f(0.2,-0.2,-29.0);
    glEnd();
    
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(-1.2,-2.0,-9.0);
    glVertex3f(-1.2,2.0,-9.0);
    glVertex3f(-0.2,0.2,-29.0);
    glVertex3f(-0.2,-0.2,-29.0);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(1.2,2.0,-9.0);
    glVertex3f(-1.2,2.0,-9.0);
    glVertex3f(-0.2,0.2,-29.0);
    glVertex3f(0.2,0.2,-29.0);
    glEnd();
    
    glBegin(GL_LINE_LOOP);
    glVertex3f(1.2,-2.0,-9.0);
    glVertex3f(-1.2,-2.0,-9.0);
    glVertex3f(-0.2,-0.2,-29.0);
    glVertex3f(0.2,-0.2,-29.0);
    glEnd();
    
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
    
}

void draw_leg()
{
    glPushMatrix();
    
    glDisable(GL_LIGHTING);
    glColor4f(0.7,0.7,0.7,1.0);//grey
    
    glBegin(GL_LINES);
    //first leg
    glVertex3f(5,-7,7);
    glVertex3f(5,-7,-7);
    
    glVertex3f(5,-7,7);
    glVertex3f(5,-6,8);
    
    glVertex3f(5,-7,-7);
    glVertex3f(5,-6,-8);
    
    glVertex3f(0,0.0,5);
    glVertex3f(5,-7.0,5);
    
    glVertex3f(0,0.0,-5);
    glVertex3f(5,-7.0,-5);
    
    glVertex3f(-5,-7,7);
    glVertex3f(-5,-7,-7);
    
    //second leg
    glVertex3f(-5,-7,7);
    glVertex3f(-5,-6,8);
    
    glVertex3f(-5,-7,-7);
    glVertex3f(-5,-6,-8);
    
    glVertex3f(0,0.0,5);
    glVertex3f(-5,-7.0,5);
    
    glVertex3f(0,0.0,-5);
    glVertex3f(-5,-7.0,-5);
    
    glEnd();
    
    glEnable(GL_LIGHTING);
    
    glPopMatrix();
}

void draw_tail_fan()
{
    glPushMatrix();
    glTranslatef(0.0,0.0,-28.0);
    glRotatef(-90.0,0.0,0.0,1.0);
    glScalef(0.15,0.11,0.15);
    draw_rotor();
    glPopMatrix();
    
}

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

// This prints a string to the screen
void Sprint( int x, int y,float r,float g,float b, char *st,void* font)
{
    int l,i;
    
    //   l=Strlen( st ); // see how many characters are in text string.
    glRasterPos2i( x, y); // location to start printing text
    glColor3f(r,g,b); //color of text
    for( i=0; i < l; i++) // loop until i is greater then l
    {
        glutBitmapCharacter(font, st[i]); // Print a character on the screen
    }
}

void help_page()
{
    char str1[]="About the Package";
    char str2[]="The Package presents a helicopter in a scene where the scene comprises of grass,helipad,buildings and sky.";
    char str3[]="Initially the user sees the top view of Helicopter,when he/she starts the Helicopter it raises to certain altitude.";
    char str4[]="Now he user can select one of the four views and move the Helicopter.Time being he/she can also change the colors.";
    char str5[]="HAPPY JOURNEY.......";
    char str6[]="Designed and Developed by:";
    char str7[]="Akash Kumar R ";
    char str8[]="1BI10CS007";
    
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glClearColor(1.0,1.0,1.0,1.0);
    
    Sprint(2,56,0.3,0.3,0.3,str1,GLUT_BITMAP_TIMES_ROMAN_24);
    
    
    Sprint(12,55,0.3,0.3,0.3,str2,GLUT_BITMAP_TIMES_ROMAN_24);
    
    
    Sprint(14,54,0.3,0.3,0.3,str3,GLUT_BITMAP_TIMES_ROMAN_24);
    
    Sprint(16,53,0.3,0.3,0.3,str4,GLUT_BITMAP_TIMES_ROMAN_24);
    
    Sprint(5,51,0.3,0.3,0.3,str5,GLUT_BITMAP_TIMES_ROMAN_24);
    
    Sprint(180,-20,0.3,0.3,0.3,str6,GLUT_BITMAP_TIMES_ROMAN_24);
    
    Sprint(420,-400,0.3,0.3,0.3,str7,GLUT_BITMAP_TIMES_ROMAN_24);
    
    Sprint(0,-400,0.3,0.3,0.3,str8,GLUT_BITMAP_TIMES_ROMAN_24);
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
}


void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (100, (GLfloat)w / (GLfloat)h, 0.1, 1500.0);
    
    glMatrixMode (GL_MODELVIEW);
    glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{
    //explicit viewer movements
    if(key=='b'||key=='B') {help=0;}
    
    if(key=='x'){viewer[0]+=1.0;}
    if(key=='X'){viewer[0]-=1.0;}
    if(key=='y') {viewer[1]+=1.0;}
    if(key=='Y') {viewer[1]-=1.0;}
    if(key=='z') {viewer[2]+=1.0;}
    if(key=='Z') {viewer[2]-=1.0;}
    if(key=='r') rotation++;
    if(key=='R') rotation--;
    if(key=='h') heli_tilt++;
    if(key=='H') heli_tilt--;
    
}


void Specialkey(int key,int x,int y)
{
    
    if(start)
    {
        if(key==GLUT_KEY_UP) {myz++; viewer[2]+=1.0;at[2]+=1.0;forward_tilt=1; }
        if(key==GLUT_KEY_RIGHT) {myx--; viewer[0]-=1.0;at[0]-=1.0;right_tilt=1;}
        if(key==GLUT_KEY_LEFT) {myx++; viewer[0]+=1.0;at[0]+=1.0;left_tilt=1;}
        if(key==GLUT_KEY_DOWN) {if(myy>0.0)myy--;if(viewer[1]>0)viewer[1]-=1.0;at[1]-=1.0;}
    }
    
}
void idle()
{
    if(start)
    {//start up
        if(rotor_angle<100)
            rotor_angle+=200.0;
        else if(rotor_angle<200)
            rotor_angle+=20.5;
        else if(rotor_angle<300)
            rotor_angle+=300.0;
        else if(rotor_angle<400)
            rotor_angle+=300.5;
        else
            rotor_angle+=500.0;
        if(rotor_angle>1000 && rotor_angle<100000)
        {
            if(myy<100)
            {
                myy+=1.0;
                at[1]+=1.0;
            }
            
        }
    }
    if(stop)
    {
        if(myy>0.0)
        {
            myy-=1.0;
            at[1]-=1.0;
            if(viewer[1]>0)
                viewer[1]-=1.0;
        }
        else
        {
            start=0;
            stop=0;
        }
    }
    display();
}

void Releasekey(int key,int x,int y)
{
    if(key==GLUT_KEY_UP) {forward_tilt=0; }
    if(key==GLUT_KEY_RIGHT) {right_tilt=0;}
    if(key==GLUT_KEY_LEFT) {left_tilt=0;}
    
}

void my_menu(int id)
{
    switch(id)
    {
        case 1: start=1;stop=0;rotor_angle=0.0; viewer[0]=40.0+myx;viewer[1]=0.0+myy;viewer[2]=0.0+myz;
            break;
        case 2:myy+=3.0;at[1]+=3.0;
            break;
        case 3:stop=1;
            break;
        case 4:help=1;
            break;
        case 5:exit(0);
    }
    glutPostRedisplay();
    
}

void view_menu(int id)
{
    switch(id)
    {
        case 1: viewer[0]=0.0+myx;viewer[1]=1.0;viewer[2]=-2.0+myz;at[0]=myx;at[1]=myy;at[2]=myz;
            break;
        case 2: viewer[0]=0.0+myx;viewer[1]=5.0+myy;viewer[2]=15.0+myz;at[0]=viewer[0]+0.0;at[1]=0.0+viewer[1];at[2]=15.0+viewer[2];
            break;
        case 3: viewer[0]=0.0+myx;viewer[1]=100.0+myy;viewer[2]=-15.0+myz;at[0]=myx;at[1]=myy;at[2]=myz;
            break;
        case 4:viewer[0]=7.0+myx;viewer[1]=0.0+myy;viewer[2]=-35.0+myz;at[0]=myx;at[1]=myy;at[2]=myz;
            break;
            
    }
    glutPostRedisplay();
    
}

void rotate_menu(int id)
{
    switch(id)
    {
        case 1:rotation=90.0;
            break;
        case 2:rotation=-90.0;
            break;
        case 3:rotation=0.0;
            break;
            
    }
    glutPostRedisplay();
}

void house_color_menu(int id)
{
    switch(id)
    {
        case 1:bc=1;
            break;
        case 2:bc=2;
            break;
        case 3:bc=0;
            break;
            
    }
    glutPostRedisplay();
}

void helicopter_color_menu(int id)
{
    switch(id)
    {
        case 1:hc=1;
            break;
        case 2:hc=2;
            break;
        case 3:hc=0;
            break;
            
    }
    glutPostRedisplay();
}

int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH|GLUT_RGBA);
    glutInitWindowSize (250, 250);
    glutInitWindowPosition (0,0);
    glutCreateWindow ("The Helicopter");
    init ();
    glutDisplayFunc (display);
    
    glutIdleFunc (idle);
    glutKeyboardFunc (keyboard);
    glutSpecialFunc(Specialkey);
    glutSpecialUpFunc(Releasekey);
    glutReshapeFunc (reshape);
    
    sub_menu[0] =glutCreateMenu(view_menu);
    glutAddMenuEntry("Ground View",1);
    glutAddMenuEntry("Pilot View",2);
    glutAddMenuEntry("Landscape View",3);
    glutAddMenuEntry("Follow View",4);
    
    sub_menu[1] =glutCreateMenu(rotate_menu);
    glutAddMenuEntry("90 degree",1);
    glutAddMenuEntry("-90 degree",2);
    glutAddMenuEntry("0 degree",3);
    
    sub_menu[2] =glutCreateMenu(house_color_menu);
    glutAddMenuEntry("Red and Blue",1);
    glutAddMenuEntry("Brown and Orange",2);
    glutAddMenuEntry("default",3);
    
    sub_menu[3] =glutCreateMenu(helicopter_color_menu);
    glutAddMenuEntry("khakhi",1);
    glutAddMenuEntry("blue",2);
    glutAddMenuEntry("default",3);
    
    glutCreateMenu(my_menu);
    glutAddMenuEntry("START",1);
    glutAddSubMenu("Viewing",sub_menu[0]);
    glutAddSubMenu("Rotate",sub_menu[1]);
    glutAddMenuEntry("Move Up",2);
    glutAddSubMenu("Color of House",sub_menu[2]);
    glutAddSubMenu("Color of Helicopter",sub_menu[3]);
    glutAddMenuEntry("STOP",3);
    glutAddMenuEntry("Help",4);
    glutAddMenuEntry("Exit",5);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    
    glutMainLoop ();
}
