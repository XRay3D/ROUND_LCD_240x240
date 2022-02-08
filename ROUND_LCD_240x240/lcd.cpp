#include "lcd.h"

#include "../lvgl/lvgl.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>

constexpr auto k = 0.5;

Color toColor(lv_color_t c) {
    switch (LV_COLOR_DEPTH) {
    case 1:
        return Color(c.ch.red * 255, c.ch.green * 255, c.ch.blue * 255);
    case 8:
        return Color(c.ch.red << 5, c.ch.green << 5, c.ch.blue << 6);
    case 16:
        return Color(c.ch.red << 3, c.ch.green << 2, c.ch.blue << 3);
    case 32:
        return Color(c.ch.red, c.ch.green, c.ch.blue);
    default:
        return Qt::magenta;
    }
}

//UG_RESULT DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
//{
//    x1 > x2 ? std::swap(x1, x2) : void();
//    y1 > y2 ? std::swap(y1, y2) : void();
//    LCD.setCurrentColor(0, toColor(c));
//    if (x1 == x2)
//        LCD.drawVLine(x1, y1, y2 - y1 + 1);
//    else if (y1 == y2)
//        LCD.drawHLine(x1, y1, x2 - x1 + 1);
//    else
//        return UG_RESULT_FAIL;

//    return UG_RESULT_OK;
//}

//UG_S16 XB, XE, YB, YE;
//UG_S16 X, Y;

//void pushPixels(UG_U16 count, UG_COLOR color)
//{
//    auto c = toColor(color);
//    while (count--) {
//        LCD.setPixel(X, Y, c);
//        if (++X > XE) {
//            X = XB;
//            if (++Y > YE)
//                Y = YB;
//        }
//    }
//}

//auto Fill(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR c)
//{
//    LCD.fill(x1, y1, x2, y2, toColor(c));
//    return pushPixels;
//}

//auto FillArea(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2)
//{
//    X = XB = x1, XE = x2, Y = YB = y1, YE = y2;
//    return pushPixels;
//}

//void DrawImage(UG_S16 x1, UG_S16 y1, UG_BMP* bmp)
//{
//    UG_COLOR* c = (UG_COLOR*)bmp->p;
//    for (int y {}; y < bmp->height; ++y)
//        for (int x {}; x < bmp->width; ++x)
//            LCD.setPixel(x1 + x, y1 + y, toColor(*c++));
//}

//void DrawPixel(int16_t x, int16_t y, uint16_t color)
//{
//    LCD.setPixel(x, y, toColor(color));
//}

//UG_DEVICE device
//{
//#if __cplusplus < 202000
//    DisplayItem::Width,
//        DisplayItem::Height,
//        DrawPixel,
//        nullptr,
//#else
//    .x_dim = DisplayItem::Width,
//    .y_dim = DisplayItem::Height,
//    .pset = DrawPixel,
//    .flush = nullptr,
//#endif
//};

//UG_GUI gui {};

void my_flush_cb(lv_disp_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p) {
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to implemented by you.*/
    int32_t x, y;
    for (y = area->y1; y <= area->y2; ++y) {
        for (x = area->x1; x <= area->x2; ++x) {
            //put_px(x, y, *color_p);
            LCD.setPixel(x, y, toColor(*color_p));
            color_p++;
        }
    }

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}

//void my_gpu_fill_cb(lv_disp_drv_t* disp_drv, lv_color_t* dest_buf, const lv_area_t* dest_area, const lv_area_t* fill_area, lv_color_t color) {
//    /*It's an example code which should be done by your GPU*/
//    uint32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/

//    for (y = fill_area->y1; y < fill_area->y2; y++) {
//        for (x = fill_area->x1; x < fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf += dest_width; /*Go to the next line*/
//    }
//}

//void my_rounder_cb(lv_disp_drv_t* disp_drv, lv_area_t* area) {
//    /* Update the areas as needed.
//     * For example it makes the area to start only on 8th rows and have Nx8 pixel height.*/
//    area->y1 = area->y1 & 0x07;
//    area->y2 = (area->y2 & 0x07) + 8;
//}

