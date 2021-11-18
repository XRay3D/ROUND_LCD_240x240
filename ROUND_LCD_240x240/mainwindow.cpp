#include "mainwindow.h"
#include "arcprogressbar.h"
#include "circle.h"
#include "lcd.h"
#include "test.h"
#include "ui_mainwindow.h"

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

extern "C" void HAL_Delay(uint32_t d)
{
    QEventLoop loop;
    QTimer::singleShot(d, [&loop] { loop.exit(); });
    loop.exec();
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    {
        ui->graphicsView->setScene(new QGraphicsScene());
        ui->graphicsView->scene()->addItem(&LCD);
        ui->graphicsView->scene()->setBackgroundBrush(Qt::black);
        QTimer::singleShot(100, [this] {
            ui->graphicsView->fitInView(ui->graphicsView->scene()->items().front(), Qt::KeepAspectRatio);
            ST7789_Test();
        });
    }

    connect(new QShortcut(Qt::Key_A, this), &QShortcut::activated, [this] { LCD.clear(), ui->horizontalSlider->setValue(ui->horizontalSlider->value() - 1); });
    connect(new QShortcut(Qt::Key_D, this), &QShortcut::activated, [this] { LCD.clear(), ui->horizontalSlider->setValue(ui->horizontalSlider->value() + 1); });

    auto toolBar = addToolBar("Zoom");
    toolBar->addAction("Fit", [this] { resizeEvent({}); });
    toolBar->addAction("100%", [this] { ui->graphicsView->setTransform({}); });

    initArcs();

    loadSettings();

    //startTimer(1);
}

MainWindow::~MainWindow()
{
    //    ui->graphicsView->scene()->removeItem(&LCD);// NOTE because scene aftyer destroy delete LCD item alocatet on stack.
    saveSettings();
    delete ui;
    exit(0);
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

uint16_t R = 120;
uint16_t K = 40;

ArcProgressBar ark[] {
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
};

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

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    ui->graphicsView->fitInView(ui->graphicsView->scene()->items().front(), Qt::KeepAspectRatio);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) { ark[0].setValue(value); }
void MainWindow::on_horizontalSlider_2_valueChanged(int value) { ark[1].setValue(value); }
void MainWindow::on_horizontalSlider_3_valueChanged(int value) { ark[2].setValue(value); }

void MainWindow::timerEvent(QTimerEvent* /*event*/)
{
    LCD.clear();
    ui->horizontalSlider->setValue((ui->horizontalSlider->value() + 1) % 180);
    // static std::default_random_engine e1({});
    // static std::uniform_int_distribution<int> uniform_dist(0, 180);
    // static int mean;
    // mean = uniform_dist(e1) * 0.05 + mean * 0.95;
    // ui->horizontalSlider_2->setValue(mean);

    // ark[0].setValue(int(ark[0].value() + 1) % 180);
    // ark[1].setValue(int(ark[0].value() + 2) % 180);
    // ark[2].setValue(int(ark[0].value() + 3) % 180);
}
