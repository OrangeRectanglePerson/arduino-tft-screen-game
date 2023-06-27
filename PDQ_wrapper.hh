#ifndef PDQ_WRAPPER_H
#define PDQ_WRAPPER_H

void drawShapeRespectToCenter(PDQ_ST7735* screen, Shape* s, double centerPtX, double centerPtY, bool clearScreen);

void drawShape(PDQ_ST7735* screen, Shape* s, bool clearScreen);

void drawSprite(PDQ_ST7735* screen, Sprite* s, bool clearScreen);

#endif