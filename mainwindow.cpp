#include "mainwindow.h"
#include "displayitem.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QTimer>
#include <QToolBar>
#include <cmath>
#include <random>

DisplayItem* lcd;

template <class T>
T ABS(T val) { return val < 0 ? -val : val; }

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);
    {
        ui->graphicsView->setScene(new QGraphicsScene());
        ui->graphicsView->scene()->addItem(lcd = new DisplayItem);
        ui->graphicsView->scene()->setBackgroundBrush(Qt::black);
        QTimer::singleShot(100, [this] {
            ui->graphicsView->fitInView(ui->graphicsView->scene()->items().front(), Qt::KeepAspectRatio);
        });
    }

    auto toolBar = addToolBar("Zoom");
    toolBar->addAction("Fit", [this] { resizeEvent({}); });
    toolBar->addAction("100%", [this] { ui->graphicsView->setTransform({}); });

    initArcs();

    loadSettings();

    startTimer(1);
}

MainWindow::~MainWindow() {
    saveSettings();
    delete ui;
}

void MainWindow::saveSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
}

void delay() {
    return;
    QEventLoop loop;
    QTimer::singleShot(1, Qt::PreciseTimer, [&loop] { loop.exit(0); });
    loop.exec();
}

void MainWindow::loadSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
}

class CalcBrezCircle {
    int32_t D;
    uint32_t curX;
    uint32_t curY;

public:
    CalcBrezCircle(uint16_t radius)
        : D(3 - (radius << 1))
        , curX {}
        , curY { radius } { }

    void setRadius(uint16_t radius) { *this = { radius }; }

