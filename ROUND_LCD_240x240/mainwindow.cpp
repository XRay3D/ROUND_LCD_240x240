#include "mainwindow.h"
#include "lcd.h"
#include "test.h"
#include "ui_mainwindow.h"

#include "screen/screen.h"
#include "../lvgl/demos/benchmark/lv_demo_benchmark.h"
#include "lvgl.h"

#include <QAction>
#include <QDebug>
#include <QSettings>
#include <QShortcut>
#include <QTime>
#include <QTimer>
#include <QToolBar>
#include <cmath>
#include <random>

//TestWindow testWindow;

extern "C" uint32_t HAL_GetTick() {
    return QTime::currentTime().msecsSinceStartOfDay();
}

extern "C" void HAL_Delay(uint32_t d) // NOTE Without blocing GUI Thread
{
    QEventLoop loop;
    QTimer::singleShot(d, [&loop] { loop.exit(); });
    loop.exec();
}

uint16_t R = 120;
uint16_t K = 40;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(new QShortcut(Qt::Key_A, this), &QShortcut::activated, [this] { ui->hslArc1->setValue(ui->hslArc1->value() - 1); }); // NOTE debug without mouse ;-)
    connect(new QShortcut(Qt::Key_D, this), &QShortcut::activated, [this] { ui->hslArc1->setValue(ui->hslArc1->value() + 1); }); // NOTE debug without mouse ;-)
    connect(new QShortcut(Qt::Key_E, this), &QShortcut::activated, [this] { ui->hslArc1->setValue(ui->hslArc1->maximum()); });   // NOTE debug without mouse ;-)

    auto toolBar = addToolBar("Zoom");

    auto action = toolBar->addAction("Fit", ui->graphicsView, &GraphicsView::zoomFit);
    action = toolBar->addAction("100%", ui->graphicsView, &GraphicsView::zoom100);
    action = toolBar->addAction("200%", ui->graphicsView, &GraphicsView::zoom200);
    action = toolBar->addAction("Zoom In", ui->graphicsView, &GraphicsView::zoomIn);
    action->setShortcut(QKeySequence::ZoomIn);
    action = toolBar->addAction("Zoom Out", ui->graphicsView, &GraphicsView::zoomOut);
    action->setShortcut(QKeySequence::ZoomOut);

    toolBar = addToolBar("Tools");
    action = toolBar->addAction("Grid", ui->graphicsView, &GraphicsView::setGridVisible);
    action->setCheckable(true);
    action->setShortcut(Qt::Key_G);

    loadSettings();

    //QTimer::singleShot(100, this, ST7789_Test);

    connect(ui->pbTest, &QAbstractButton::clicked, ST7789_Test);
    connect(ui->pushButton, &QAbstractButton::clicked, [] {
        //        testWindow.show();
    });

    //    connect(ui->hslArc1, &QAbstractSlider::valueChanged, [](int value) { ark[0].setValue(value); });
    //    connect(ui->hslArc2, &QAbstractSlider::valueChanged, [](int value) { ark[1].setValue(value); });
    //    connect(ui->hslArc3, &QAbstractSlider::valueChanged, [](int value) { ark[2].setValue(value); });
    //lv_demo_widgets();
    ::screen();
    startTimer(16);
    //    testWindow.show();
}

MainWindow::~MainWindow() {
    ui->graphicsView->scene()->removeItem(&LCD); // NOTE because scene aftyer destroy delete LCD item alocatet on stack.
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
}

void MainWindow::loadSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
}

void MainWindow::timerEvent(QTimerEvent* /*event*/) {
    lv_tick_inc(16);
    lv_timer_handler(); /* let the GUI do its work */
    //ui->label->setPixmap(QPixmap::fromImage(LCD.pixmapLcd));
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    ui->graphicsView->zoom100();
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    ui->graphicsView->zoom100();
}
