#include "mainwindow.h"
#include "displayitem.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QShortcut>
#include <QTimer>
#include <QToolBar>
#include <cmath>
#include <random>

DisplayItem* lcd;

template <class T>
T ABS(T val) { return val < 0 ? -val : val; }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    {
        ui->graphicsView->setScene(new QGraphicsScene());
        ui->graphicsView->scene()->addItem(lcd = new DisplayItem);
        ui->graphicsView->scene()->setBackgroundBrush(Qt::black);
        QTimer::singleShot(100, [this] {
            ui->graphicsView->fitInView(ui->graphicsView->scene()->items().front(), Qt::KeepAspectRatio);
        });
    }

    connect(new QShortcut(Qt::Key_A, this), &QShortcut::activated, [this] { lcd->clear(), ui->horizontalSlider->setValue(ui->horizontalSlider->value() - 1); });
    connect(new QShortcut(Qt::Key_D, this), &QShortcut::activated, [this] { lcd->clear(), ui->horizontalSlider->setValue(ui->horizontalSlider->value() + 1); });

    auto toolBar = addToolBar("Zoom");
    toolBar->addAction("Fit", [this] { resizeEvent({}); });
    toolBar->addAction("100%", [this] { ui->graphicsView->setTransform({}); });

    initArcs();

    loadSettings();

    //startTimer(1);
}

MainWindow::~MainWindow()
{
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

void delay()
{
    return;
    QEventLoop loop;
    QTimer::singleShot(1, Qt::PreciseTimer, [&loop] { loop.exit(0); });
    loop.exec();
}

void MainWindow::loadSettings()
{
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
}

struct Point16 {
    int16_t curX;
    int16_t curY;
};

class CalcBrezCircle {
    int32_t D;
    uint32_t curX;
    uint32_t curY;

public:
    CalcBrezCircle(uint16_t radius)
        : D(3 - (radius << 1))
        , curX {}
        , curY { radius }
    {
    }

    void setRadius(uint16_t radius) { *this = { radius }; }

    void operator()()
    {
        if (D < 0) {
            D += (curX << 2) + 6;
        } else {
            D += ((curX - curY) << 2) + 10;
            curY--;
        }
        curX++;
    }

    operator bool() { return curX <= curY; }

    uint32_t x() { return curX; };
    uint32_t y() { return curY; };
};

void DrawCircle(uint16_t X, uint16_t Y, uint16_t Radius)
{
    CalcBrezCircle calc { Radius };

    while (calc) {
        lcd->setPixel((X + calc.x()), (Y + calc.y()));
        lcd->setPixel((X + calc.x()), (Y - calc.y()));
        lcd->setPixel((X + calc.y()), (Y + calc.x()));
        lcd->setPixel((X + calc.y()), (Y - calc.x()));
        lcd->setPixel((X - calc.x()), (Y + calc.y()));
        lcd->setPixel((X - calc.x()), (Y - calc.y()));
        lcd->setPixel((X - calc.y()), (Y + calc.x()));
        lcd->setPixel((X - calc.y()), (Y - calc.x()));

        calc();
    }
}

void FillCircle(uint16_t X, uint16_t Y, uint16_t Radius)
{

    CalcBrezCircle calc { Radius };

    while (calc) {
        if (calc.y() > 0) {
            lcd->drawHLine(X - calc.y(), Y + calc.x(), 2 * calc.y());
            lcd->drawHLine(X - calc.y(), Y - calc.x(), 2 * calc.y());
        }

        if (calc.x() > 0) {
            lcd->drawHLine(X - calc.x(), Y - calc.y(), 2 * calc.x());
            lcd->drawHLine(X - calc.x(), Y + calc.y(), 2 * calc.x());
        }

        calc();
    }

    // DrawCircle(X, Y, Radius);
}

void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* buf = nullptr)
{
    // int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    // yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    // curpixel = 0;

    int16_t deltax = ABS(x2 - x1); /* The difference between the x's */
    int16_t deltay = ABS(y2 - y1); /* The difference between the y's */
    int16_t x = x1; /* Start x off at the first pixel */
    int16_t y = y1; /* Start y off at the first pixel */

    int16_t xinc1;
    int16_t xinc2;
    int16_t yinc1;
    int16_t yinc2;

    int16_t den;
    int16_t num;
    int16_t numadd;
    int16_t numpixels;

    if (x2 >= x1) { /* The x-values are increasing */
        xinc1 = 1;
        xinc2 = 1;
    } else { /* The x-values are decreasing */
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) { /* The y-values are increasing */
        yinc1 = 1;
        yinc2 = 1;
    } else { /* The y-values are decreasing */
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) { /* There is at least one x-value for every y-value */
        xinc1 = 0; /* Don't change the x when numerator >= denominator */
        yinc2 = 0; /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; /* There are more x-values than y-values */
    } else { /* There is at least one y-value for every x-value */
        xinc2 = 0; /* Don't change the x for every iteration */
        yinc1 = 0; /* Don't change the y when numerator >= denominator */
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay; /* There are more y-values than x-values */
    }

    for (int16_t curpixel = 0; curpixel <= numpixels; curpixel++) {
        if (buf)
            buf[y] = x;
        else
            lcd->setPixel(x, y); /* Draw the current pixel */
        num += numadd; /* Increase the numerator by the top of the fraction */
        if (num >= den) /* Check if numerator >= denominator */
        {
            num -= den; /* Calculate the new numerator value */
            x += xinc1; /* Change the x as appropriate */
            y += yinc1; /* Change the y as appropriate */
        }
        x += xinc2; /* Change the x as appropriate */
        y += yinc2; /* Change the y as appropriate */
    }
}

