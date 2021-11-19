#pragma once

#include "lcd.h"
#include <cinttypes>

class Line {
    PointU16 p1;
    PointU16 p2;

public:
    Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
        : p1 { x1, y1 }
        , p2 { x2, y2 } { }
    Line(PointU16 p1, PointU16 p2)
        : p1 { p1 }
        , p2 { p2 } { }

    void operator()() { draw(); }

    void draw() {

        int16_t deltax = abs(p2.x() - p1.x()); /* The difference between the x's */
        int16_t deltay = abs(p2.y() - p1.y()); /* The difference between the y's */
        int16_t x = p1.x(); /* Start x off at the first pixel */
        int16_t y = p1.y(); /* Start y off at the first pixel */

        int16_t xinc1;
        int16_t xinc2;
        int16_t yinc1;
        int16_t yinc2;

        int16_t den;
        int16_t num;
        int16_t numadd;
        int16_t numpixels;

        if (p2.x() >= p1.x()) { /* The x-values are increasing */
            xinc1 = 1;
            xinc2 = 1;
        } else { /* The x-values are decreasing */
            xinc1 = -1;
            xinc2 = -1;
        }

        if (p2.y() >= p1.y()) { /* The y-values are increasing */
            yinc1 = 1;
            yinc2 = 1;
        } else { /* The y-values are decreasing */
            yinc1 = -1;
            yinc2 = -1;
        }

        if (deltax >= deltay) { /* There is at least one x-value for every y-value */
            xinc1 = 0; /* Don't change the x when numerator >= denominator */
            yinc2 = 0; /* Don't change the y for every iteration */
            den = deltax;
            num = deltax / 2;
            numadd = deltay;
            numpixels = deltax; /* There are more x-values than y-values */
        } else { /* There is at least one y-value for every x-value */
            xinc2 = 0; /* Don't change the x for every iteration */
            yinc1 = 0; /* Don't change the y when numerator >= denominator */
            den = deltay;
            num = deltay / 2;
            numadd = deltax;
            numpixels = deltay; /* There are more y-values than x-values */
        }

        for (int16_t curpixel = 0; curpixel <= numpixels; curpixel++) {

            LCD.setPixel(x, y); /* Draw the current pixel */
            num += numadd; /* Increase the numerator by the top of the fraction */
            if (num >= den) /* Check if numerator >= denominator */
            {
                num -= den; /* Calculate the new numerator value */
                x += xinc1; /* Change the x as appropriate */
                y += yinc1; /* Change the y as appropriate */
            }
            x += xinc2; /* Change the x as appropriate */
            y += yinc2; /* Change the y as appropriate */
        }
    }
};
