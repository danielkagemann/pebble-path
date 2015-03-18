#include <pebble.h>
#include "animation.h"

static Window 		*window;
static TextLayer 	*text_month;
static TextLayer 	*text_hour;
static TextLayer 	*text_minute_u;
static TextLayer 	*text_minute_d;
static Layer      *seconds_layer;

#define BLOCKSIZE    2
#define MARGIN      12
#define DATESIZE    20

#define DURATION    300
#define DELAY       400

static char* month_name[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEV"};

animationchain    amonth_out, amonth_in;
animationchain    ahour_out, ahour_in;
animationchain    aminu_out, aminu_in;
animationchain    amind_out, amind_in;

/**
 * drawing seconds path
 */
static void seconds_display_callback(Layer *layer, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  static char today[5];
  
  // seconds in different areas

  GRect  rc;
  rc.origin.x = MARGIN;
  rc.origin.y = 150;
  rc.size.w   = t->tm_sec * BLOCKSIZE;
  rc.size.h   = BLOCKSIZE * 2;
  
#ifdef PBL_COLOR
  // draw seconds
  graphics_context_set_stroke_color(ctx, GColorJazzberryJam);
  graphics_context_set_fill_color(ctx, GColorJazzberryJam);
  
#else
  // draw seconds
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
#endif
  graphics_fill_rect(ctx, rc, 0, GCornerNone);
  
  // draw date line
  graphics_draw_line(ctx, GPoint(65, 27), GPoint(144 - MARGIN, 27));
  // width = 144 - 65 - MARGIN = 79 - MARGIN
  // draw date on line (width / 30) * t->tm_mday
  rc.origin.x = 65 + t->tm_mday*((79 - MARGIN)/30);
  rc.origin.y = 18;
  rc.size.w   = DATESIZE;
  rc.size.h   = DATESIZE;
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorDarkCandyAppleRed);
#endif

  graphics_fill_rect(ctx, rc, 3, GCornersAll);
  
  // now draw the text in it
  snprintf (today,sizeof(today), "%d", t->tm_mday);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_draw_text(ctx, today, fonts_get_system_font(FONT_KEY_GOTHIC_14), rc, GTextOverflowModeFill,GTextAlignmentCenter,NULL);
}

/**
 * loading window 
 */
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // month
  text_month = text_layer_create((GRect) { .origin = { 12, 18 }, .size = { 50, 20 } });
  text_layer_set_text_color(text_month, GColorBlack);
  text_layer_set_font(text_month, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_background_color(text_month, GColorClear);
  text_layer_set_text_alignment(text_month, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_month));

  // hour
  text_hour = text_layer_create((GRect) { .origin = { 0, 58 }, .size = { 75, 50 } });
#ifdef PBL_COLOR
    text_layer_set_text_color(text_hour, GColorDarkGray);
#else
    text_layer_set_text_color(text_hour, GColorBlack);
#endif
  text_layer_set_font(text_hour, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_background_color(text_hour, GColorClear);
  text_layer_set_text_alignment(text_hour, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(text_hour));

  // minute_u
  text_minute_u = text_layer_create((GRect) { .origin = { 85, 60 }, .size = { 70, 22 } });
#ifdef PBL_COLOR
  text_layer_set_text_color(text_minute_u, GColorLightGray);
#else
  text_layer_set_text_color(text_minute_u, GColorBlack);
#endif
  text_layer_set_font(text_minute_u, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_background_color(text_minute_u, GColorClear);
  text_layer_set_text_alignment(text_minute_u, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_minute_u));

  // minute_d
  text_minute_d = text_layer_create((GRect) { .origin = { 85, 82 }, .size = { 70, 22 } });
#ifdef PBL_COLOR
  text_layer_set_text_color(text_minute_d, GColorLightGray);
#else
  text_layer_set_text_color(text_minute_d, GColorBlack);
#endif
  text_layer_set_font(text_minute_d, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_background_color(text_minute_d, GColorClear);
  text_layer_set_text_alignment(text_minute_d, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_minute_d));
  
  // Init the layer for the minute display
  seconds_layer = layer_create(bounds);
  layer_set_update_proc(seconds_layer, seconds_display_callback);
  layer_add_child(window_layer, seconds_layer);
}

/**
 * unloading window 
 */
static void window_unload(Window *window) {
   text_layer_destroy(text_month);
   text_layer_destroy(text_hour);
   text_layer_destroy(text_minute_u);
   text_layer_destroy(text_minute_d);
}

/**
 * handle time tick 
 */
static void handle_time_tick(struct tm *tick_time, TimeUnits units_changed) {
   time_t now = time(NULL);
   struct tm *t = localtime(&now);
   char  buffer[5];
   
   static int  last_month = -1, last_hour = -1, last_min_d = -1, last_min_u = -1;
      
   layer_mark_dirty(seconds_layer);
   
   if (last_month != t->tm_mon) {
      last_month = t->tm_mon;
      ca_initialize(&amonth_out, text_month, "", exitToTop, DURATION, 0);
      ca_initialize(&amonth_in, text_month, month_name[t->tm_mon], appearFromTop, DURATION, DELAY);
   }
   if (last_hour != t->tm_hour) {
      last_hour = t->tm_hour;
      snprintf (buffer,sizeof(buffer), "%d", t->tm_hour);
      ca_initialize(&ahour_out, text_hour, "", exitToLeft, DURATION, 0);
      ca_initialize(&ahour_in, text_hour, buffer, appearFromLeft, DURATION, DELAY);
   }
   if (last_min_u != (t->tm_min / 10)) {
      last_min_u = t->tm_min / 10;
      snprintf (buffer,sizeof(buffer), "%d", last_min_u);
      ca_initialize(&aminu_out, text_minute_u, "", exitToRight, DURATION, 0);
      ca_initialize(&aminu_in, text_minute_u, buffer, appearFromRight, DURATION, DELAY);
   }
   if (last_min_d != (t->tm_min % 10)) {
      last_min_d = t->tm_min % 10;
      snprintf (buffer,sizeof(buffer), "%d", last_min_d);
      ca_initialize(&amind_out, text_minute_d, "", exitToRight, DURATION, 0);
      ca_initialize(&amind_in, text_minute_d, buffer, appearFromRight, DURATION, DELAY);
   }
}

/**
 * init application 
 */
static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
  window_set_fullscreen(window, true);
  window_set_background_color(window, GColorWhite);
  tick_timer_service_subscribe(SECOND_UNIT, handle_time_tick);
}

/**
 * deinitialize application
 */
static void deinit(void) {
  window_destroy(window);
}

/**
 * main entry point
 */
int main(void) {
  init();
  app_event_loop();
  deinit();
}
