#pragma once
#include <array>
#include <span>
#include <string_view>

#include "ugui.h"

extern "C" {
extern uint32_t HAL_GetTick();
extern void HAL_Delay(uint32_t d);
#define DEFAULT_FONT FONT_arial_20X23 //FONT_12X20
} // extern "C"

class TestWindow {

    enum {
        MAX_OBJECTS = 20,
        WIDTH = 800,
        HEIGHT = 480
    };
    static inline UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];
    static inline UG_WINDOW window_1 { .style = WND_STYLE_2D };
    static inline UG_TEXTBOX textbox_1 = { .style = WND_STYLE_3D };
    static inline int textbox_1id;

    static inline std::array<UG_BUTTON, 10> buttons {
        UG_BUTTON { .style = WND_STYLE_3D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },

        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
        { .style = WND_STYLE_2D },
    };

    static inline std::string_view btNames[10] {
        { "Butt 1" },
        { "Butt 2" },
        { "Butt 3" },
        { "Butt 4" },
        { "Butt 5" },
        { "Butt 6" },
        { "Butt 7" },
        { "Butt 8" },
        { "Butt 9" },
        { "Close" },
    };
    static void callback(UG_MESSAGE* msg);

public:
    TestWindow();

    void show();
};
