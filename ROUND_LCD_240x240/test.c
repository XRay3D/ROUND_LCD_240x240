#include "test.h"
#include "images.h"
#include "stdio.h"
#include "ugui.h"

extern uint32_t HAL_GetTick();
extern void HAL_Delay(uint32_t d);

#define DEFAULT_FONT FONT_arial_20X23 //FONT_12X20

enum {
    MAX_OBJECTS = 10,
    ST7789_WIDTH = 240,
    ST7789_HEIGHT = 240,
};

UG_WINDOW window_1 = { .style = WND_STYLE_2D };
UG_BUTTON button_1 = { .style = WND_STYLE_2D };
UG_TEXTBOX textbox_1 = { .style = WND_STYLE_2D };
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
UG_PROGRESS pgb;

static uint32_t draw_time = 0;
static void clearTime(void) { draw_time = HAL_GetTick(); }

static void printTime(void)
{
    char str[8];
    sprintf(str, "%ums", HAL_GetTick() - draw_time);
    UG_FontSelect(DEFAULT_FONT);
    UG_SetForecolor(C_WHITE);
    UG_SetBackcolor(C_BLACK);
    UG_PutString(160, 110, str);
}

void window_1_callback(UG_MESSAGE* msg);

void ST7789_PutStr(uint16_t x, uint16_t y, char* str, UG_FONT* font, uint16_t color, uint16_t bgcolor)
{
    UG_FontSelect(font);
    UG_SetForecolor(color);
    UG_SetBackcolor(bgcolor);
    UG_PutString(x, y, str);
}

