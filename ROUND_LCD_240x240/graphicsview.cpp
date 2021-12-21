#include "graphicsview.h"

#include <QMouseEvent>

#include "ugui.h"

constexpr auto inScaleFactor { 1.5 };
constexpr auto outScaleFactor { 1 / inScaleFactor };

GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView { parent }
{
    setScene(new QGraphicsScene());
    scene()->addItem(&LCD);
    scene()->setBackgroundBrush(Qt::black);
    setInteractive(true);
    setMouseTracking(true);
    setTabletTracking(true);
}

void GraphicsView::zoomFit() { fitInView(scene()->items().front(), Qt::KeepAspectRatio); }

void GraphicsView::zoom100() { setTransform({}); }

void GraphicsView::zoom200() { setTransform({}), scale(2, 2); }

void GraphicsView::zoomIn() { scale(inScaleFactor, inScaleFactor); }

void GraphicsView::zoomOut() { scale(outScaleFactor, outScaleFactor); }

void GraphicsView::setGridVisible(bool fl)
{
    grid = fl;
    updateScene({ LCD.boundingRect() });
}

void GraphicsView::drawForeground(QPainter* painter, const QRectF& /*rect*/)
{
    if (!grid)
        return;
    painter->setPen({ QColor(127, 127, 127, 127), 0.0 });

    int bottom = LCD.boundingRect().bottom();
    int left = LCD.boundingRect().left();
    int right = LCD.boundingRect().right();
    int top = LCD.boundingRect().top();

    for (int x = left; x < right; ++x)
        painter->drawLine(x, top, x, bottom);

    for (int y = top; y < bottom; ++y)
        painter->drawLine(left, y, right, y);
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
    setDragMode(ScrollHandDrag);
    QGraphicsView::mousePressEvent(event);
    auto pt { mapToScene(event->pos()).toPoint() };
    UG_TouchUpdate(std::clamp(pt.rx(), 0, 799), std::clamp(pt.ry(), 0, 479), 1);
    UG_Update();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    setDragMode(NoDrag);
    auto pt { mapToScene(event->pos()).toPoint() };
    UG_TouchUpdate(std::clamp(pt.rx(), 0, 799), std::clamp(pt.ry(), 0, 479), 0);
    UG_Update();
}

//void GraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
//{
//    QGraphicsView::mouseDoubleClickEvent(event);
//}

void GraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    QGraphicsView::mouseMoveEvent(event);
    auto pt { mapToScene(event->pos()).toPoint() };
    UG_TouchUpdate(std::clamp(pt.rx(), 0, 799), std::clamp(pt.ry(), 0, 479), UG_GetGUI()->touch.state);
    UG_Update();
}
