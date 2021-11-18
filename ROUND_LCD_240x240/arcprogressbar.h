#pragma once

#include "circle.h"
#include "lcd.h"
#include "line.h"

class ArcProgressBar {
    PointU16 center;
    const uint16_t radO; //Radius Outer
    const uint16_t radI; //Radius Inner
    float min_ {};
    float max_ { 1.f };
    float value_ {};
    float map(float val);
    uint8_t valColorId_;
    uint8_t basColorId_;

public:
    constexpr ArcProgressBar(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Radius2)
        : center { X, Y }
        , radO { Radius }
        , radI { Radius2 } {
    }

    void pie();

    void draw();

    float
    value() const;
    void setValue(float newValue);

    float max() const;
    void setMax(float newMax);

    float min() const;
    void setMin(float newMin);
    void setRange(float newMin, float newMax);
    uint8_t valColorId() const;
    void setValColorId(uint8_t newValColorId);
    uint8_t basColorId() const;
    void setBasColorId(uint8_t newBasColorId);
};