void ST7789_Test(void)
{
    /*  UG_FillScreen(C_WHITE);
    ST7789_PutStr(10, 10, "Starting Test", DEFAULT_FONT, C_RED, C_WHITE);
    HAL_Delay(500);

    uint8_t r = 0, g = 0, b = 0;
    for (r = 0; r < 32; r++) { // R++, G=0, B=0
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    r = 31;
    for (g = 0; g < 64; g += 2) { // R=31, G++, B=0
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    g = 63;
    for (r = 28; r; r--) { // R--, Gmax, B=0
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    for (b = 0; b < 32; b++) { // R=0, Gmax, B++
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    b = 31;
    for (g = 56; g; g -= 2) { // R=0, G--, Bmax
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    for (r = 0; r < 32; r++) { // R++, G=0, Bmax
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }
    r = 31;
    for (g = 0; g < 64; g += 2) { // Rmax, G++, Bmax
        HAL_Delay(1);
        UG_FillScreen((uint16_t)r << 11 | g << 5 | b);
    }

    UG_FillScreen(C_RED);
    HAL_Delay(500);
    UG_FillScreen(C_GREEN);
    HAL_Delay(500);
    UG_FillScreen(C_BLUE);
    HAL_Delay(500);
    UG_FillScreen(C_BLACK);
    HAL_Delay(500);

    clearTime();
    UG_FillScreen(C_WHITE);
    printTime();
    ST7789_PutStr(10, 10, "Fill Time", DEFAULT_FONT, C_RED, C_WHITE);
    HAL_Delay(500);

    UG_FillScreen(C_BLACK);
    ST7789_PutStr(10, 10, "Font test.", DEFAULT_FONT, C_AZURE, C_BLACK);
    clearTime();
    ST7789_PutStr(10, 35, "Hello Steve!", DEFAULT_FONT, C_CYAN, C_BLACK);
    ST7789_PutStr(10, 60, "Hello Steve!", DEFAULT_FONT, C_ORANGE_RED, C_BLACK);
    ST7789_PutStr(10, 85, "Hello Steve!", DEFAULT_FONT, C_LIME_GREEN, C_BLACK);
    ST7789_PutStr(10, 110, "Hello Steve!", DEFAULT_FONT, C_HOT_PINK, C_BLACK);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Line.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_DrawLine(30, 40, 30, 110, C_WHITE);
    UG_DrawLine(30, 40, 100, 40, C_WHITE);
    UG_DrawLine(30, 40, 100, 110, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Rect.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_DrawFrame(30, 40, 100, 110, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Filled Rect.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_FillFrame(30, 40, 100, 110, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Mesh.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_DrawMesh(30, 40, 100, 110, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Circle.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_DrawCircle(75, 75, 35, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Filled Cir.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_FillCircle(75, 75, 35, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    ST7789_PutStr(10, 10, "Triangle.", DEFAULT_FONT, C_YELLOW, C_RED);
    clearTime();
    UG_DrawTriangle(30, 40, 120, 40, 60, 120, C_WHITE);
    printTime();
    HAL_Delay(500);

    UG_FillScreen(C_RED);
    clearTime();
    ST7789_PutStr(10, 10, "Filled Tri.", DEFAULT_FONT, C_YELLOW, C_RED);
    UG_FillTriangle(30, 40, 120, 40, 60, 120, C_WHITE);
    printTime();
    HAL_Delay(500);
*/
    UG_FillScreen(C_BLACK);
    // Create the window
    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback);
    // Window Title
    UG_WindowSetTitleText(&window_1, "Test Window");
    UG_WindowSetTitleTextFont(&window_1, DEFAULT_FONT);
    UG_WindowSetTitleHeight(&window_1, 30);
    UG_WindowSetXStart(&window_1, 0);
    UG_WindowSetYStart(&window_1, 0);
    UG_WindowSetXEnd(&window_1, 239);
    UG_WindowSetYEnd(&window_1, 134);

    // Create Buttons
    UG_ButtonCreate(&window_1, &button_1, BTN_ID_0, 50, 5, 170, 35);
    //Label Buttons
    UG_ButtonSetFont(&window_1, BTN_ID_0, DEFAULT_FONT);
    UG_ButtonSetForeColor(&window_1, BTN_ID_0, C_BLACK);
    UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_LIGHT_GRAY);
    UG_ButtonSetText(&window_1, BTN_ID_0, "Button");

    // Create Textbox
    UG_TextboxCreate(&window_1, &textbox_1, TXB_ID_0, 10, 40, 200, 65);
    UG_TextboxSetFont(&window_1, TXB_ID_0, DEFAULT_FONT);
    UG_TextboxSetText(&window_1, TXB_ID_0, "Some Text");
    //    UG_TextboxSetBackColor(&window_1, TXB_ID_0, C_LIGHT_YELLOW);
    UG_TextboxSetForeColor(&window_1, TXB_ID_0, C_BLACK);
    UG_TextboxSetAlignment(&window_1, TXB_ID_0, ALIGN_CENTER);

    // Create progress bar
    UG_ProgressCreate(&window_1, &pgb, PGB_ID_0, 10, 72, 200, 85);
    UG_ProgressSetForeColor(&window_1, PGB_ID_0, C_ROYAL_BLUE);

    UG_WindowShow(&window_1);
    clearTime();
    UG_Update();
    printTime();
    HAL_Delay(500);
    UG_WindowShow(&window_1); // Force update
    uint32_t time = HAL_GetTick() + 3000;
    uint8_t progress = 0;
    while (HAL_GetTick() < time) {
        HAL_Delay(1);
        UG_ProgressSetProgress(&window_1, PGB_ID_0, progress++);
        UG_Update();
    }

    //  If program doesn't fit in the FLASH, please disable this code:
    //->>
    UG_FillScreen(0x4b10);
    clearTime();
    UG_DrawBMP((ST7789_WIDTH - fry.width) / 2, (ST7789_HEIGHT - fry.height) / 2, &fry);
    printTime();
    UG_FontSetTransparency(1);
    ST7789_PutStr(10, 10, "Image.", DEFAULT_FONT, C_YELLOW, C_RED);
    UG_FontSetTransparency(0);
    //<<-

    HAL_Delay(500);
}

void window_1_callback(UG_MESSAGE* msg)
{
    /*
    if(msg->type == MSG_TYPE_OBJECT)
    {
        if(msg->id == OBJ_TYPE_BUTTON)
        {
            if(msg->event == OBJ_EVENT_PRESSED)
            {
                switch(msg->sub_id)
                {
                    case BTN_ID_0:
                    {
                        LED4_Write(0);
                        UG_ButtonHide(&window_1,BTN_ID_1);
                        break;
                    }
                    case BTN_ID_1:
                    {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "Pressed!");
                        break;
                    }
                    case BTN_ID_2:
                    {
                        LED4_Write(1);
                        UG_ButtonShow(&window_1,BTN_ID_1);
                        break;
                    }
                    case BTN_ID_3:
                    {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "Pressed!");
                        LED4_Write(!LED4_Read());
                        break;
                    }
                }
            }
            if(msg->event == OBJ_EVENT_RELEASED)
            {
                if(msg->sub_id == BTN_ID_1)
                {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "This is a \n test sample window!");
                }
                if(msg->sub_id == BTN_ID_3)
                {
                        UG_TextboxSetText(&window_1, TXB_ID_0, "This is a \n test sample window!");
                }
            }
        }
    }*/
}
