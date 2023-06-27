
#include "Arduino.h"
#include "shape.hh"

void editPoint(Point* p, double x, double y){
  p->x = x; p->y = y;
}

Shape* newShape(byte numPoints, double x, double y, bool connectAllPoints, uint16_t color){
  Point *points[numPoints]; 
  for (byte i; i < numPoints; i++){
    points[i] = malloc(sizeof(Point));
  }

  Shape* s = malloc(sizeof(byte) + 2*sizeof(double) + sizeof(uint16_t) + sizeof(bool) + sizeof(points));

  s->numOfPoints = numPoints; //byte
  s->centerX = x; //double
  s->centerY = y; //double
  s->color = color; //uint16_t
  s->connectAllPoints = connectAllPoints; //bool
  memmove(s->points, points, sizeof(points)); //points

  return s;
}

void destroyShape(Shape* s){
  for(byte i = 0; i < s->numOfPoints; i++) free(s->points[i]);
  free(s);
}

void translateShape(Shape* s, double x0, double y0, double x1, double y1){
  // matrix multiplication of all points in the shape s by 
  // x0 y0 v x
  // x1 y1 ^ y
  //
  // newX = x0*x + y0*y
  // newX = x1*x + y1*y
  for(byte i = 0; i < s->numOfPoints; i++){
    double newX = x0*s->points[i]->x + y0*s->points[i]->y;
    double newY = x1*s->points[i]->x + y1*s->points[i]->y;
    s->points[i]->x = newX;
    s->points[i]->y = newY;    
  }
}

void moveShape(Shape* s, double x, double y){
  s->centerX += x; s->centerY += y;  
}



double distanceBetweenPoints(Point* subjectPoint, Point* fromPoint){
  //pythoagoras thoerem
  //use this to detect "collisions" between sprites cause im too lazy to write program to detect if point inside shape
  return sqrt(pow((subjectPoint->x - fromPoint->x),2)+pow((subjectPoint->y - fromPoint->y),2));
}
