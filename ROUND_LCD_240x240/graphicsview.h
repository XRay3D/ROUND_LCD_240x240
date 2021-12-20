#pragma once

#include "lcd.h"
#include <QGraphicsView>

class GraphicsView : public QGraphicsView {
    bool grid {};

public:
    GraphicsView(QWidget* parent = nullptr);

    void zoomFit();
    void zoom100();
    void zoom200();
    void zoomIn();
    void zoomOut();
    void setGridVisible(bool fl);

    // QGraphicsView interface
protected:
    void drawForeground(QPainter* painter, const QRectF& rect) override;

    // QWidget interface
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    //    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
};
