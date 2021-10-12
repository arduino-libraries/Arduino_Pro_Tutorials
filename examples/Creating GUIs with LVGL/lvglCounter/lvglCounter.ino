/*
    Using LVGL v7.11
*/

#include "Portenta_LittleVGL.h"

static lv_obj_t *label;
int counter = 0;
lv_timer_t * timer;

static void updateCounterTimer(lv_timer_t *timer) {
  // Print the count to the Serial monitor
  Serial.println(counter);

  // Update the text of the label
  lv_label_set_text_fmt(label, "%d" , counter);    

  // Increase the count number
  counter++;                                               
}

void setup() {  
  Serial.begin(9600);

  // Initialize Portenta's video interface
  portenta_init_video();

  // Setting up the label making it a child of the screen
  label = lv_label_create(lv_scr_act());

  // Set the label's text
  lv_label_set_text(label , "Counter");

  // We move it to the center of the screen and align it centered
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // Create a timer to update the counter
  timer = lv_timer_create(updateCounterTimer, 1000, NULL);
}

void loop() {
  // put your main code here, to run repeatedly:
  lv_timer_handler();
}