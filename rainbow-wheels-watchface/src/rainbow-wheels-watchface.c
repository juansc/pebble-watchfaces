#include <pebble.h>

#define DEGREES_IN_A_CIRCLE 360
#define HOURS_IN_A_CIRCLE 12
#define MINUTES_IN_A_CIRCLE 60
#define SECONDS_IN_A_CIRCLE 60
#define DEGREES_IN_A_SECOND (DEGREES_IN_A_CIRCLE / SECONDS_IN_A_CIRCLE)
#define DEGREES_IN_A_MINUTE (DEGREES_IN_A_CIRCLE / MINUTES_IN_A_CIRCLE)
#define DEGREES_IN_AN_HOUR (DEGREES_IN_A_CIRCLE / HOURS_IN_A_CIRCLE)

// Our static variables
static Window * s_main_window;

// Images
static Layer * hr_layer;
static Layer * min_layer;
static Layer * sec_layer;

static int hr_angle;
static int min_angle;
static int sec_angle;

// Colors
static const int num_of_colors = 12;
static const int sec_shrink = 20;
static const int min_shrink = 40;
static const int hr_shrink = 65;
static const int sec_ring_width = 15;
static const int min_ring_width = 20;
static const int hr_ring_width = 15;

// Note: We forward declare all our functions here.
// This way we can organize our functions in the code
// according to levels of abstraction.
static void update_time();
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void main_window_load(Window * window);
static void main_window_unload(Window * window);
static void init();
static void deinit();
static void render_sec_layer(Layer *layer, GContext *ctx);
static void render_min_layer(Layer *layer, GContext *ctx);
static void render_hr_layer(Layer *layer, GContext *ctx);
static void draw_rainbow_wheel(GContext *ctx, GRect bounds,
                               int radius, uint16_t thickness, int angle_offset);

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
  window_set_background_color(s_main_window, GColorLightGray);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Every minute call the tick_handler function
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

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

  sec_layer = layer_create(bounds);
  layer_set_update_proc(sec_layer, render_sec_layer);
  layer_add_child(window_layer, (Layer *)sec_layer);

  min_layer = layer_create(bounds);
  layer_set_update_proc(min_layer, render_min_layer);
  layer_add_child(window_layer, (Layer *)min_layer);

  hr_layer = layer_create(bounds);
  layer_set_update_proc(hr_layer, render_hr_layer);
  layer_add_child(window_layer, (Layer *)hr_layer);
}

// Destroy the bitmap layers once the app closes.
static void main_window_unload(Window * window) {
  // Destroy images
  layer_destroy(hr_layer);
  layer_destroy(min_layer);
  layer_destroy(sec_layer);
}

// This function is called each time the time is updated.
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void render_sec_layer(Layer *layer, GContext *ctx) {
  GRect my_bounds = layer_get_bounds(layer);
  draw_rainbow_wheel(ctx, my_bounds, sec_shrink, sec_ring_width, sec_angle);
}

static void render_min_layer(Layer *layer, GContext *ctx) {
  GRect my_bounds = layer_get_bounds(layer);
  draw_rainbow_wheel(ctx, my_bounds, min_shrink, min_ring_width, min_angle);
}

static void render_hr_layer(Layer *layer, GContext *ctx) {
  GRect my_bounds = layer_get_bounds(layer);
  draw_rainbow_wheel(ctx, my_bounds, hr_shrink, hr_ring_width, hr_angle);
}

static void draw_rainbow_wheel(GContext *ctx, GRect bounds,
                               int radius, uint16_t thickness, int angle_offset) {

  GRect frame = grect_inset(bounds, GEdgeInsets(radius));

  int deg_per_color = 360 / num_of_colors;
  int start_deg = angle_offset;
  int end_deg = start_deg + deg_per_color;

  GColor8 colors [] = {GColorRed,
                       GColorOrange,
                       GColorRajah,
                       GColorYellow,
                       GColorSpringBud,
                       GColorGreen,
                       GColorVividCerulean,
                       GColorBlueMoon,
                       GColorBlue,
                       GColorDukeBlue,
                       GColorIndigo,
                       GColorPurple};

  for(int i = 0; i < num_of_colors; i += 1) {
    graphics_context_set_fill_color(ctx, colors[i]);
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, thickness,
                       DEG_TO_TRIGANGLE(start_deg), DEG_TO_TRIGANGLE(end_deg));
    start_deg += deg_per_color;
    end_deg += deg_per_color;
  }

}

static void update_time() {
  // We get the time structure from the update.
  time_t temp = time(NULL);
  struct tm * tick_time = localtime(&temp);

  int angle_offset = DEGREES_IN_A_CIRCLE / (2 * num_of_colors);

  hr_angle = (tick_time->tm_hour - 1) * DEGREES_IN_AN_HOUR - angle_offset;
  min_angle = tick_time->tm_min * DEGREES_IN_A_MINUTE - angle_offset;
  sec_angle = tick_time->tm_sec * DEGREES_IN_A_SECOND - angle_offset;
  layer_mark_dirty(hr_layer);
  layer_mark_dirty(min_layer);
  layer_mark_dirty(sec_layer);
}