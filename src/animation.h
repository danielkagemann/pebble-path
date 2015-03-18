/**
 * animation module for handling two animations and an update between both
 * @author Daniel Kagemann @corefault
 * @created 13.03.2015
 *
 */
 
 enum animationdirection { appearFromLeft = 0, 
	 								appearFromRight, 
									appearFromTop, 
									appearFromBottom, 
									exitToLeft,
									exitToRight,
									exitToTop,
									exitToBottom,
									fadeToTop, 
									fadeToBottom };
 
 typedef struct t_animationchain {
	char			  		textbuffer[20];
	PropertyAnimation	*prop;
	TextLayer         *layer;
   GRect          	from, to;
 } animationchain;
 
 void ca_initialize(animationchain* self, TextLayer* layer, char* text, enum animationdirection dir, int duration, int delay);
 