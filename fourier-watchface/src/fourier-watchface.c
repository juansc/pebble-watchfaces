#include <pebble.h>

#define DEGREES_IN_A_CIRCLE 360
#define HOURS_IN_A_CIRCLE 12
#define MINUTES_IN_A_CIRCLE 60
#define SECONDS_IN_A_CIRCLE 60
#define DEGREES_IN_AN_HOUR (DEGREES_IN_A_CIRCLE / HOURS_IN_A_CIRCLE)
#define DEGREES_IN_A_MINUTE (DEGREES_IN_A_CIRCLE / MINUTES_IN_A_CIRCLE)
#define DEGREES_IN_A_SECOND (DEGREES_IN_A_CIRCLE / SECONDS_IN_A_CIRCLE)
#define ANGLE_OFFSET 90

static Window * window;
static Layer * sec_circle_layer;
static Layer * min_circle_layer;
static Layer * hr_circle_layer;
static GRect bounds;

static const int circ_1_radius = 10;
static const int circ_2_radius = 5;
static const int circ_3_radius = 3;

static void window_load(Window *window);
static void window_unload(Window *window);
static void init();
static void deinit();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

int main(void) {
  init();
  app_event_loop();
  deinit();
}

static void init() {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  update_time();
}

static void deinit() {
  window_destroy(window);
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  bounds = layer_get_bounds(window_layer);
}

static void window_unload(Window *window) {
  layer_destroy(hr_circle_layer);
  layer_destroy(min_circle_layer);
  layer_destroy(sec_circle_layer);
}

// This function is called each time the time is updated.
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void update_time() {
  int circ_1_angle, circ_2_angle, circ_3_angle;
  int16_t circ_1_x, circ_2_x, circ_3_x,
          circ_1_y, circ_2_y, circ_3_y;
  GPoint circ_1_center, circ_2_center, circ_3_center;
  // We get the time structure from the update.
  time_t temp = time(NULL);
  struct tm * tick_time = localtime(&temp);

  // Generate the angles
  circ_1_angle = tick_time->tm_hour * DEGREES_IN_AN_HOUR - ANGLE_OFFSET;
  circ_2_angle = tick_time->tm_min * DEGREES_IN_A_MINUTE - ANGLE_OFFSET;
  circ_3_angle = tick_time->tm_sec * DEGREES_IN_A_SECOND - ANGLE_OFFSET;

  circ_1_x = bounds.size.w / 4;
  circ_1_y = bounds.size.h / 4;
  circ_2_x = (int16_t)(cos_lookup(DEG_TO_TRIGANGLE(circ_1_angle)) * circ_1_radius + circ_1_x);
  circ_2_y = (int16_t)(sin_lookup(DEG_TO_TRIGANGLE(circ_1_angle)) * circ_1_radius + circ_1_y);
  circ_3_x = (int16_t)(cos_lookup(DEG_TO_TRIGANGLE(circ_2_angle)) * circ_2_radius + circ_2_x);
  circ_3_y = (int16_t)(sin_lookup(DEG_TO_TRIGANGLE(circ_2_angle)) * circ_2_radius + circ_2_y);

  circ_1_center = GPoint(circ_1_x, circ_1_y);
  circ_2_center = GPoint(circ_2_x, circ_2_y);
  circ_3_center = GPoint(circ_3_x, circ_3_y);

  // Create circles in correct position with Trixie's functions here
}
