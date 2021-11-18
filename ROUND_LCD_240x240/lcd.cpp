#include "lcd.h"

#include <QDebug>
#include <QPainter>

extern "C" {
#include "ugui.h"
}

constexpr auto k = 0.5;

Color toColor(UG_COLOR c)
{
#pragma pack(push, 1)
    union {
        UG_COLOR c;
        struct {
            unsigned b : 5;
            unsigned g : 6;
            unsigned r : 5;
        };
    } comp { .c = c };
#pragma pack(pop)
    return Color(comp.r << 3, comp.g << 2, comp.b << 3);
}

UG_RESULT DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
#undef swap
    x1 > x2 ? std::swap(x1, x2) : void();
    y1 > y2 ? std::swap(y1, y2) : void();
    LCD.setCurrentColor(0, toColor(c));
    if (x1 == x2)
        LCD.drawVLine(x1, y1, y2 - y1 + 1);
    else if (y1 == y2)
        LCD.drawHLine(x1, y1, x2 - x1 + 1);
    else
        return UG_RESULT_FAIL;

    return UG_RESULT_OK;
}

UG_S16 XB, XE, YB, YE;
UG_S16 X, Y;

void pushPixels(UG_U16 count, UG_COLOR color)
{
    auto c = toColor(color);
    while (count--) {
        LCD.setPixel(X, Y, c);
        if (++X > XE) {
            X = XB;
            if (++Y > YE) {
                Y = YB;
            }
        }
    }
}

auto Fill(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
{
    LCD.fill(x1, y1, x2, y2, toColor(c));
    return pushPixels;
}

auto FillArea(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2)
{
    X = XB = x1, XE = x2, Y = YB = y1, YE = y2;
    return pushPixels;
}

void DrawImage(UG_S16 x1, UG_S16 y1, UG_BMP* bmp)
{
    UG_COLOR* c = (UG_COLOR*)bmp->p;
    for (int y {}; y < bmp->height; ++y) {
        for (int x {}; x < bmp->width; ++x) {
            LCD.setPixel(x1 + x, y1 + y, toColor(*c++));
        }
    }
}

void DrawPixel(int16_t x, int16_t y, uint16_t color)
{
    LCD.setPixel(x, y, toColor(color));
}

UG_DEVICE device {
    .x_dim = DisplayItem::Width,
    .y_dim = DisplayItem::Height,
    .pset = DrawPixel,
    .flush = nullptr,
};

UG_GUI gui {};

DisplayItem::DisplayItem()
    : QGraphicsItem()
{
    UG_Init(&gui, &device);
    UG_DriverRegister(DRIVER_DRAW_LINE, (void*)DrawLine);
    UG_DriverRegister(DRIVER_FILL_FRAME, (void*)Fill);
    UG_DriverRegister(DRIVER_FILL_AREA, (void*)FillArea);
    UG_DriverRegister(DRIVER_DRAW_BMP, (void*)DrawImage);
    UG_FontSetHSpace(0);
    UG_FontSetVSpace(0);
}

Color DisplayItem::pixel(int x, int y) const
{
    return pixmap.pixelColor(x, y);
}

void DisplayItem::setPixel(int x, int y, Color color)
{
    pixmap.setPixelColor(x, y, color);
    update();
}

void DisplayItem::setPixel(int x, int y)
{
    pixmap.setPixelColor(x, y, currentColor_[index_]);
    update();
}

void DisplayItem::setPixel(PointU16 pt)
{
    pixmap.setPixelColor(pt.x(), pt.y(), currentColor_[index_]);
    update();
}

void DisplayItem::clear()
{
    pixmap.fill(Qt::black);
    update();
}

void DisplayItem::drawHLine(uint16_t x, uint16_t y, uint8_t lenght)
{ //NOTE DMA frendly
    if (!lenght)
        return;
    while (lenght--)
        pixmap.setPixelColor(x++, y, currentColor_[index_]);
    update();
}

void DisplayItem::drawVLine(uint16_t x, uint16_t y, uint8_t lenght)
{ //NOTE DMA frendly
    if (!lenght)
        return;
    while (lenght--)
        pixmap.setPixelColor(x, y++, currentColor_[index_]);
    update();
}

void DisplayItem::fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color c)
{
    for (int x { x1 }; x <= x2; ++x)
        for (int y { y1 }; y <= y2; ++y)
            pixmap.setPixelColor(x, y, c);
    update();
}

QRectF DisplayItem::boundingRect() const
{
    return size + QMarginsF { k, k, k, k };
}

void DisplayItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->drawImage(pixmap.rect(), pixmap);

    if constexpr (1) { //border
        painter->setPen(QColor(127, 127, 127, 127));
        painter->drawRect(size + QMarginsF { k, k, k, k });
        painter->drawEllipse(size + QMarginsF { k, k, k, k });
    }

    if constexpr (0) { //grid
        painter->setPen({ QColor(127, 127, 127, 127), 0.0 });
        for (int x = 0; x < size.width(); ++x)
            painter->drawLine(x, 0, x, size.height());
        for (int y = 0; y < size.height(); ++y)
            painter->drawLine(0, y, size.width(), y);
    }
}

void DisplayItem::setCurrentColor(int index, const Color& newCurrentColor)
{
    currentColor_[index_ = index] = newCurrentColor;
}

void DisplayItem::setCurrentColor(int index)
{
    index_ = index;
}
