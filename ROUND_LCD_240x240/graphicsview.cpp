#include "graphicsview.h"

#include <QMouseEvent>

#include "../lvgl/lvgl.h"

constexpr auto inScaleFactor { 1.5 };
constexpr auto outScaleFactor { 1 / inScaleFactor };

static uint16_t touchX, touchY;
bool touched;
void my_touchpad_read(lv_indev_drv_t* indev_driver, lv_indev_data_t* data) {
    if (!touched) {
        data->state = LV_INDEV_STATE_REL;
    } else {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;
    }
}

GraphicsView::GraphicsView(QWidget* parent)
    : QGraphicsView { parent } {
    setScene(new QGraphicsScene());
    scene()->addItem(&LCD);
    scene()->setBackgroundBrush(Qt::black);
    setInteractive(true);
    setMouseTracking(true);
    setTabletTracking(true);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    auto indev = lv_indev_drv_register(&indev_drv);
    lv_indev_enable(indev, true);
//    lv_timer_t timer;
//    timer.period = 10;
//    timer.timer_cb = ;

//    auto timer = lv_timer_create(lv_indev_read_timer_cb, 10, NULL);

}

void GraphicsView::zoomFit() { fitInView(scene()->items().front(), Qt::KeepAspectRatio); }

void GraphicsView::zoom100() { setTransform({}); }

void GraphicsView::zoom200() { setTransform({}), scale(2, 2); }

void GraphicsView::zoomIn() { scale(inScaleFactor, inScaleFactor); }

void GraphicsView::zoomOut() { scale(outScaleFactor, outScaleFactor); }

void GraphicsView::setGridVisible(bool fl) {
    grid = fl;
    updateScene({ LCD.boundingRect() });
}

void GraphicsView::drawForeground(QPainter* painter, const QRectF& /*rect*/) {
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

void GraphicsView::mousePressEvent(QMouseEvent* event) {
    qDebug(__FUNCTION__);
    setDragMode(ScrollHandDrag);
    QGraphicsView::mousePressEvent(event);
    auto pt { mapToScene(event->pos()).toPoint() };
    touched = true;
    touchX = pt.x(), touchY = pt.y();
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event) {
    qDebug(__FUNCTION__);
    QGraphicsView::mouseReleaseEvent(event);
    setDragMode(NoDrag);
    auto pt { mapToScene(event->pos()).toPoint() };
    touched = false;
    touchX = pt.x(), touchY = pt.y();
}

void GraphicsView::mouseMoveEvent(QMouseEvent* event) {
    QGraphicsView::mouseMoveEvent(event);
    auto pt { mapToScene(event->pos()).toPoint() };
    touchX = pt.x(), touchY = pt.y();
}