    void operator()() {
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

void DrawCircle(uint16_t X, uint16_t Y, uint16_t Radius) {
    CalcBrezCircle calc { Radius };

    while (calc) {
        lcd->setPixel((X + calc.x()), (Y + calc.y()) /*, Qt::white*/);
        lcd->setPixel((X + calc.x()), (Y - calc.y()) /*, Qt::white*/);
        lcd->setPixel((X + calc.y()), (Y + calc.x()) /*, Qt::white*/);
        lcd->setPixel((X + calc.y()), (Y - calc.x()) /*, Qt::white*/);
        lcd->setPixel((X - calc.x()), (Y + calc.y()) /*, Qt::white*/);
        lcd->setPixel((X - calc.x()), (Y - calc.y()) /*, Qt::white*/);
        lcd->setPixel((X - calc.y()), (Y + calc.x()) /*, Qt::white*/);
        lcd->setPixel((X - calc.y()), (Y - calc.x()) /*, Qt::white*/);

        calc();
    }
}

void FillCircle(uint16_t X, uint16_t Y, uint16_t Radius) {

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

    //    DrawCircle(X, Y, Radius);
}

void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t* buf = nullptr) {
    //    int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
    //            yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
    //            curpixel = 0;

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

    if (deltax >= deltay) /* There is at least one x-value for every y-value */
    {
        xinc1 = 0; /* Don't change the x when numerator >= denominator */
        yinc2 = 0; /* Don't change the y for every iteration */
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax; /* There are more x-values than y-values */
    } else /* There is at least one y-value for every x-value */
    {
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
    const uint16_t X, Y, Radius, Radius2;
    float min_ {};
    float max_ { 1.f };
    float value_ {};

    bool top { true };

    float map(float val) { return (val - min_) * (0 - -180) / (max_ - min_) + -180; }

public:
    FillPieArc(uint16_t X, uint16_t Y, uint16_t Radius, uint16_t Radius2)
        : X { X }
        , Y { Y }
        , Radius { Radius }
        , Radius2 { Radius2 } {
    }

    void pie() {
        CalcBrezCircle calc { Radius };
        if (top) {
            while (calc) {
                if (calc.y() > 0)
                    lcd->drawHLine(X - calc.y(), Y - calc.x(), 2 * calc.y());
                if (calc.x() > 0)
                    lcd->drawHLine(X - calc.x(), Y - calc.y(), 2 * calc.x());
                calc();
            }
        } else {
            while (calc) {
                if (calc.y() > 0)
                    lcd->drawHLine(X - calc.y(), Y + calc.x(), 2 * calc.y());
                if (calc.x() > 0)
                    lcd->drawHLine(X - calc.x(), Y + calc.y(), 2 * calc.x());

                calc();
            }
        }
    }

    void draw() {
        uint16_t buf[256] {};
        uint16_t XV;
        uint16_t YV;
        {
            auto val = map(value_);
            XV = X + cos(val * (M_PI / 180)) * Radius;
            YV = X + sin(val * (M_PI / 180)) * Radius;
        }

        DrawLine(X, Y, XV, YV, buf);

        if (!Radius2) {
            pie();
        } else {
            uint16_t innerArc[256] {};
            if (top) {

                CalcBrezCircle calc { Radius2 };
                while (calc) {
                    if (calc.y() > 0)
                        innerArc[calc.x()] = calc.y();
                    if (calc.x() > 0)
                        innerArc[calc.y()] = calc.x();
                    calc();
                }
                calc = CalcBrezCircle { Radius };

                auto draw = [&](uint16_t x, uint16_t y, uint16_t x2, uint16_t y2) {
                    if (innerArc[y2]) {
                        uint16_t xr = innerArc[y2] + X;
                        if (X <= XV) { // left full
                            lcd->setCurrentColor(valueColor_);
                            lcd->drawHLine(x, y, x2 - innerArc[y2]);
                        } else { // right clear
                            lcd->setCurrentColor(baseColor_);
                            lcd->drawHLine(xr, y, x2 - innerArc[y2]);
                        }

                        if (X > XV && y > YV) { // left val
                            lcd->setCurrentColor(valueColor_);
                            uint16_t len = std::min<uint16_t>(x2 - innerArc[y2], buf[y] - x);
                            lcd->drawHLine(x, y, len);
                            lcd->setCurrentColor(baseColor_);
                            lcd->drawHLine(x + len, y, x2 - innerArc[y2] - len);
                            return;
                        } else if (X > XV) { // left fill
                            lcd->setCurrentColor(baseColor_);
                            lcd->drawHLine(x, y, x2 - innerArc[y2]);
                            return;
                        } else if (X < XV) { // right val
                            lcd->setCurrentColor(valueColor_);
                            uint16_t len = std::min<uint16_t>(x2 - innerArc[y2], abs(buf[y] - xr));
                            if (buf[y] && buf[y] < xr) {
                                lcd->setCurrentColor(baseColor_);
                                lcd->drawHLine(xr, y, x2 - innerArc[y2]);
                            } else {
                                lcd->drawHLine(xr, y, len);
                                lcd->setCurrentColor(baseColor_);
                                lcd->drawHLine(xr + len, y, x2 - innerArc[y2] - len);
                            }
                            return;
                        } else {
                            lcd->setCurrentColor(baseColor_);
                            lcd->drawHLine(xr, y, x2 - innerArc[y2]);
                            return;
                        }

                    } else if (x <= XV) {
                        lcd->setCurrentColor(valueColor_);
                        uint16_t len = std::min<uint16_t>(x2 * 2, buf[y] - x);
                        lcd->drawHLine(x, y, len);
                        lcd->setCurrentColor(baseColor_);
                        lcd->drawHLine(x + len, y, x2 * 2 - len);
                    } else {
                        lcd->setCurrentColor(baseColor_);
                        lcd->drawHLine(x, y, x2 * 2);
                    }
                };

                while (calc) {
                    if (calc.y() > 0)
                        draw(X - calc.y(), Y - calc.x(), calc.y(), calc.x());
                    if (calc.x() > 0)
                        draw(X - calc.x(), Y - calc.y(), calc.x(), calc.y());
                    calc();
                }
            } else {
            }
        }
    }

    float value() const { return value_; }
    void setValue(float newValue) {
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
    void setRange(float newMin, float newMax) {
        min_ = newMin;
        max_ = newMax;
    }
};

void DoRoundTicks(int j, int k) {
    //    double a = std::atan2(m_sinA, m_normals[k].X * m_normals[j].X + m_normals[k].Y * m_normals[j].Y);
    //    int steps = std::max((int)Round(m_StepsPerRad * std::fabs(a)), 1);
    //    m_srcPoly.reserve(m_srcPoly.size() + steps + 1);
    //    double X = m_normals[k].X, Y = m_normals[k].Y, X2;
    //    for (int i = 0; i < steps; ++i) {
    //        m_destPoly.push_back(IntPoint(
    //            Round(m_srcPoly[j].X + X * m_delta),
    //            Round(m_srcPoly[j].Y + Y * m_delta)));
    //        X2 = X;
    //        X = X * m_cos - m_sin * Y;
    //        Y = X2 * m_sin + Y * m_cos;
    //    }
    //    m_destPoly.push_back(IntPoint(
    //        Round(m_srcPoly[j].X + m_normals[j].X * m_delta),
    //        Round(m_srcPoly[j].Y + m_normals[j].Y * m_delta)));
}

int R = 120;
int K = 20;

FillPieArc ark[] {
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
    { 120, 120, static_cast<uint16_t>(R), static_cast<uint16_t>((R -= K) + 5) },
};

void MainWindow::initArcs() {
    ark[0].setValueColor(Qt::red);
    ark[1].setValueColor(Qt::green);
    ark[2].setValueColor(Qt::blue);
    for (auto&& ark : ark) {
        ark.setBaseColor(Qt::darkGray);
        ark.setRange(0, 180);
        ark.setValue(0); // first draw
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    ui->graphicsView->fitInView(ui->graphicsView->scene()->items().front(), Qt::KeepAspectRatio);
}

void MainWindow::on_horizontalSlider_valueChanged(int value) { ark[0].setValue(value); }
void MainWindow::on_horizontalSlider_2_valueChanged(int value) { ark[1].setValue(value); }
void MainWindow::on_horizontalSlider_3_valueChanged(int value) { ark[2].setValue(value); }

void MainWindow::timerEvent(QTimerEvent* /*event*/) {
    ui->horizontalSlider->setValue((ui->horizontalSlider->value() + 1) % 180);
    static std::default_random_engine e1({});
    static std::uniform_int_distribution<int> uniform_dist(0, 180);
    static int mean;
    mean = uniform_dist(e1) * 0.05 + mean * 0.95;
    ui->horizontalSlider_2->setValue(mean);

    //    ark[0].setValue(int(ark[0].value() + 1) % 180);
    //    ark[1].setValue(int(ark[0].value() + 2) % 180);
    //    ark[2].setValue(int(ark[0].value() + 3) % 180);
}