//void my_set_px_cb(lv_disp_drv_t* disp_drv, uint8_t* buf, lv_coord_t buf_w, lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa) {
//    /* Write to the buffer as required for the display.
//     * For example it writes only 1-bit for monochrome displays mapped vertically.*/
//    buf += buf_w * (y >> 3) + x;
//    if (lv_color_brightness(color) > 128)
//        (*buf) |= (1 << (y % 8));
//    else
//        (*buf) &= ~(1 << (y % 8));
//}

//void my_monitor_cb(lv_disp_drv_t* disp_drv, uint32_t time, uint32_t px) {
//    qDebug("%d px refreshed in %d ms\n", time, ms);
//}

//void my_clean_dcache_cb(lv_disp_drv_t* disp_drv, uint32) {
//    /* Example for Cortex-M (CMSIS) */
//    SCB_CleanInvalidateDCache();
//}

DisplayItem::DisplayItem()
    : QGraphicsItem() {

    lv_init();
    /*A static or global variable to store the buffers*/
    static lv_disp_draw_buf_t disp_buf;

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[Width * 10];
    static lv_color_t buf_2[Width * 10];

    /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, Width * 10);

    static lv_disp_drv_t disp_drv;   /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);     /*Basic initialization*/
    disp_drv.draw_buf = &disp_buf;   /*Set an initialized buffer*/
    disp_drv.flush_cb = my_flush_cb; /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = Width;        /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = Height;       /*Set the vertical resolution in pixels*/
    disp_drv.dpi = 150;

    lv_disp_t* disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    //    UG_Init(&gui, &device);
    //    UG_DriverRegister(DRIVER_DRAW_LINE, (void*)DrawLine);
    //    UG_DriverRegister(DRIVER_FILL_FRAME, (void*)Fill);
    //    UG_DriverRegister(DRIVER_FILL_AREA, (void*)FillArea);
    //    UG_DriverRegister(DRIVER_DRAW_BMP, (void*)DrawImage);
    //    UG_FontSetHSpace(0);
    //    UG_FontSetVSpace(0);
    clear();
}

Color DisplayItem::pixel(int x, int y) const {
    return pixmap->pixelColor(x, y);
}

void DisplayItem::setPixel(int x, int y, Color color) {
    pixmap->setPixelColor(x, y, color);
    update();
}

void DisplayItem::setPixel(int x, int y) {
    pixmap->setPixelColor(x, y, currentColor_[index_]);
    update();
}

void DisplayItem::clear() {
    pixmap->fill(pixmap == &pixmapLcd ? Qt::black : Qt::transparent);
    update();
}

void DisplayItem::drawHLine(uint16_t x, uint16_t y, uint8_t lenght) { //NOTE DMA frendly
    if (!lenght)
        return;
    while (lenght--)
        pixmap->setPixelColor(x++, y, currentColor_[index_]);
    update();
}

void DisplayItem::drawVLine(uint16_t x, uint16_t y, uint8_t lenght) { //NOTE DMA frendly
    if (!lenght)
        return;
    while (lenght--)
        pixmap->setPixelColor(x, y++, currentColor_[index_]);
    update();
}

void DisplayItem::fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, Color c) {
    for (int x { x1 }; x <= x2; ++x)
        for (int y { y1 }; y <= y2; ++y)
            pixmap->setPixelColor(x, y, c);
    update();
}

QRectF DisplayItem::boundingRect() const {
    return size + QMarginsF { k, k, k, k };
}

void DisplayItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/) {
    painter->drawImage(pixmapLcd.rect(), pixmapLcd);
    //    painter->drawImage(pixmapDbg.rect(), pixmapDbg);

    //    static auto pixmap_ = scene()->addPixmap(QPixmap::fromImage(pixmapLcd));
    //    pixmap_->setZValue(1000);
    //    pixmap_->setPixmap(QPixmap::fromImage(pixmapLcd));

    if constexpr (1) { //border
        painter->setPen(QColor(127, 127, 127, 127));
        painter->drawRect(size + QMarginsF { k, k, k, k });
        //        painter->drawEllipse(size + QMarginsF { k, k, k, k });
    }
}

void DisplayItem::setCurrentColor(int index, const Color& newCurrentColor) {
    currentColor_[index_ = index] = newCurrentColor;
}

void DisplayItem::setCurrentColor(int index) {
    index_ = index;
}
