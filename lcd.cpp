#include "lcd.h"

#include <QPainter>
constexpr auto k = 0.5;


DisplayItem::DisplayItem()
    : QGraphicsItem() {
    instance = this;
}

QRectF DisplayItem::boundingRect() const {
    return size + QMarginsF { k, k, k, k };
}

void DisplayItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    //    painter->fillRect(size + QMargins { k, k, k, k }, Qt::transparent);
    painter->translate(0.5, 0.5);

    painter->setPen(QColor(255, 127, 0));
    for (int x = 0; x < size.width(); ++x) {
        for (int y = 0; y < size.height(); ++y) {
            painter->setPen(pixmap[x][y]);
            //            if (pixmap[x][y] == QColor {})
            painter->drawPoint(x, y);
        }
    }
    painter->translate(-0.5, -0.5);

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
