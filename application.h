#ifndef APPLICATION_H
#define APPLICATION_H

#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "point.h"
#include "bezier.h"

// set window size
#define WINDOW_HEIGHT 512
#define WINDOW_WIDTH 512

// set number of vertex per bezier curve
// the more, the better quality of curve
#define POINTS_PER_BEZIER 2056
    
// equals -1 when no circle is dragging
// otherwise equals the number of dragging circle
int isMoving;

// pixel array
GLubyte pixelArray[WINDOW_WIDTH][WINDOW_HEIGHT][3];

// texture id
GLuint textureId;

// structure representing circle
typedef struct{
    double radius;
    
    // center of circle
    Point center;
} Circle;

// set of circles representing points on the screen
Circle circles[5];

void drawLineStrip(){
    glLoadIdentity();
    glColor3ub(0, 128, 128);
    glLineWidth(4);
    
    glBegin(GL_LINE_STRIP);
    for(int i = 0; i < 5; ++i){
        glVertex2f(circles[i].center.x, circles[i].center.y);
    }
    glEnd();
}

// draws single circle object
void drawCircle(Circle circle){
    const double twicePi = 2.0 * 3.142;
    
    glLoadIdentity();
    glColor3ub(0, 128, 128);
    
    // begin drawing circle
    glBegin(GL_TRIANGLE_FAN);
    
    // center of a circle
    glVertex2f(circle.center.x, circle.center.y);
    for(int i = 0; i <= 20; ++i){
        glVertex2f (
            ( circle.center.x + (circle.radius * cos(i * twicePi / 20)) ), ( circle.center.y + (circle.radius * sin(i * twicePi / 20)) )
        );        
    }
    
    // end drawing a circle
    glEnd();
}

void drawBezier(){
    Point points[5];
    
    for(int i = 0; i < 5; ++i){
        points[i] = circles[i].center;
    }
    
    glLoadIdentity();
    glColor3ub(0, 192, 192);
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for(double t = 0; t <= 1; t += 0.01){
        Point tmp = getBezierPoint(points, 5, t);
        glVertex2f(tmp.x, tmp.y);
    }
    glEnd();
}

void wipePixelArray(){
    for(int i = 0; i < WINDOW_WIDTH; ++i){
        for(int j = 0; j < WINDOW_HEIGHT; ++j){
            pixelArray[i][j][0] = 255;
            pixelArray[i][j][1] = 255;
            pixelArray[i][j][2] = 255;
        }
    }
}

void drawAsmBezier(float step){
    wipePixelArray();
    
    // prepare coordinates of points
    // as they are in one array
    float points[10];
    for(int i = 0; i < 5; ++i){
        points[2*i] = circles[i].center.x;
        points[2*i+1] = WINDOW_HEIGHT-1 - circles[i].center.y;
    }
    
    // draw bezier with asm
    asmDrawBezier(points, step, pixelArray, WINDOW_WIDTH);
    
    glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, pixelArray);
}

// displays whole scene
void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    
    // loads texture
    drawAsmBezier((float)1/POINTS_PER_BEZIER);
    
    // draw lines between circles
    drawLineStrip();

    // draw bezier
//     drawBezier();
    
    // draw circles
    for(int i = 0; i < 5; ++i){
        drawCircle(circles[i]);
    }
    
    glFlush();
}

// sets basic GL configuration
void GLSetup(int width, int height) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

// moves dragging circle with cursor
void dragCircle(Circle *circle, Point mouse){
    Point *center = &circle->center;
    
    center->x = mouse.x;
    center->y = mouse.y;
}

// function launching when mouse is pressed and moved
void drag(int x, int y){
    if(isMoving != -1){
        Point mouse;
        mouse.x = x;
        mouse.y = y;
        
	// move objects only if they are
	// within the screen
	if( (mouse.x <= WINDOW_WIDTH-1) && (mouse.x >= 0) &&
	    (mouse.y <= WINDOW_HEIGHT-1) && (mouse.y >= 0) ){
        	dragCircle(&circles[isMoving], mouse);
        	glutPostRedisplay();
	}
    }
}

// returns number of the clicked circle
// otherwise -1
int checkIfAnyCircleIsClicked(Point mouse){
    // object holding coordinates of circle center
    Point center;
    // radius of actual circle
    double radius;
    
    for(int i = 0; i < 5; ++i){
        // get radius of actual circle
        radius = circles[i].radius;
        // get coordinates of actual circle
        center = circles[i].center;
        
        // check if the mouse is within the circle
        double distance = sqrt( ((double)center.x - (double)mouse.x)*((double)center.x - (double)mouse.x) +
                                ((double)center.y - (double)mouse.y)*((double)center.y - (double)mouse.y) );
        
        if(distance <= radius){
            return i;
        }
    }
    
    return -1;
}

// marks which circle is dragging
// by setting global 'isMoving' variable
// if none - sets -1
void mouseFunction(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            Point mouse;
            mouse.x = x;
            mouse.y = y;
            
            isMoving = checkIfAnyCircleIsClicked(mouse);
        }
        else if(state == GLUT_UP){
            isMoving = -1;
        }
    }
}

void initApplication(int argc, char **argv){
    for(int i = 0; i < 5; ++i){
        circles[i].radius = 7;
    }
    
    circles[0].center.x = 256;
    circles[0].center.y = 156;
    circles[1].center.x = 161;
    circles[1].center.y = 225;
    circles[2].center.x = 197;
    circles[2].center.y = 337;
    circles[3].center.x = 315;
    circles[3].center.y = 337;
    circles[4].center.x = 351;
    circles[4].center.y = 225;
        
    
    // set that no object is dragging
    isMoving = -1;
    
    glutInit(&argc, argv);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH)-WINDOW_WIDTH)/2,
                           (glutGet(GLUT_SCREEN_HEIGHT)-WINDOW_HEIGHT)/2);
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("Bezier curve");
    GLSetup(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutDisplayFunc(display);
    glutMouseFunc(mouseFunction);
    glutMotionFunc(drag);
    glutMainLoop();
}

#endif
