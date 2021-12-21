#include "testwindow.h"
#include <QDebug>

QDebug operator<<(QDebug debug, const UG_MESSAGE& msg)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "UG_MESSAGE{ ";
    debug.nospace() << "type=" << msg.type;
    debug.nospace() << ", id=" << msg.id;
    debug.nospace() << ", sub_id=" << msg.sub_id;
    debug.nospace() << ", event=" << msg.event;
    debug.nospace() << ", src=" << msg.src;
    debug.nospace() << " }";
    return debug;
}

void TestWindow::callback(UG_MESSAGE* msg)
{
    qDebug() << *msg;
    UG_Button b;
    if (msg->type == MSG_TYPE_OBJECT) {
        if (msg->id == OBJ_TYPE_BUTTON) {
            if (msg->event == OBJ_EVENT_PRESSED) {
                switch (msg->sub_id) {
                case BTN_ID_0: {
                    //                    LED4_Write(0);
                    b.Hide(&window_1, BTN_ID_1);
                    break;
                }
                case BTN_ID_1: {
                    UG_TextboxSetText(&window_1, textbox_1id, "Pressed!");
                    break;
                }
                case BTN_ID_2: {
                    //                    LED4_Write(1);
                    b.Show(&window_1, BTN_ID_1);
                    break;
                }
                case BTN_ID_3: {
                    UG_TextboxSetText(&window_1, textbox_1id, "Pressed!");
                    //                    LED4_Write(!LED4_Read());
                    break;
                }
                case BTN_ID_8: {
                    UG_TextboxSetText(&window_1, textbox_1id, "Pressed!");
                    break;
                }
                case BTN_ID_9: {
                    exit(0);
                    //                    LED4_Write(!LED4_Read());
                    break;
                }
                }
            }
            if (msg->event == OBJ_EVENT_RELEASED) {
                if (msg->sub_id == BTN_ID_1) {
                    UG_TextboxSetText(&window_1, textbox_1id, "This is a \n test sample window!");
                }
                if (msg->sub_id == BTN_ID_3) {
                    UG_TextboxSetText(&window_1, textbox_1id, "This is a \n test sample window!");
                }
            }
        }
    }
}

TestWindow::TestWindow()
{ // Create the window
    UG_WindowCreate(&window_1, obj_buff_wnd_1, MAX_OBJECTS, callback);
    // Window Title
    UG_WindowSetTitleText(&window_1, "Test Window");
    UG_WindowSetTitleTextFont(&window_1, DEFAULT_FONT);
    UG_WindowSetTitleHeight(&window_1, 30);
    UG_WindowSetXStart(&window_1, 10);
    UG_WindowSetYStart(&window_1, 10);
    UG_WindowSetXEnd(&window_1, WIDTH - 10);
    UG_WindowSetYEnd(&window_1, HEIGHT - 10);

    const int w { 80 }, h { 30 };
    int idCounter {};

    int x {}, y {};
    struct PT {
        int x, y;
    } btPos[10] {
        { 10, y = 10 },
        { 10, y += h + 10 },
        { 10, y += h + 10 },
        { 10, y += h + 10 },
        { 10, y += h + 10 },

        { w + 20, y = 10 },
        { w + 20, y += h + 10 },
        { w + 20, y += h + 10 },
        { w + 20, y += h + 10 },
        { w + 20, y += h + 10 },

    };
    UG_Button b;
    for (auto& bt : buttons) {
        // Create Buttons
        b.Create(&window_1, &bt, idCounter, btPos[idCounter].x, btPos[idCounter].y, btPos[idCounter].x + w, btPos[idCounter].y + h);

        //Label Buttons
        b.SetFont(&window_1, idCounter, DEFAULT_FONT);
        b.SetForeColor(&window_1, idCounter, C_BLACK);
        b.SetBackColor(&window_1, idCounter, C_LIGHT_GRAY);
        b.SetText(&window_1, idCounter, btNames[idCounter].data());
        ++idCounter;
        x += w, y += h;
    }

    // Create Textbox
    auto rt = UG_TextboxCreate(&window_1, &textbox_1, textbox_1id, 400, 200, 200, 100);
    rt = UG_TextboxSetFont(&window_1, textbox_1id, DEFAULT_FONT);
    rt = UG_TextboxSetText(&window_1, textbox_1id, "Some Text");
    //    rt = UG_TextboxSetBackColor(&window_1, textbox_1id, C_MAROON);
    rt = UG_TextboxSetForeColor(&window_1, textbox_1id, C_BLACK);
    rt = UG_TextboxSetAlignment(&window_1, textbox_1id, ALIGN_CENTER);
}

void TestWindow::show()
{
    UG_WindowShow(&window_1);
    UG_Update();
}
