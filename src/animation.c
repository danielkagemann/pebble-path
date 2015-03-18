/**
 * animation module for handling two animations and an update between both
 * @author Daniel Kagemann @corefault
 * @created 13.03.2015
 *
 */
#include <pebble.h>
#include "animation.h"

/**
 * calculate the rectangle
 */
void ca_calc_rect (GRect orig, animationchain* self, enum animationdirection dir) {
   self->to = orig;
   self->from = orig;
   
   switch (dir) {
      case appearFromLeft:
         self->from.origin.x -= 144;
         break;
      case appearFromRight:
         self->from.origin.x += 144;
         break;
      case appearFromTop:
         self->from.origin.y -= 168;
         break;
      case appearFromBottom:
         self->from.origin.y += 168;
         break;
			
		case exitToLeft:
         self->to.origin.x -= 144;
			break;
		case exitToRight:
         self->to.origin.x += 144;
			break;
			
		case exitToTop:
			self->to.origin.y -= 168;
			break;
		case exitToBottom:
			self->to.origin.y += 168;
			break;
			
		case fadeToTop:
			self->to.size.h = 0;
			break;
		case fadeToBottom:
			self->from.size.h = 0;
			break;
   }
}

/**
 * started animation callback handler
 */
void ca_started(Animation *animation, void *data) {
   animationchain*	self = (animationchain*)data;
	if (strlen(self->textbuffer) > 0) {
		text_layer_set_text(self->layer, (char*)self->textbuffer);
	}
}

/**
 * create propertyanimation
 */
void ca_initialize(animationchain* self, TextLayer* layer, char* text, enum animationdirection dir, int duration, int delay) {

   // check previous animations
   if (self->prop != NULL) {
      property_animation_destroy(self->prop);
   }

	// set the rects
   GRect  frame  = layer_get_frame((Layer*)layer);
   
   ca_calc_rect(frame, self, dir);

	strcpy (self->textbuffer, text); // to avoid static vars all the time
   APP_LOG(APP_LOG_LEVEL_DEBUG, "initialized with %s", self->textbuffer);
   self->layer = layer;
   
   self->prop = property_animation_create_layer_frame((Layer*)layer, &self->from, &self->to);
   
   // set handler
   animation_set_handlers((Animation*) self->prop, (AnimationHandlers) {
       .started = (AnimationStartedHandler) ca_started,
       .stopped = NULL
     }, self);
   
     animation_set_duration((Animation*)self->prop, duration);
     animation_set_delay((Animation*)self->prop, delay);
     animation_set_curve((Animation*)self->prop, AnimationCurveEaseInOut);
   
   // now start it
   animation_schedule((Animation*) self->prop);
}

