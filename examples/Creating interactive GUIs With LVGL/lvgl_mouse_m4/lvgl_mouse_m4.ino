#include "Portenta_USBhost.h"
#include "RPC_internal.h"

#ifndef CORE_CM4
#error "This sketch should be compiled for Portenta (M4 core)"
#endif

void setup()
{
    // put your setup code here, to run once:
    USBhost_init();
    RPC1.begin();
}

void loop()
{
}

static int mouseCallback(tusbh_ep_info_t *ep, const uint8_t *mouse)
{
    uint8_t btn = mouse[0];
    int8_t x = ((int8_t *)mouse)[1];
    int8_t y = ((int8_t *)mouse)[2];
    RPC1.call("on_mouse", btn, x, y);
}

static int keyboardCallback(tusbh_ep_info_t *ep, const uint8_t *keys)
{
    char sendCharacter;
    sendCharacter = parseKeyboardInput(ep, keys);
    RPC1.call("on_key", sendCharacter);
    return 0;
}