#include "Portenta_LittleVGL.h"
#include "USBHost.h"

static lv_obj_t * lv_btn;
static lv_obj_t * myCustomLabel;
static void label_Task(lv_task_t * myTask);
uint32_t count = 0;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(115200);

  portenta_init_video();

  //Setting up the Button
  lv_btn = lv_btn_create(lv_scr_act(), NULL);
  lv_obj_align(lv_btn, NULL, LV_ALIGN_CENTER, -40, -40);
  //lv_obj_set_event_cb(lv_btn, event_handler);        // If you want to handle the button's callback create a cb_btn function

  //Setting up inner Label
  myCustomLabel = lv_label_create(lv_btn, NULL);       // We make the object be a label widget, lv_btn child
  lv_label_set_text(myCustomLabel , "Button");         // We set the default text

  //Create a task
  lv_task_create(label_Task, 1000, LV_TASK_PRIO_MID, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_task_handler();
  
}

static void label_Task(lv_task_t * myTask) {
  //printf("count: %d\n", count);                      // We can see in the Serial monitor the count
  lv_label_set_text_fmt(myCustomLabel, "%d" , count);  // Update the text from the label
  count++;                                             // Increase the count number
}
