#include "pebble.h"

#define TAP_NOT_DATA true

static Window *s_main_window;
static Layer *s_layer_1;
static BitmapLayer *s_image_layer_1_1;
static BitmapLayer *s_image_layer_1_2;
static BitmapLayer *s_image_layer_1_3;
static BitmapLayer *s_image_layer_1_4;
static BitmapLayer *s_image_layer_2_1;
static BitmapLayer *s_image_layer_2_2;
static BitmapLayer *s_image_layer_2_3;
static BitmapLayer *s_image_layer_2_4;
static BitmapLayer *s_image_layer_3_1;
static BitmapLayer *s_image_layer_3_2;
static BitmapLayer *s_image_layer_3_3;
static BitmapLayer *s_image_layer_3_4;
static BitmapLayer *s_image_layer_4_1;
static BitmapLayer *s_image_layer_4_2;
static BitmapLayer *s_image_layer_4_3;
static BitmapLayer *s_image_layer_4_4;
static GBitmap *s_image_bitmap_on;
static GBitmap *s_image_bitmap_off;
static TextLayer *s_reg_time_layer;

static void setImage(BitmapLayer *bitmap_layer, int onOrOff) {
  if (onOrOff == 1) {
    bitmap_layer_set_bitmap(bitmap_layer, s_image_bitmap_on);
  } else {
    bitmap_layer_set_bitmap(bitmap_layer, s_image_bitmap_off);
  }
}

static void setImages(BitmapLayer *bitmap_layer1, BitmapLayer *bitmap_layer2, BitmapLayer *bitmap_layer3, BitmapLayer *bitmap_layer4, int numDigit) {
  setImage(bitmap_layer4, numDigit & 1);
  numDigit>>=1;
  setImage(bitmap_layer3, numDigit & 1);
  numDigit>>=1;
  setImage(bitmap_layer2, numDigit & 1);
  numDigit>>=1;
  setImage(bitmap_layer1, numDigit & 1);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  int ten_hour = (tick_time->tm_hour - (tick_time->tm_hour % 10))/10;
  int hour = tick_time->tm_hour % 10;
  int ten_min =  (tick_time->tm_min - (tick_time->tm_min % 10))/10;
  int minutes = tick_time->tm_min % 10;

  setImages(s_image_layer_1_1, s_image_layer_1_2, s_image_layer_1_3, s_image_layer_1_4, ten_hour);
  setImages(s_image_layer_2_1, s_image_layer_2_2, s_image_layer_2_3, s_image_layer_2_4, hour);
  setImages(s_image_layer_3_1, s_image_layer_3_2, s_image_layer_3_3, s_image_layer_3_4, ten_min);
  setImages(s_image_layer_4_1, s_image_layer_4_2, s_image_layer_4_3, s_image_layer_4_4, minutes);

}

