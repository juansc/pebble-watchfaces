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
static RotBitmapLayer * hour_arm_layer;
static RotBitmapLayer * body_layer;
static RotBitmapLayer * min_arm_layer;
static GBitmap * dancer_body_bitmap;
static GBitmap * minute_arm_bitmap;
static GBitmap * hour_arm_bitmap;

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
static RotBitmapLayer * create_new_rot_bitmap_layer(GRect bounds,
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
  GRect body_bounds = GRect(45,45,500,500);
  GRect m_arm_bounds = GRect(50,40,120,120);
  GRect h_arm_bounds = GRect(40,39,120,120);


  // We have three different layers: minute arm, the body, and the hour arm.
  // Each must be in its own rotatable bitmap layer, so we must create three different
  // rot_bitmap layers.
  min_arm_layer = create_new_rot_bitmap_layer(m_arm_bounds, minute_arm_bitmap, RESOURCE_ID_MINUTE_ARM);
  rot_bitmap_set_src_ic(min_arm_layer, GPoint(5, 5));
  layer_add_child(window_layer, (Layer *)min_arm_layer);


  body_layer = create_new_rot_bitmap_layer(body_bounds, dancer_body_bitmap, RESOURCE_ID_DANCER_BODY);
  layer_add_child(window_layer, (Layer *)body_layer);

  hour_arm_layer = create_new_rot_bitmap_layer(h_arm_bounds, hour_arm_bitmap, RESOURCE_ID_HOUR_ARM);
  rot_bitmap_set_src_ic(hour_arm_layer, GPoint(10, 10));
  layer_add_child(window_layer, (Layer *)hour_arm_layer);

}

// This adds a new rot_bitmap layer. Since we do this three times for
// this app, this function prevents repetitive code.
static RotBitmapLayer * create_new_rot_bitmap_layer(GRect bounds,
                                                    GBitmap * bitmap,
                                                    uint32_t resource_id) {
  bitmap = gbitmap_create_with_resource(resource_id);
  RotBitmapLayer * bitmap_layer = rot_bitmap_layer_create(bitmap);
  rot_bitmap_set_compositing_mode(bitmap_layer, GCompOpSet);
  layer_set_frame((Layer *)bitmap_layer, bounds);
  return bitmap_layer;
}

// Destroy the bitmap layers once the app closes.
static void main_window_unload(Window * window) {
  // Destroy images
  rot_bitmap_layer_destroy(min_arm_layer);
  gbitmap_destroy(minute_arm_bitmap);
  rot_bitmap_layer_destroy(body_layer);
  gbitmap_destroy(dancer_body_bitmap);
  rot_bitmap_layer_destroy(hour_arm_layer);
  gbitmap_destroy(hour_arm_bitmap);
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

  rot_bitmap_layer_set_angle(min_arm_layer, DEG_TO_TRIGANGLE(tick_time->tm_min * 360 / 60 - 90));
  rot_bitmap_layer_set_angle(hour_arm_layer, DEG_TO_TRIGANGLE(tick_time->tm_hour * 360 / 12 - 90));

}