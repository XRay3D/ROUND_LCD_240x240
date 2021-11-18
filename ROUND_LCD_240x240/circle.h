#pragma once

#include "lcd.h"
#include <cinttypes>

class BresenhamCircle {
    int32_t D;
    uint32_t curX;
    uint32_t curY;

public:
    BresenhamCircle(uint16_t radius)
        : D(3 - (radius << 1))
        , curX {}
        , curY { radius } {
    }

    void setRadius(uint16_t radius) { *this = { radius }; }

    void operator()() {
        if (D < 0) {
            D += (curX << 2) + 6;
        } else {
            D += ((curX - curY) << 2) + 10;
            curY--;
        }
        curX++;
    }

    operator bool() { return curX <= curY; }
    operator PointU16() { return PointU16(curX, curY); }

    uint32_t x() { return curX; };
    uint32_t y() { return curY; };
};

inline void DrawCircle(uint16_t X, uint16_t Y, uint16_t Radius) {
    BresenhamCircle calc { Radius };
    while (calc) {
        LCD.setPixel(X + calc.x(), Y + calc.y());
        LCD.setPixel(X + calc.x(), Y - calc.y());
        LCD.setPixel(X + calc.y(), Y + calc.x());
        LCD.setPixel(X + calc.y(), Y - calc.x());
        LCD.setPixel(X - calc.x(), Y + calc.y());
        LCD.setPixel(X - calc.x(), Y - calc.y());
        LCD.setPixel(X - calc.y(), Y + calc.x());
        LCD.setPixel(X - calc.y(), Y - calc.x());
        calc();
    }
}

inline void FillCircle(uint16_t X, uint16_t Y, uint16_t Radius) {
    BresenhamCircle calc { Radius };
    while (calc) {
        if (calc.y() > 0) {
            LCD.drawHLine(X - calc.y(), Y + calc.x(), 2 * calc.y());
            LCD.drawHLine(X - calc.y(), Y - calc.x(), 2 * calc.y());
        }
        if (calc.x() > 0) {
            LCD.drawHLine(X - calc.x(), Y - calc.y(), 2 * calc.x());
            LCD.drawHLine(X - calc.x(), Y + calc.y(), 2 * calc.x());
        }
        calc();
    }

    // DrawCircle(X, Y, Radius);
}

inline void DoRoundTicks(int j, int k) {
    // double a = std::atan2(m_sinA, m_normals[k].X * m_normals[j].X + m_normals[k].Y * m_normals[j].Y);
    // int steps = std::max((int)Round(m_StepsPerRad * std::fabs(a)), 1);
    // m_srcPoly.reserve(m_srcPoly.size() + steps + 1);
    // double X = m_normals[k].X, Y = m_normals[k].Y, X2;
    // for (int i = 0; i < steps; ++i) {
    // m_destPoly.push_back(IntPoint(
    // Round(m_srcPoly[j].X + X * m_delta),
    // Round(m_srcPoly[j].Y + Y * m_delta)));
    // X2 = X;
    // X = X * m_cos - m_sin * Y;
    // Y = X2 * m_sin + Y * m_cos;
    // }
    // m_destPoly.push_back(IntPoint(
    // Round(m_srcPoly[j].X + m_normals[j].X * m_delta),
    // Round(m_srcPoly[j].Y + m_normals[j].Y * m_delta)));
}
