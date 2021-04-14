#include <Portenta_LittleVGL.h>

int16_t mouseX = 0;           // Mouse coord X
int16_t mouseY = 0;           // Mouse coord Y
uint8_t mouseButtons = 0;     // Mouse buttons
bool textAreaFocused = false; //Know when the text area is focused, so it allows to input trough the keyboard

//Create all the widgets and configure them
void createWidgets();

//Popup message creator, it will create the message with the custom text, and a button to close that popup
void popupMessage(const char *innerText);

//Widgets objects and events
//Text area
static lv_obj_t *textArea;
static void textAreaEventHandler(lv_obj_t *textArea, lv_event_t textAreaEvent);

//Submit button, it will create a popup message with the text from the text area
static lv_obj_t *submitButton;
static void submitButtonEventHandler(lv_obj_t *submitButton, lv_event_t submitEvent);

//Launch keyboard button, in case you dont connect any usb keyboard to the portenta, you can write with the screen keyboard from LVGL
static lv_obj_t *launchButton;
static void launchButtonEventHandler(lv_obj_t *launchButton, lv_event_t buttonEvent);

//LVGL's keyboard widget, virtual on-screen keyboard
static lv_obj_t *keyboard;
static void keyboardEventHandler(lv_obj_t *keyboard, lv_event_t keyboardEvent);
static void keyboardCreate();

//Mouse driver, gets the mouse data and interact with the rest of the widgets
static lv_indev_drv_t mouseIndevDriver;
static lv_obj_t *mouseCursor;
bool mouseRead(lv_indev_drv_t *mouseIndevDriver, lv_indev_data_t *data);

void createWidgets()
{
    // Mouse pointer driver init
    lv_indev_drv_init(&mouseIndevDriver);
    mouseIndevDriver.type = LV_INDEV_TYPE_POINTER;
    mouseIndevDriver.read_cb = mouseRead;
    lv_indev_t *my_indev_mouse = lv_indev_drv_register(&mouseIndevDriver);

    // Mouse pointer
    mouseCursor = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(mouseCursor, LV_SYMBOL_GPS);
    lv_indev_set_cursor(my_indev_mouse, mouseCursor); // connect the object to the driver

    //Submit button
    submitButton = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_align(submitButton, NULL, LV_ALIGN_CENTER, 185, -150);
    lv_obj_set_event_cb(submitButton, submitButtonEventHandler);
    //Text for the submit button
    lv_obj_t *submitButtonText = lv_label_create(submitButton, NULL);
    lv_label_set_text(submitButtonText, "Submit");

    //Launch keyboard button
    launchButton = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(launchButton, launchButtonEventHandler);
    lv_obj_align(launchButton, NULL, LV_ALIGN_CENTER, 185, -100);
    lv_obj_t *launchButtonText = lv_label_create(launchButton, NULL);
    lv_label_set_text(launchButtonText, LV_SYMBOL_KEYBOARD);

    //Text Area
    textArea = lv_textarea_create(lv_scr_act(), NULL);
    lv_obj_set_size(textArea, 200, 100);
    lv_obj_align(textArea, NULL, LV_ALIGN_CENTER, 0, -120);
    lv_textarea_set_placeholder_text(textArea, "LVGL and Arduino PRO");
    lv_textarea_set_text(textArea, ""); /*Set an initial text*/
    lv_obj_set_event_cb(textArea, textAreaEventHandler);
    lv_textarea_set_cursor_click_pos(textArea, true);
    lv_textarea_set_cursor_hidden(textArea, true);
    lv_textarea_set_text_sel(textArea, true);
}

// popup message creator, with custom text in the parameters
void popupMessage(const char *innerText)
{
    static const char *options[] = {"OK", ""}; //Button matrix, second space empty
    innerText += '\0';

    /* Create the message box as a child of the modal background */
    lv_obj_t *messageBox = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_add_btns(messageBox, options);
    lv_msgbox_set_text(messageBox, innerText);
    lv_obj_align(messageBox, NULL, LV_ALIGN_CENTER, 0, 0);
}

// Launch keyboard button callback, it will create a keyboard to interact with the text area
static void launchButtonEventHandler(lv_obj_t *launchButton, lv_event_t launchButtonEvent)
{
    if (launchButtonEvent == LV_EVENT_CLICKED)
    {
        keyboardCreate();
    }
}

// Submit button callback, it will send the content from the text area to a new popup message
static void submitButtonEventHandler(lv_obj_t *submitButton, lv_event_t submitButtonEvent)
{
    if (submitButtonEvent == LV_EVENT_CLICKED)
    {
        const char *newText = lv_textarea_get_text(textArea); //the text in the message box
        popupMessage(newText);
    }
}

// LVGL's keyboard widget callback, its not needed to configure each KEY, just we add the feature to exit and submit from the keyboard, with the cross and tick buttons (cancel button, and confirm button)
static void keyboardEventHandler(lv_obj_t *keyboard, lv_event_t keyboardEvent)
{
    lv_keyboard_def_event_cb(keyboard, keyboardEvent);
    if (keyboardEvent == LV_EVENT_CANCEL)
    {
        lv_keyboard_set_textarea(keyboard, NULL);
        lv_obj_del(keyboard);
        keyboard = NULL;
    }
    else if (keyboardEvent == LV_EVENT_APPLY)
    {
        const char *newText = lv_textarea_get_text(textArea); //the text in the message box
        popupMessage(newText);
    }
}

//Create keyboard widget
static void keyboardCreate(void)
{
    keyboard = lv_keyboard_create(lv_scr_act(), NULL);
    lv_keyboard_set_cursor_manage(keyboard, true);
    lv_obj_set_event_cb(keyboard, keyboardEventHandler);
    lv_keyboard_set_textarea(keyboard, textArea);
}

//This event handler callback, used to know when the Text area is being focused by the user, so it allows the usb keyboard to interact with it
static void textAreaEventHandler(lv_obj_t *textArea, lv_event_t textAreaEvent)
{
    if (textAreaEvent == LV_EVENT_FOCUSED)
    {
        textAreaFocused = true;
        lv_textarea_set_cursor_hidden(textArea, false);
    }
    else if (textAreaEvent == LV_EVENT_DEFOCUSED)
    {
        textAreaFocused = false;
        lv_textarea_set_cursor_hidden(textArea, true);
    }
}

//Function to get the data from the Mouse, to the LVGL's mouse driver
bool mouseRead(lv_indev_drv_t *mouseIndevDriver, lv_indev_data_t *data)
{
    static uint32_t lastButton = 0;       /*Store the last pressed button*/
    int buttonPressed = mouseButtons - 1; /*Get the ID (0,1,2...) of the pressed button*/

    data->point.x = mouseX;
    data->point.y = mouseY;

    //If left mouse button is being clicked
    if (buttonPressed >= 0)
    {                                    /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
        lastButton = buttonPressed;      /*Save the ID of the pressed button*/
        data->state = LV_INDEV_STATE_PR; /*Set the pressed state*/
    }
    else
    {
        data->state = LV_INDEV_STATE_REL; /*Set the released state*/
    }

    data->btn_id = lastButton; /*Save the last button*/

    return false; /*No buffering now so no more data read*/
}