class FillPieArc {
    Color valueColor_;
    Color baseColor_;
    const uint16_t xc, //center
        yc, //center
        radO, //Radius Outer
        radI; //Radius Inner
    float min_ {};
    float max_ { 1.f };
    float value_ {};

    float map(float val) { return (val - min_) * (0 - -180) / (max_ - min_) + -180; }

public:
    FillPieArc(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Radius2)
        : xc { X }
        , yc { Y }
        , radO { Radius }
        , radI { Radius2 }
    {
    }

    void pie()
    {
        CalcBrezCircle calc { radO };
        while (calc) {
            if (calc.y())
                lcd->drawHLine(xc - calc.y(), yc - calc.x(), 2 * calc.y());
            if (calc.x())
                lcd->drawHLine(xc - calc.x(), yc - calc.y(), 2 * calc.x());
            calc();
        }
    }

    void draw()
    {
        uint16_t buf[256] {};
        uint16_t xo;
        uint16_t yo;
        uint16_t xi;
        uint16_t yi;
        {
            auto val = map(value_);
            auto sin_ = sin(val * (M_PI / 180));
            auto cos_ = cos(val * (M_PI / 180));
            xo = xc + cos_ * radO;
            yo = yc + sin_ * radO;
            xi = xc + cos_ * radI;
            yi = yc + sin_ * radI;
        }

        DrawLine(xc, yc, xo, yo, buf);

        if (!radI) {
            pie();
        } else {
            uint16_t arcInX[256] {};

            CalcBrezCircle calc { radI };
            while (calc) {
                if (calc.y() > 0)
                    arcInX[calc.x()] = calc.y();
                if (calc.x() > 0)
                    arcInX[calc.y()] = calc.x();
                calc();
            }
            calc = CalcBrezCircle { radO };

            auto draw = [&](uint16_t x, uint16_t y) {
                uint16_t xd = xc - x, yd = yc - y;
                if (y <= radI) {
                    uint16_t len = x - arcInX[y];
                    if (xo < xc) {
                        // bottom left arc with inner

                        if (yd >= yi) {
                            lcd->setCurrentColor(Qt::red);
                            lcd->drawHLine(xd, yd, len);
                        } else if (yd <= yo) {
                            lcd->setCurrentColor(Qt::gray);
                            lcd->drawHLine(xd, yd, len);
                        } else if (buf[yd] >= xd) {
                            lcd->setCurrentColor(Qt::red);
                            uint16_t len2 = buf[yd] - xd;
                            lcd->drawHLine(xd, yd, len2);
                            if (len > len2) {
                                lcd->setCurrentColor(Qt::gray);
                                lcd->drawHLine(xd + len2, yd, len - len2);
                            }
                        }
                        xd = xc + arcInX[y];

                        // bottom right arc with inner
                        lcd->setCurrentColor(Qt::gray);
                        lcd->drawHLine(xd, yd, len);
                    } else {
                        // bottom left arc with inner
                        lcd->setCurrentColor(Qt::red);
                        lcd->drawHLine(xd, yd, len);
                        // bottom right arc with inner
                        xd = xc + arcInX[y];
                        if (yd <= yo) {
                            lcd->setCurrentColor(Qt::red);
                            lcd->drawHLine(xd, yd, len);
                        } else if (yd >= yi) {
                            lcd->setCurrentColor(Qt::gray);
                            lcd->drawHLine(xd, yd, len);
                        } else if (buf[yd] > xd) {
                            lcd->setCurrentColor(Qt::red);
                            uint16_t len2 = buf[yd] - xd;
                            lcd->drawHLine(xd, yd, len2);
                            if (len > len2) {
                                lcd->setCurrentColor(Qt::gray);
                                lcd->drawHLine(xd + len2, yd, len - len2);
                            }
                        }
                    }
                } else { // top arc without inner
                    x *= 2;
                    if (yo > yd) {
                        lcd->setCurrentColor(xo < xc ? Qt::gray : Qt::red);
                        lcd->drawHLine(xd, yd, x);
                    } else {
                        uint16_t len = 0;
                        if (buf[yd] > xd) {
                            len = buf[yd] - xd;
                            lcd->setCurrentColor(Qt::red);
                            lcd->drawHLine(xd, yd, len);
                        }
                        if (x > len) {
                            lcd->setCurrentColor(Qt::gray);
                            lcd->drawHLine(xd + len, yd, x - len);
                        }
                    }
                }
            };

            while (calc) {
                if (calc.y() > 0)
                    draw(calc.y(), calc.x());
                if (calc.x() > 0)
                    draw(calc.x(), calc.y());
                calc();
            }
        }
    }

