#include "arcprogressbar.h"
#include <cmath>

float ArcProgressBar::map(float val) { return (val - min_) * (0 - -180) / (max_ - min_) + -180; }

void ArcProgressBar::pie() {
    BresenhamCircle calc { radO };
    while (calc) {
        if (calc.y())
            LCD.drawHLine(center.x() - calc.y(), center.y() - calc.x(), 2 * calc.y());
        if (calc.x())
            LCD.drawHLine(center.x() - calc.x(), center.y() - calc.y(), 2 * calc.x());
        calc();
    }
}

void ArcProgressBar::draw() {
    uint16_t buf[std::max(LCD.Width, LCD.Height)] {};
    PointS16 outPt;
    PointS16 inPt;
    {
        auto val = map(value_);
        auto sin_ = sin(val * (M_PI / 180.));
        auto cos_ = cos(val * (M_PI / 180.));
        outPt = Point(center.x() + cos_ * radO, center.y() + sin_ * radO);
        inPt = Point(center.x() + cos_ * radI, center.y() + sin_ * radI);
        float scale;
        auto delta = inPt - outPt;
        if (delta.y()) {
            scale = delta.x() / float(delta.y());
            qDebug() << delta;
            qDebug("%f", scale);
            LCDDBG dbg(1);
            for (int y = 0; y <= delta.y(); ++y) {
                buf[outPt.y() + y] = outPt.x() + y * scale;
                LCD.setPixel(buf[outPt.y() + y], outPt.y() + y, { 0, 255, 0, 200 });
            }
        } else {
            buf[outPt.y()] = std::max(inPt.x(), outPt.x());
            LCD.setPixel(buf[outPt.y()], outPt.y(), { 0, 255, 0, 200 });
        }
    }

    if (!radI) {
        pie();
    } else {
        uint16_t arcInX[std::max(LCD.Width, LCD.Height)] {};
        BresenhamCircle calc { radI };
        while (calc) {
            if (calc.y())
                arcInX[calc.x()] = calc.y();
            if (calc.x())
                arcInX[calc.y()] = calc.x();
            calc();
        }
        calc = { radO };
        auto draw = [&](uint16_t x, uint16_t y) {
            uint16_t xOuter = center.x() - x, yd = center.y() - y;
            if (y <= radI) {

                const uint16_t len = x - arcInX[y];

                auto drawCap = [len, &xOuter, yd, xCap = buf[yd], this]() {
                    uint16_t len2 {};
                    if (xCap >= xOuter)
                        len2 = std::min<uint16_t>(xCap - xOuter + 1, len);
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xOuter, yd, len2);
                    if (len >= len2) {
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xOuter + len2, yd, len - len2);
                    }
                };

                if (outPt.x() < center.x()) { // left
                    /*  */ if (yd > inPt.y()) { // bot
                        LCD.setCurrentColor(valColorId_);
                        LCD.drawHLine(xOuter, yd, len);
                    } else if (yd <= outPt.y()) { // top
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xOuter, yd, len);
                    } else if (buf[yd] >= xOuter) {
                        drawCap();
                    }
                    xOuter = center.x() + arcInX[y];
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xOuter, yd, len);
                } else { //rigth
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xOuter, yd, len);
                    xOuter = center.x() + arcInX[y];
                    /*  */ if (yd < outPt.y()) { // top
                        LCD.setCurrentColor(valColorId_);
                        LCD.drawHLine(xOuter, yd, len);
                    } else if (yd > inPt.y()) { // bot
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xOuter, yd, len);
                    } else { // if (buf[yd] >= xOuter) {
                        drawCap();
                    }
                }
            } else { // top arc without inner
                x *= 2;
                if (outPt.y() > yd) {
                    LCD.setCurrentColor(outPt.x() < center.x() ? basColorId_ : valColorId_);
                    LCD.drawHLine(xOuter, yd, x);
                } else if (buf[yd] > xOuter) {
                    uint16_t len = std::min<uint16_t>(buf[yd] - xOuter + 1, x);
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xOuter, yd, len);
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xOuter + len, yd, x - len);
                } else {
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xOuter, yd, x);
                }
            }
        };

        while (calc) {
            if (calc.y())
                draw(calc.y(), calc.x());
            if (calc.x())
                draw(calc.x(), calc.y());
            calc();
        }
    }
}

float ArcProgressBar::value() const { return value_; }

void ArcProgressBar::setValue(float newValue) {
    value_ = newValue;
    draw();
}

float ArcProgressBar::max() const { return max_; }

void ArcProgressBar::setMax(float newMax) { max_ = newMax; }

float ArcProgressBar::min() const { return min_; }

void ArcProgressBar::setMin(float newMin) { min_ = newMin; }

void ArcProgressBar::setRange(float newMin, float newMax) { min_ = newMin, max_ = newMax; }

uint8_t ArcProgressBar::basColorId() const { return basColorId_; }

void ArcProgressBar::setBasColorId(uint8_t newBasColorId) { basColorId_ = newBasColorId; }

uint8_t ArcProgressBar::valColorId() const { return valColorId_; }

void ArcProgressBar::setValColorId(uint8_t newValColorId) { valColorId_ = newValColorId; }
