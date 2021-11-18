#include "arcprogressbar.h"
#include <cmath>


float ArcProgressBar::map(float val) { return (val - min_) * (0 - -180) / (max_ - min_) + -180; }

void ArcProgressBar::pie()
{
    BresenhamCircle calc { radO };
    while (calc) {
        if (calc.y())
            LCD.drawHLine(center.x() - calc.y(), center.y() - calc.x(), 2 * calc.y());
        if (calc.x())
            LCD.drawHLine(center.x() - calc.x(), center.y() - calc.y(), 2 * calc.x());
        calc();
    }
}

void ArcProgressBar::draw()
{
    uint16_t buf[256] {};
    PointU16 o;
    PointU16 i;
    {
        auto val = map(value_);
        auto sin_ = sin(val * (M_PI / 180));
        auto cos_ = cos(val * (M_PI / 180));
        o.rx() = center.x() + cos_ * radO;
        o.ry() = center.y() + sin_ * radO;
        i.rx() = center.x() + cos_ * radI;
        i.ry() = center.y() + sin_ * radI;
    }

    Line(i, o)(buf);

    if (!radI) {
        pie();
    } else {
        uint16_t arcInX[256] {};
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
            uint16_t xd = center.x() - x, yd = center.y() - y;
            if (y <= radI) {
                const uint16_t len = x - arcInX[y];
                auto drawCap = [len, &xd, yd, X = buf[yd], this]() {
                    const uint16_t len2 = X - xd;
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xd, yd, len2);
                    if (len > len2) {
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xd + len2, yd, len - len2);
                    }
                };

                if (o.x() < center.x()) {
                    if (yd >= i.y()) {
                        LCD.setCurrentColor(valColorId_);
                        LCD.drawHLine(xd, yd, len);
                    } else if (yd <= o.y()) {
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xd, yd, len);
                    } else if (buf[yd] >= xd) {
                        drawCap();
                    }
                    xd = center.x() + arcInX[y];
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xd, yd, len);
                } else {
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xd, yd, len);
                    xd = center.x() + arcInX[y];
                    if (yd < o.y()) {
                        LCD.setCurrentColor(valColorId_);
                        LCD.drawHLine(xd, yd, len);
                    } else if (yd >= i.y()) {
                        LCD.setCurrentColor(basColorId_);
                        LCD.drawHLine(xd, yd, len);
                    } else if (buf[yd] > xd) {
                        drawCap();
                    }
                }
            } else { // top arc without inner
                x *= 2;
                if (o.y() > yd) {
                    LCD.setCurrentColor(o.x() < center.x() ? basColorId_ : valColorId_);
                    LCD.drawHLine(xd, yd, x);
                } else if (buf[yd] > xd) {
                    uint16_t len = std::min<uint16_t>(buf[yd] - xd, x);
                    LCD.setCurrentColor(valColorId_);
                    LCD.drawHLine(xd, yd, len);
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xd + len, yd, x - len);
                } else {
                    LCD.setCurrentColor(basColorId_);
                    LCD.drawHLine(xd, yd, x);
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
        //        LCD.setCurrentColor(Qt::green);
        //        Line(i, o)();
    }
}

float ArcProgressBar::value() const { return value_; }

void ArcProgressBar::setValue(float newValue)
{
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
