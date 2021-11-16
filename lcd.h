#pragma once

#include "point.h"
#include <QGraphicsItem>
#include <array>

using Color = QColor;

class DisplayItem : public QGraphicsItem {
    enum {
        Width = 240,
        Height = 240
    };
    std::array<std::array<Color, Height>, Width> pixmap {};
    const QRect size { 0, 0, Width, Height };

    Color currentColor_[256];
    int index_ {};

public:
    static inline DisplayItem* instance;

    explicit DisplayItem();

    Color pixel(int x, int y) const {
        return pixmap[x][y];
    }

    void setPixel(int x, int y, Color color) {
        pixmap[x][y] = color;
        update();
    }

    void setPixel(int x, int y) {
        pixmap[x][y] = currentColor_[index_];
        update();
    }
    void setPixel(PointU16 pt) {
        pixmap[pt.x()][pt.y()] = currentColor_[index_];
        update();
    }

    void clear() {
        pixmap = {};
        update();
    }

    void drawHLine(uint16_t x, uint16_t y, uint8_t lenght) { //NOTE DMA frendly
        if (!lenght)
            return;
        while (lenght--)
            pixmap[x++][y] = currentColor_[index_];
        update();
    };

    void drawVLine(uint16_t x, uint16_t y, uint8_t lenght) { //NOTE DMA frendly
        if (!lenght)
            return;
        while (lenght--)
            pixmap[x][y++] = currentColor_[index_];
        update();
    };

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    //    const Color& currentColor() const { return currentColor_; }
    void setCurrentColor(int index, const Color& newCurrentColor) { currentColor_[index_ = index] = newCurrentColor; }
    void setCurrentColor(int index) { index_ = index; }
};

inline DisplayItem LCD;
