#ifndef LV_CONF_H
#define LV_CONF_H

//extern "C" {
//#include <stdio.h>
//#include <stdlib.h>
//};

#define LV_MEME_SIZE (38ul * 1024ul)

#define LV_COLOR_DEPTH 16

#define LV_USE_DEMO_BENCHMARK          1
#define LV_USE_DEMO_KEYPAD_AND_ENCODER 1
#define LV_USE_DEMO_MUSIC              1
#define LV_USE_DEMO_STRESS             1
//#define LV_USE_DEMO_WIDGETS            1

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_24 1

#define LV_DEMO_MUSIC_AUTO_PLAY 1

//LV_FONT_CUSTOM_DECLARE

#define LV_FONT_CUSTOM_DECLARE \
    LV_FONT_DECLARE(Ru)        \
    LV_FONT_DECLARE(Ru24)      \
    LV_FONT_DECLARE(Ru32)

#endif // LV_CONF_H
