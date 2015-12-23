#include <pebble.h>

#define DEGREES_IN_A_MINUTE 6 // 360 / 60
#define DEGREES_IN_AN_HOUR 30 // 360 / 24

typedef struct TimeInfo{
  int hours;
  int minutes;
  int min_angle;
  int hour_angle;
  bool is_am;
} TimeInfo;

// Our static variables
static Window * s_main_window;

// Images
static BitmapLayer *s_bitmap_layer_body;
static BitmapLayer *s_bitmap_layer_h_arm;
static BitmapLayer *s_bitmap_layer_m_arm;
static GBitmap * s_dancer_body;
static GBitmap * s_minute_arm;
static GBitmap * s_hour_arm;

// Information about current time.
static TimeInfo current_time;

// Note: We forward declare all our functions here.
// This way we can organize our functions in the code
// according to levels of abstraction.
static void update_time();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void main_window_load(Window * window);
static void main_window_unload(Window * window);
static void init();
static void deinit();
static BitmapLayer * create_new_bitmap_layer(GRect bounds,
                                            GBitmap * bitmap,
                                            uint32_t resource_id);

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
  window_set_background_color(s_main_window, COLOR_FALLBACK(GColorBlue, GColorBlack));

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
  // Get information for window and set up variables
  // for bitmap layers.
  Layer * window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GRect m_arm_bounds = GRect(55,20,120,120);
  GRect h_arm_bounds = GRect(40,27,120,120);


  // We have three different layers: minute arm, the body, and the hour arm.
  // Each must be in its own bitmap layer, so we must create three different
  // bitmap layers.
  s_bitmap_layer_m_arm = create_new_bitmap_layer(m_arm_bounds, s_minute_arm, RESOURCE_ID_MINUTE_ARM);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer_m_arm));

  s_bitmap_layer_body = create_new_bitmap_layer(bounds, s_dancer_body, RESOURCE_ID_DANCER_BODY);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer_body));

  s_bitmap_layer_h_arm = create_new_bitmap_layer(h_arm_bounds, s_hour_arm, RESOURCE_ID_HOUR_ARM);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer_h_arm));

}

// This adds a new bitmap layer. Since we do this three times for
// this app, this function prevents repetitive code.
static BitmapLayer * create_new_bitmap_layer(GRect bounds,
                                             GBitmap * bitmap,
                                             uint32_t resource_id) {
  BitmapLayer * bitmap_layer = bitmap_layer_create(bounds);
  bitmap = gbitmap_create_with_resource(resource_id);
  bitmap_layer_set_bitmap(bitmap_layer, bitmap);
  bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpSet);
  return bitmap_layer;
}

// Destroy the bitmap layers once the app closes.
static void main_window_unload(Window * window) {
  // Destroy images
  bitmap_layer_destroy(s_bitmap_layer_m_arm);
  gbitmap_destroy(s_dancer_body);
  bitmap_layer_destroy(s_bitmap_layer_body);
  gbitmap_destroy(s_minute_arm);
  bitmap_layer_destroy(s_bitmap_layer_h_arm);
  gbitmap_destroy(s_hour_arm);
}

// This function is called each time the time is updated.
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

// This function should only be called once when the app is started.
/*static void initialize_time() {
  time_t temp = time(NULL);
  struct tm * tick_time = localtime(&temp);

  current_time.hours = tick_time->tm_hour;
  current_time.minutes = tick_time->tm_min;
  current_time.is_am = tick_time->tm_hour < 12;
  current_time.min_angle = tick_time->tm_hour * 360 / 60;
  current_time.hour_angle = tick_time->tm_hour * 360 / 24;

}*/

static void update_time() {
  // We get the time structure from the update.
  time_t temp = time(NULL);
  struct tm * tick_time = localtime(&temp);

  current_time.hours = tick_time->tm_hour;
  current_time.minutes = tick_time->tm_min;
  current_time.is_am = tick_time->tm_hour < 12;
  current_time.min_angle = (current_time.min_angle + DEGREES_IN_A_MINUTE) % 360;
  current_time.hour_angle = (current_time.hour_angle + DEGREES_IN_AN_HOUR) % 360;

}