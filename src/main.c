#include <pebble.h>
#include "internet_time.h"

static Window *s_main_window;
static GFont s_time_font, s_date_font;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_bits_layer;

#define BUFFER_SIZE 86

static char bits_buffer[BUFFER_SIZE] = "999"; 

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);
 

  // Create some long-lived buffers
  static char time_buffer[] = "00:00";
  static char date_buffer[] = "Mon 01 Jan";
  
  
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 24h hour format    
    strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
  } else {
    //Use 12 hour format    
    strftime(time_buffer, sizeof(time_buffer), "%I:%M", tick_time);
  }

    strftime(date_buffer, sizeof(date_buffer), "%a %d %b", tick_time);
  
  internet_time(tick_time->tm_hour, tick_time->tm_min, bits_buffer, BUFFER_SIZE);
  
    // Display this time elements on the Layers
  text_layer_set_text(s_time_layer, time_buffer);
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_bits_layer, bits_buffer);
  
  
}

static void main_window_load(Window *window) {
  
  // Create time TimeLayer
  s_time_layer = text_layer_create(GRect(3, 3, 138, 71));
  text_layer_set_background_color(s_time_layer, GColorBlack);
  text_layer_set_text_color(s_time_layer, GColorCyan);
  text_layer_set_text(s_time_layer, "00:00");
  
  // Create time DateLayer
  s_date_layer = text_layer_create(GRect(3, 74, 138, 28));  
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorRed);
  text_layer_set_text(s_date_layer, "Mon 01 Jan");
  
  // Create time BitsLayer
  s_bits_layer = text_layer_create(GRect(3, 102, 138, 69));  
  //s_data.label = text_layer_create(GRect(3, 102, 138, 69));  
  text_layer_set_background_color(s_bits_layer, GColorBlack);
  text_layer_set_text_color(s_bits_layer, GColorCyan);
  text_layer_set_text(s_bits_layer, bits_buffer);
       
  // Create GFont
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCD_60));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCD_25));
  
  // Apply to TextLayer
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_font(s_bits_layer, s_time_font);
  
    // Improve the layout to be more like a watchface  
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_bits_layer, GTextAlignmentCenter);
  
    // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_bits_layer));
      
  // Make sure the time is displayed from the start
  update_time();
}

static void main_window_unload(Window *window) {
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
  
  // Destroy TextLayers
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_bits_layer); 
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}
  
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  
  // set the window background
  window_set_background_color(s_main_window, GColorBlack);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}