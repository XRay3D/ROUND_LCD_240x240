#pragma once

#include "point.h"
#include <QGraphicsItem>
#include <array>

using Color = QColor;

class DisplayItem : public QGraphicsItem {
public:
    enum {
        Width = 240,
        Height = 240
    };

    explicit DisplayItem();

    Color pixel(int x, int y) const;

    void setPixel(int x, int y, Color color);

    void setPixel(int x, int y);
    void setPixel(PointU16 pt);

    void clear();

    void drawHLine(uint16_t x, uint16_t y, uint8_t lenght);
    void drawVLine(uint16_t x, uint16_t y, uint8_t lenght);

    void fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color c);

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //    const Color& currentColor() const { return currentColor_; }
    void setCurrentColor(int index, const Color& newCurrentColor);
    void setCurrentColor(int index);

private:
    std::array<std::array<Color, Height>, Width> pixmap {};
    const QRect size { 0, 0, Width, Height };

    Color currentColor_[256];
    int index_ {};
};

inline DisplayItem LCD;
