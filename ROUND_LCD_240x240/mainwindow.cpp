#include "mainwindow.h"
#include "arcprogressbar.h"
#include "circle.h"
#include "lcd.h"
#include "test.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QSettings>
#include <QShortcut>
#include <QTime>
#include <QTimer>
#include <QToolBar>
#include <cmath>
#include <random>

extern "C" uint32_t HAL_GetTick()
{
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

ArcProgressBar ark[] {
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(new QShortcut(Qt::Key_A, this), &QShortcut::activated, [this] { LCD.clear(), ui->hslArc1->setValue(ui->hslArc1->value() - 1); });
    connect(new QShortcut(Qt::Key_D, this), &QShortcut::activated, [this] { LCD.clear(), ui->hslArc1->setValue(ui->hslArc1->value() + 1); });

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
    initArcs();

    loadSettings();

    QTimer::singleShot(100, this, ST7789_Test);

    connect(ui->pbTest, &QAbstractButton::clicked, ST7789_Test);

    connect(ui->hslArc1, &QAbstractSlider::valueChanged, [](int value) { ark[0].setValue(value); });
    connect(ui->hslArc2, &QAbstractSlider::valueChanged, [](int value) { ark[1].setValue(value); });
    connect(ui->hslArc3, &QAbstractSlider::valueChanged, [](int value) { ark[2].setValue(value); });

    //startTimer(1);
}

MainWindow::~MainWindow()
{
    ui->graphicsView->scene()->removeItem(&LCD); // NOTE because scene aftyer destroy delete LCD item alocatet on stack.
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
}

void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
}

void MainWindow::initArcs()
{
    LCD.setCurrentColor(1, Qt::red);
    LCD.setCurrentColor(2, Qt::green);
    LCD.setCurrentColor(3, Qt::blue);
    LCD.setCurrentColor(0, Qt::darkGray);

    ark[0].setValColorId(1);
    ark[1].setValColorId(2);
    ark[2].setValColorId(3);
    for (auto&& ark : ark) {
        ark.setBasColorId(0);
        ark.setRange(0, 180);
        //        ark.setValue(0); // first draw
    }
}

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
    LCD.clear();
    ui->hslArc1->setValue((ui->hslArc1->value() + 1) % 180);
    // static std::default_random_engine e1({});
    // static std::uniform_int_distribution<int> uniform_dist(0, 180);
    // static int mean;
    // mean = uniform_dist(e1) * 0.05 + mean * 0.95;
    // ui->hslArc2->setValue(mean);

    // ark[0].setValue(int(ark[0].value() + 1) % 180);
    // ark[1].setValue(int(ark[0].value() + 2) % 180);
    // ark[2].setValue(int(ark[0].value() + 3) % 180);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    ui->graphicsView->zoomFit();
}

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    ui->graphicsView->zoomFit();
}
