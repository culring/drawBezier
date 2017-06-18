#ifndef POINT_H
#define POINT_H

// structure representing point
typedef struct{
    float x, y;
} Point;

Point addPoints(Point a, Point b){
    Point result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    
    return result;
}

Point subtractPoints(Point a, Point b){
    Point result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    
    return result;
}

Point multiplyByScalar(double s, Point a){
    Point result;
    result.x = s * a.x;
    result.y = s * a.y;
    
    return result;
}

#endif