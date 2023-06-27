#ifndef SHAPE_H
#define SHAPE_H

typedef struct Point {
  double x;
  double y;
};

void editPoint(Point* p, double x, double y);

typedef struct Shape {
  byte numOfPoints;
  double centerX;
  double centerY;
  uint16_t color;  
  bool connectAllPoints;
  Point* points[];
};

Shape* newShape(byte numPoints, double x, double y, bool connectAllPoints, uint16_t color);

void destroyShape(Shape* s);

void translateShape(Shape* s, double x0, double y0, double x1, double y1);

void moveShape(Shape*s, double x, double y);



double distanceBetweenPoints(Point* subjectPoint, Point* fromPoint);

#endif