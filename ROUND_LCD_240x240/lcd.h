#pragma once

#include <QGraphicsItem>
#include <QImage>
#include <array>

using Color = QColor;

class DisplayItem : public QGraphicsItem {
public:
    enum {
        Width = 800,
        Height = 480
    };

    explicit DisplayItem();

    Color pixel(int x, int y) const;

    void setPixel(int x, int y, Color color);

    void setPixel(int x, int y);

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

    void setDbg(bool fl) { pixmap = &(fl ? pixmapDbg : pixmapLcd); }

    //private:
    QImage pixmapLcd { Width, Height, QImage::Format_RGB888 };
    QImage pixmapDbg { Width, Height, QImage::Format_RGBA8888 };

    QImage* pixmap = &pixmapLcd;

    const QRect size { 0, 0, Width, Height };

    Color currentColor_[256];
    int index_ {};
};

inline DisplayItem LCD;

struct LCDDBG {
    LCDDBG(bool clear) {
        LCD.setDbg(1);
        if (clear)
            LCD.clear();
    }
    ~LCDDBG() { LCD.setDbg(0); }
};
