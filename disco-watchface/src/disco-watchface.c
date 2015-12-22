#include <pebble.h>

static Window * s_main_window;
static TextLayer * s_time_layer;

// Note: We forward declare all our functions here.
// This way we can organize our functions in the code
// according to levels of abstraction.

static void customize_time_text();
static void update_time();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void main_window_load(Window * window);
static void main_window_unload(Window * window);
static void init();
static void deinit();

// Entry point for our program
int main(void) {
  init();
  app_event_loop();
  deinit();
}

// What happens on startup
static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Every minute call the tick_handler function
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

static void main_window_load(Window * window) {
  // Get information about the Window
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer
  s_time_layer = text_layer_create(
                    GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));

  // Apply styles to TextLayer.
  customize_time_text();

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

}

static void main_window_unload(Window * window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

// Improve the layout to be more like a watchface
static void customize_time_text() {

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

}

// This function is called each time the time is updated.
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_time() {
  // We get the time structure from the update.
  time_t temp = time(NULL);
  struct tm * tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  // We use static to make the s_buffer persist accross
  // calls.
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}