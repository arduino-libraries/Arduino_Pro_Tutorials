#include "RPC_internal.h"
#include "USBHost.h"
#include "widgets.h"

void setup()
{
    // put your setup code here, to run once:
    Serial1.begin(115200);
    initInputs();
    portenta_init_video();
    createWidgets();
}

void loop()
{
    // put your main code here, to run repeatedly:
    lv_task_handler();
}

//Keyboard callback, in case the Text area is focused (mouse clicked in it) it can bypass the input to it
void onKey(char ch)
{
    if (textAreaFocused)
    {
        lv_textarea_add_char(textArea, ch);
    }
}

// Mouse callback, store the mouse vector that is going to be used by LVGL's mouse driver
void onMouse(uint8_t buttons, int8_t x, int8_t y)
{
    mouseX += x;
    mouseY += y;
    if (mouseX < 0)
    {
        mouseX = 0;
    }
    if (mouseY < 0)
    {
        mouseY = 0;
    }
    mouseButtons = buttons;
}

// init RPC an attach the RPC messages to the functions
void initInputs()
{
    RPC1.begin();
    RPC1.bind("on_mouse", onMouse);
    RPC1.bind("on_key", onKey);
}