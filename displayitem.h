#pragma once

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

    Color currentColor_ {};

public:
    static inline DisplayItem* instance;

    explicit DisplayItem();

    Color pixel(int x, int y) const
    {
        return pixmap[x][y];
    }

    void setPixel(int x, int y, Color color)
    {
        pixmap[x][y] = color;
        update();
    }

    void setPixel(int x, int y)
    {
        pixmap[x][y] = currentColor_;
        update();
    }

    void clear()
    {
        pixmap = {};
        update();
    }

    void drawHLine(uint16_t x, uint16_t y, uint8_t lenght)
    { //NOTE DMA frendly
        if (!lenght)
            return;
        while (lenght--)
            pixmap[x++][y] = currentColor_;
        update();
    };

    void drawVLine(uint16_t x, uint16_t y, uint8_t lenght)
    { //NOTE DMA frendly
        if (!lenght)
            return;
        while (lenght--)
            pixmap[x][y++] = currentColor_;
        update();
    };

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    const Color& currentColor() const { return currentColor_; }
    void setCurrentColor(const Color& newCurrentColor) { currentColor_ = newCurrentColor; }
};
