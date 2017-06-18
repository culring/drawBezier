#ifndef BEZIER_H
#define BEZIER_H

#include <stdio.h>
#include <math.h>
#include <GL/glu.h>
#include "point.h"

Point getBezierPoint( Point* points, int numPoints, float t ) {
    Point tmp[numPoints];
    for(int i = 0; i < numPoints; ++i){
        tmp[i] = points[i];
    }
    
    int i = numPoints - 1;
    while (i > 0) {
        for (int k = 0; k < i; k++)
            tmp[k] = addPoints( tmp[k], multiplyByScalar(t, subtractPoints(tmp[k+1], tmp[k])) );
        i--;
    }
    
    Point answer = tmp[0];
    
    return answer;
}

void asmDrawBezier(float *points, float step, void *pixelArray, int width);

#endif