    float
    value() const
    {
        return value_;
    }
    void setValue(float newValue)
    {
        value_ = newValue;
        draw();
    }

    void setBaseColor(const Color& newBaseColor) { baseColor_ = newBaseColor; }
    const Color& baseColor() const { return baseColor_; }

    void setValueColor(const Color& newValueColor) { valueColor_ = newValueColor; }
    const Color& valueColor() const { return valueColor_; }

    float max() const { return max_; }
    void setMax(float newMax) { max_ = newMax; }

    float min() const { return min_; }
    void setMin(float newMin) { min_ = newMin; }
    void setRange(float newMin, float newMax)
    {
        min_ = newMin;
        max_ = newMax;
    }
};

void DoRoundTicks(int j, int k)
{
    // double a = std::atan2(m_sinA, m_normals[k].X * m_normals[j].X + m_normals[k].Y * m_normals[j].Y);
    // int steps = std::max((int)Round(m_StepsPerRad * std::fabs(a)), 1);
    // m_srcPoly.reserve(m_srcPoly.size() + steps + 1);
    // double X = m_normals[k].X, Y = m_normals[k].Y, X2;
    // for (int i = 0; i < steps; ++i) {
    // m_destPoly.push_back(IntPoint(
    // Round(m_srcPoly[j].X + X * m_delta),
    // Round(m_srcPoly[j].Y + Y * m_delta)));
    // X2 = X;
    // X = X * m_cos - m_sin * Y;
    // Y = X2 * m_sin + Y * m_cos;
    // }
    // m_destPoly.push_back(IntPoint(
    // Round(m_srcPoly[j].X + m_normals[j].X * m_delta),
    // Round(m_srcPoly[j].Y + m_normals[j].Y * m_delta)));
}

uint16_t R = 120;
uint16_t K = 40;

FillPieArc ark[] {
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
};

void MainWindow::initArcs()
{
    ark[0].setValueColor(Qt::red);
    ark[1].setValueColor(Qt::green);
    ark[2].setValueColor(Qt::blue);
    for (auto&& ark : ark) {
        ark.setBaseColor(Qt::darkGray);
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
    lcd->clear();
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
