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

// number of points determining Bezier curve
int numberOfPoints;

// is bezier drawn by asm disabled
// (and drawing by c started)
int isAsmDisabled;

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
    for(int i = 0; i < numberOfPoints; ++i){
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

void drawBezier(float step){
    Point points[numberOfPoints];
    
    for(int i = 0; i < numberOfPoints; ++i){
        points[i] = circles[i].center;
    }
    
    glLoadIdentity();
    glColor3ub(0, 192, 192);
    glLineWidth(4);
    glBegin(GL_LINE_STRIP);
    for(double t = 0; t <= 1; t += step){
        Point tmp = getBezierPoint(points, numberOfPoints, t);
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
    
    if(numberOfPoints >= 2){
        if(isAsmDisabled){
            // draw Bezier curve using C function
            drawBezier((float)1/POINTS_PER_BEZIER);
        }
        else{
            // draw bezier with asm
            drawAsmBezier((float)1/POINTS_PER_BEZIER);
        }
        // draw lines between circles
        drawLineStrip();
    }
    
    // draw circles
    for(int i = 0; i < numberOfPoints; ++i){
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
                // if it is the last circle, change as well
                // coordiantes of the rest inivisible circles
                for(int i = isMoving; i < 5; ++i){
                    dragCircle(&circles[isMoving], mouse);
                }
                
                // set flag that screen need to be
                // redisplayed
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
    
    for(int i = 0; i < numberOfPoints; ++i){
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

void addPoint(Point mouse){
    for(int i = numberOfPoints; i < 5; ++i){
        circles[i].center = mouse;
    }
    ++numberOfPoints;
}

void addPointByCoords(int x, int y){
    for(int i = numberOfPoints; i < 5; ++i){
        circles[i].center.x = x;
        circles[i].center.y = y;
    }
    ++numberOfPoints;
}

void addPointToBegin(Point mouse){
    for(int i = numberOfPoints; i >= 1; --i){
        circles[i].center = circles[i-1].center;
    }
    circles[0].center = mouse;
    ++numberOfPoints;
}

float distance(Point a, Point b){
    return sqrt( (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) );
}

// marks which circle is dragging
// by setting global 'isMoving' variable
// if none - sets -1
void mouseFunction(int button, int state, int x, int y){
    Point mouse;
    mouse.x = x;
    mouse.y = y;
    
    if(button == GLUT_LEFT_BUTTON){
        if(state == GLUT_DOWN){
            int clickedCircle = checkIfAnyCircleIsClicked(mouse);
            
            if(clickedCircle != -1){
                isMoving = clickedCircle;
            }
            else{
                if(numberOfPoints < 5){
                    // if the are at least two points
                    // and the mouse is closer to the first point
                    if(numberOfPoints >= 2 &&
                       distance(mouse, circles[0].center) < distance(mouse, circles[numberOfPoints-1].center)){
                            addPointToBegin(mouse);
                    }
                    else{
                        addPoint(mouse);
                    }
                        
                    glutPostRedisplay();
                }
            }
        }
        else if(state == GLUT_UP){
            isMoving = -1;
        }
    }
    
    if(button == GLUT_RIGHT_BUTTON){
        if(state == GLUT_DOWN){
                int clickedCircle = checkIfAnyCircleIsClicked(mouse);
                if(clickedCircle != -1){
                    // if it is the last point in the path
                    // and there is more than one point
                    if(clickedCircle == numberOfPoints - 1 &&
                       numberOfPoints > 1){
                        // move all the points from the end
                        for(int i = clickedCircle; i < 5; ++i){
                            circles[i].center = circles[clickedCircle-1].center;
                        }
                    }
                    else{
                        for(int i = clickedCircle; i < numberOfPoints - 1; ++i){
                            circles[i] = circles[i+1];
                        }
                    }
                    --numberOfPoints;
                }
                else{
                    // change drawing bezier curve mode
                    // (between asm and c function)
                    isAsmDisabled = !isAsmDisabled;
                }
                
                glutPostRedisplay();
        }
    }
}

void initApplication(int argc, char **argv){
    for(int i = 0; i < 5; ++i){
        circles[i].radius = 7;
    }
    
    addPointByCoords(256, 128);
    addPointByCoords(134, 216);
    addPointByCoords(181, 360);
    addPointByCoords(331, 360);
    addPointByCoords(378, 216);
    
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
