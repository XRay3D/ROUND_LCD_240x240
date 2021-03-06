#pragma once

#include "circle.h"
#include "lcd.h"
#include "line.h"

struct ArcProgressBarAbstr {
    virtual uint16_t radO() const = 0;
    virtual uint16_t radI() const = 0;
};

template <uint16_t RadO, uint16_t RadI>
struct ArcProgressBarPar final : ArcProgressBarAbstr {
    uint16_t radO() const override { return RadO; };
    uint16_t radI() const override { return RadI; };
};

class ArcProgressBar {
    PointU16 center;
    const uint16_t radO; //Radius Outer
    const uint16_t radI; //Radius Inner
    float min_ {};
    float max_ { 1.f };
    float value_ {};
    uint8_t valColorId_;
    uint8_t basColorId_;

    float map(float val);

public:
    ArcProgressBar(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Width)
        : center { X, Y }
        , radO { Radius }
        , radI { Radius -= Width } {
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