void animation_stopped(Animation *animation, bool finished, void *data) {
    //Free the memory used by the Animation
    property_animation_destroy((PropertyAnimation*) animation);
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {
    //Declare animation
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
 
    //Set characteristics
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
 
    //Set stopped handler to free memory
    AnimationHandlers handlers = {
        //The reference to the stopped handler is the only one in the array
        .stopped = (AnimationStoppedHandler) animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
 
    //Start animation!
    animation_schedule((Animation*) anim);
}

void updateTimeRegular() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    //Use 2h hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    //Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_reg_time_layer, buffer);

  // Set start and end
  GRect from_frame = GRect(0, -60, 144, 60);
  GRect to_frame = GRect(0, 228, 144, 30);

//  animate_layer(text_layer_get_layer(s_reg_time_layer), &from_frame, &to_frame, 3000, 500);
}

static void tap_handler(AccelAxisType axis, int32_t direction) {
  updateTimeRegular();
}


static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, GColorBlack);
  GRect bounds = layer_get_frame(window_layer);

  GRect layer_1 = GRect(22,10,100,166);
  s_layer_1 = layer_create(layer_1);
  
  s_image_bitmap_on  = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WAVEFORM_ON);
  s_image_bitmap_off = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_WAVEFORM_OFF);

  s_image_layer_1_1 = bitmap_layer_create(GRect(0, 0, 25, 35));
  s_image_layer_1_2 = bitmap_layer_create(GRect(25, 0, 25, 35));
  s_image_layer_1_3 = bitmap_layer_create(GRect(50, 0, 25, 35));
  s_image_layer_1_4 = bitmap_layer_create(GRect(75, 0, 25, 35));
  s_image_layer_2_1 = bitmap_layer_create(GRect(0, 35, 25, 35));
  s_image_layer_2_2 = bitmap_layer_create(GRect(25, 35, 25, 35));
  s_image_layer_2_3 = bitmap_layer_create(GRect(50, 35, 25, 35));
  s_image_layer_2_4 = bitmap_layer_create(GRect(75, 35, 25, 35));
  s_image_layer_3_1 = bitmap_layer_create(GRect(0, 70, 25, 35));
  s_image_layer_3_2 = bitmap_layer_create(GRect(25, 70, 25, 35));
  s_image_layer_3_3 = bitmap_layer_create(GRect(50, 70, 25, 35));
  s_image_layer_3_4 = bitmap_layer_create(GRect(75, 70, 25, 35));
  s_image_layer_4_1 = bitmap_layer_create(GRect(0, 105, 25, 35));
  s_image_layer_4_2 = bitmap_layer_create(GRect(25, 105, 25, 35));
  s_image_layer_4_3 = bitmap_layer_create(GRect(50, 105, 25, 35));
  s_image_layer_4_4 = bitmap_layer_create(GRect(75, 105, 25, 35));

  bitmap_layer_set_bitmap(s_image_layer_1_1, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_1_2, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_1_3, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_1_4, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_2_1, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_2_2, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_2_3, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_2_4, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_3_1, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_3_2, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_3_3, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_3_4, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_4_1, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_4_2, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_4_3, s_image_bitmap_off);
  bitmap_layer_set_bitmap(s_image_layer_4_4, s_image_bitmap_off);

  layer_add_child(window_layer, s_layer_1);

  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_1_1));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_1_2));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_1_3));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_1_4));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_2_1));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_2_2));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_2_3));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_2_4));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_3_1));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_3_2));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_3_3));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_3_4));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_4_1));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_4_2));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_4_3));
  layer_add_child(s_layer_1, bitmap_layer_get_layer(s_image_layer_4_4));
  
  s_reg_time_layer = text_layer_create(GRect(0, -60, 144, 60));
  text_layer_set_background_color(s_reg_time_layer, GColorWhite);
  text_layer_set_text_color(s_reg_time_layer, GColorBlack);
  text_layer_set_text(s_reg_time_layer, "00:00");
  text_layer_set_font(s_reg_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
  text_layer_set_text_alignment(s_reg_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_reg_time_layer));

}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_reg_time_layer);
  layer_destroy(s_layer_1);
  bitmap_layer_destroy(s_image_layer_1_1);
  bitmap_layer_destroy(s_image_layer_1_2);
  bitmap_layer_destroy(s_image_layer_1_3);
  bitmap_layer_destroy(s_image_layer_1_4);
  bitmap_layer_destroy(s_image_layer_2_1);
  bitmap_layer_destroy(s_image_layer_2_2);
  bitmap_layer_destroy(s_image_layer_2_3);
  bitmap_layer_destroy(s_image_layer_2_4);
  bitmap_layer_destroy(s_image_layer_3_1);
  bitmap_layer_destroy(s_image_layer_3_2);
  bitmap_layer_destroy(s_image_layer_3_3);
  bitmap_layer_destroy(s_image_layer_3_4);
  bitmap_layer_destroy(s_image_layer_4_1);
  bitmap_layer_destroy(s_image_layer_4_2);
  bitmap_layer_destroy(s_image_layer_4_3);
  bitmap_layer_destroy(s_image_layer_4_4);
  gbitmap_destroy(s_image_bitmap_on);
  gbitmap_destroy(s_image_bitmap_off);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
